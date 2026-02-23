using System.Diagnostics;
using System.Net.Sockets;
using AdminTool.Models;

namespace AdminTool.Services;

/// <summary>
/// TCP client for Makga game servers.
/// Packet format (servers/common/net_packet.h):
///   [packet_id: uint16][packet_size: uint32][packet_key: uint8] = 7 bytes header
///   Followed by FlatBuffer binary payload.
/// All fields are little-endian.
/// </summary>
public class ServerClient
{
    private const int HeaderSize = 7; // 2 + 4 + 1

    public async Task<(bool Online, long? Ms)> PingAsync(string host, int port, int timeoutMs = 2000)
    {
        var sw = Stopwatch.StartNew();
        try
        {
            using var tcp = new TcpClient();
            var connect = tcp.ConnectAsync(host, port);
            if (await Task.WhenAny(connect, Task.Delay(timeoutMs)) != connect)
                return (false, null);
            sw.Stop();
            return (true, sw.ElapsedMilliseconds);
        }
        catch { return (false, null); }
    }

    public async Task<PacketSendResponse> SendPacketAsync(
        string host, int port,
        ushort packetId, byte[] payload, byte packetKey = 0,
        int timeoutMs = 5000)
    {
        var sw = Stopwatch.StartNew();
        try
        {
            int totalSize = HeaderSize + payload.Length;
            var packet = new byte[totalSize];
            BitConverter.TryWriteBytes(packet.AsSpan(0, 2), packetId);       // uint16 LE
            BitConverter.TryWriteBytes(packet.AsSpan(2, 4), (uint)totalSize); // uint32 LE
            packet[6] = packetKey;
            payload.CopyTo(packet, HeaderSize);

            using var tcp = new TcpClient();
            var connect = tcp.ConnectAsync(host, port);
            if (await Task.WhenAny(connect, Task.Delay(timeoutMs)) != connect)
                return Fail("Connection timeout", sw);

            var stream = tcp.GetStream();
            stream.WriteTimeout = timeoutMs;
            stream.ReadTimeout = timeoutMs;

            await stream.WriteAsync(packet);

            // Read response header
            var header = new byte[HeaderSize];
            if (!await ReadExactAsync(stream, header, HeaderSize))
                return Fail("Failed to read response header", sw);

            ushort respId   = BitConverter.ToUInt16(header, 0);
            uint   respSize = BitConverter.ToUInt32(header, 2);
            byte   respKey  = header[6];

            byte[] respBody = Array.Empty<byte>();
            int bodyLen = (int)respSize - HeaderSize;
            if (bodyLen > 0)
            {
                respBody = new byte[bodyLen];
                if (!await ReadExactAsync(stream, respBody, bodyLen))
                    return Fail("Failed to read response body", sw);
            }

            sw.Stop();
            return new PacketSendResponse
            {
                Success = true,
                ResponsePacketId = respId,
                ResponseKey = respKey,
                ResponseHex = Convert.ToHexString(respBody),
                ElapsedMs = sw.ElapsedMilliseconds,
            };
        }
        catch (Exception ex)
        {
            return Fail(ex.Message, sw);
        }
    }

    private static PacketSendResponse Fail(string msg, Stopwatch sw)
    {
        sw.Stop();
        return new PacketSendResponse { Success = false, Error = msg, ElapsedMs = sw.ElapsedMilliseconds };
    }

    private static async Task<bool> ReadExactAsync(NetworkStream stream, byte[] buf, int count)
    {
        int read = 0;
        while (read < count)
        {
            int n = await stream.ReadAsync(buf.AsMemory(read, count - read));
            if (n == 0) return false;
            read += n;
        }
        return true;
    }
}
