using CSharp.Queue.Models;
using CSharp.Queue.Repository;
using CSharp.Queue.Services.ServerResolver;

namespace CSharp.Queue.Services;

public class QueueService
{
    private readonly QueueRepository _repository;
    private readonly ISessionValidationService _sessionValidation;
    private readonly IWorldServerResolver _serverResolver;
    private readonly ILogger<QueueService> _logger;
    private readonly IConfiguration _configuration;

    private TimeSpan TicketTtl => TimeSpan.FromMinutes(_configuration.GetValue("Queue:TicketTtlMinutes", 30));
    private TimeSpan ReadyTtl => TimeSpan.FromSeconds(_configuration.GetValue("Queue:ReadyTtlSeconds", 60));

    public QueueService(
        QueueRepository repository,
        ISessionValidationService sessionValidation,
        IWorldServerResolver serverResolver,
        ILogger<QueueService> logger,
        IConfiguration configuration)
    {
        _repository = repository;
        _sessionValidation = sessionValidation;
        _serverResolver = serverResolver;
        _logger = logger;
        _configuration = configuration;
    }

    public async Task<JoinResponse?> JoinQueueAsync(JoinRequest request)
    {
        var accountUid = await _sessionValidation.ValidateSessionTokenAsync(request.SessionToken);
        if (!accountUid.HasValue)
        {
            _logger.LogWarning("Invalid session token for accountUid: {AccountUid}", request.AccountUid);
            return null;
        }

        if (accountUid.Value != request.AccountUid)
        {
            _logger.LogWarning("Session token mismatch for accountUid: {AccountUid}", request.AccountUid);
            return null;
        }

        var existingTicketId = await _repository.GetAccountQueueAsync(accountUid.Value);
        if (existingTicketId != null)
        {
            var existingTicket = await _repository.GetTicketAsync(existingTicketId);
            if (existingTicket != null && existingTicket.Status != QueueStatus.Expired)
            {
                var position = await GetPositionAsync(request.WorldId, existingTicketId);
                return new JoinResponse
                {
                    TicketId = existingTicketId,
                    Status = existingTicket.Status,
                    Position = position,
                    ServerInfo = existingTicket.ServerInfo
                };
            }
        }

        var capacity = await _repository.GetCapacityAsync(request.WorldId);
        var online = await _repository.GetOnlineAsync(request.WorldId);
        
        var ticketId = Guid.NewGuid().ToString("N");
        var ticket = new QueueTicket
        {
            TicketId = ticketId,
            AccountUid = accountUid.Value,
            WorldId = request.WorldId,
            CreatedAt = DateTime.UtcNow
        };

        if (online < capacity)
        {
            var serverInfo = await _serverResolver.ResolveAsync(request.WorldId);
            if (serverInfo == null)
            {
                _logger.LogError("Failed to resolve server for worldId: {WorldId}", request.WorldId);
                return null;
            }

            var entryToken = Guid.NewGuid().ToString("N");
            serverInfo.EntryToken = entryToken;

            await _repository.SetEntryTokenAsync(entryToken, new EntryTokenInfo
            {
                AccountUid = accountUid.Value,
                WorldId = request.WorldId,
                ServerId = serverInfo.ServerId ?? ""
            }, ReadyTtl);

            ticket.Status = QueueStatus.Ready;
            ticket.ReadyAt = DateTime.UtcNow;
            ticket.ServerInfo = serverInfo;

            await _repository.SetTicketAsync(ticket, ReadyTtl);
            await _repository.SetAccountQueueAsync(accountUid.Value, ticketId, ReadyTtl);

            _logger.LogInformation("User {AccountUid} granted immediate entry to {WorldId}", accountUid.Value, request.WorldId);

            return new JoinResponse
            {
                TicketId = ticketId,
                Status = QueueStatus.Ready,
                Position = 0,
                ServerInfo = serverInfo
            };
        }
        else
        {
            ticket.Status = QueueStatus.Waiting;
            var timestamp = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds();
            
            await _repository.AddToQueueAsync(request.WorldId, ticketId, timestamp);
            await _repository.SetTicketAsync(ticket, TicketTtl);
            await _repository.SetAccountQueueAsync(accountUid.Value, ticketId, TicketTtl);

            var position = await GetPositionAsync(request.WorldId, ticketId);

            _logger.LogInformation("User {AccountUid} added to queue for {WorldId} at position {Position}", 
                accountUid.Value, request.WorldId, position);

            return new JoinResponse
            {
                TicketId = ticketId,
                Status = QueueStatus.Waiting,
                Position = position,
                ServerInfo = null
            };
        }
    }

    public async Task<QueueStatusResponse?> GetStatusAsync(string ticketId)
    {
        var ticket = await _repository.GetTicketAsync(ticketId);
        if (ticket == null)
        {
            return null;
        }

        var position = await GetPositionAsync(ticket.WorldId, ticketId);

        return new QueueStatusResponse
        {
            Status = ticket.Status,
            Position = position,
            ServerInfo = ticket.ServerInfo
        };
    }

    public async Task<bool> LeaveQueueAsync(string ticketId)
    {
        var ticket = await _repository.GetTicketAsync(ticketId);
        if (ticket == null)
        {
            return false;
        }

        await _repository.RemoveFromQueueAsync(ticket.WorldId, ticketId);
        await _repository.DeleteTicketAsync(ticketId);
        await _repository.DeleteAccountQueueAsync(ticket.AccountUid);

        if (ticket.ServerInfo?.EntryToken != null)
        {
            await _repository.DeleteEntryTokenAsync(ticket.ServerInfo.EntryToken);
        }

        _logger.LogInformation("User {AccountUid} left queue for {WorldId}", ticket.AccountUid, ticket.WorldId);

        return true;
    }

    private async Task<int> GetPositionAsync(string worldId, string ticketId)
    {
        var ticket = await _repository.GetTicketAsync(ticketId);
        if (ticket == null || ticket.Status != QueueStatus.Waiting)
        {
            return 0;
        }

        var position = await _repository.GetQueuePositionAsync(worldId, ticketId);
        return position >= 0 ? (int)position + 1 : 0;
    }

    public async Task<long> GetAvailableSlotsAsync(string worldId)
    {
        var capacity = await _repository.GetCapacityAsync(worldId);
        var online = await _repository.GetOnlineAsync(worldId);
        return Math.Max(0, capacity - online);
    }
}
