using System.Buffers.Binary;
using System.Text;
using System.Text.Json;

namespace DeployTool.Common.Net;

/// <summary>
/// Provides packet header and payload serialization/deserialization for binary protocol.
/// Header format: [packet_id: uint16 LE][packet_size: uint32 LE][packet_key: uint8] (7 bytes total).
/// </summary>
public static class PacketHeader
{
	/// <summary>
	/// Size of packet header in bytes (7)
	/// </summary>
	public const int Size = 7;

	/// <summary>
	/// Writes packet header information to a buffer.
	/// </summary>
	/// <param name="buf">Target buffer (minimum 7 bytes)</param>
	/// <param name="id">Packet ID</param>
	/// <param name="payloadLen">Length of payload in bytes</param>
	/// <param name="key">Optional packet key</param>
	public static void Write(Span<byte> buf, PacketId id, int payloadLen, byte key = 0)
	{
		BinaryPrimitives.WriteUInt16LittleEndian(buf[..2], (ushort)id);
		BinaryPrimitives.WriteUInt32LittleEndian(buf[2..6], (uint)payloadLen);
		buf[6] = key;
	}

	/// <summary>
	/// Reads packet header information from a buffer.
	/// </summary>
	/// <param name="buf">Source buffer (minimum 7 bytes)</param>
	/// <returns>Tuple of packet ID, payload length, and packet key</returns>
	public static (PacketId id, int payloadLen, byte key) Read(ReadOnlySpan<byte> buf)
	{
		var id         = (PacketId)BinaryPrimitives.ReadUInt16LittleEndian(buf[..2]);
		var payloadLen = (int)BinaryPrimitives.ReadUInt32LittleEndian(buf[2..6]);
		var key        = buf[6];
		return (id, payloadLen, key);
	}
}

/// <summary>
/// Handles binary packet serialization and deserialization with JSON payloads.
/// Implements variable-length packet protocol with size limit protection (50MB max).
/// </summary>
public static class PacketSerializer
{
	// .NET API: JsonSerializerOptions를 static readonly로 캐시하여 반복 생성 오버헤드 제거
	private static readonly JsonSerializerOptions DefaultOptions = new();

	/// <summary>
	/// Serializes an object to JSON and wraps it with packet header and payload.
	/// </summary>
	/// <typeparam name="T">Payload type</typeparam>
	/// <param name="id">Packet ID</param>
	/// <param name="payload">Object to serialize</param>
	/// <param name="key">Optional packet key</param>
	/// <returns>Complete packet bytes (header + payload)</returns>
	public static byte[] Serialize<T>(PacketId id, T payload, byte key = 0)
	{
		var json        = JsonSerializer.SerializeToUtf8Bytes(payload, DefaultOptions);
		var buf         = new byte[PacketHeader.Size + json.Length];
		PacketHeader.Write(buf.AsSpan(0, PacketHeader.Size), id, json.Length, key);
		json.CopyTo(buf, PacketHeader.Size);
		return buf;
	}

	/// <summary>
	/// Creates a packet with header only (no payload).
	/// </summary>
	/// <param name="id">Packet ID</param>
	/// <param name="key">Optional packet key</param>
	/// <returns>Header-only packet bytes</returns>
	public static byte[] SerializeEmpty(PacketId id, byte key = 0)
	{
		var buf = new byte[PacketHeader.Size];
		PacketHeader.Write(buf, id, 0, key);
		return buf;
	}

	/// <summary>
	/// Deserializes a JSON payload to the specified type.
	/// </summary>
	/// <typeparam name="T">Target type</typeparam>
	/// <param name="payload">JSON payload bytes</param>
	/// <returns>Deserialized object or null if payload is empty</returns>
	public static T? Deserialize<T>(ReadOnlySpan<byte> payload)
	{
		if (null == payload || 0 == payload.Length)
			return default;
		return JsonSerializer.Deserialize<T>(payload, DefaultOptions);
	}

	/// <summary>
	/// Asynchronously reads one complete packet from a stream (header + payload).
	/// Enforces maximum payload size limit (50MB) to prevent DoS attacks.
	/// </summary>
	/// <param name="stream">Network stream to read from</param>
	/// <param name="ct">Cancellation token</param>
	/// <returns>Packet tuple (ID, payload, key) or null if stream closed</returns>
	public static async Task<(PacketId id, byte[] payload, byte key)?> ReadPacketAsync(
		Stream stream, CancellationToken ct = default)
	{
		const int MaxPayloadSize = 52_428_800; // 50MB 제한
		var headerBuf = new byte[PacketHeader.Size];
		if (!await ReadExactAsync(stream, headerBuf, ct))
			return null;

		var (id, payloadLen, key) = PacketHeader.Read(headerBuf);

		// 페이로드 크기 검증
		if (payloadLen < 0 || payloadLen > MaxPayloadSize)
			return null;

		if (0 == payloadLen)
			return (id, Array.Empty<byte>(), key);

		var payloadBuf = new byte[payloadLen];
		if (!await ReadExactAsync(stream, payloadBuf, ct))
			return null;

		return (id, payloadBuf, key);
	}

	/// <summary>
	/// Asynchronously reads exactly the specified number of bytes from a stream.
	/// Handles partial reads by looping until all bytes are received or stream closes.
	/// </summary>
	/// <param name="stream">Stream to read from</param>
	/// <param name="buf">Buffer to fill</param>
	/// <param name="ct">Cancellation token</param>
	/// <returns>True if all bytes were read, false if stream closed before completion</returns>
	private static async Task<bool> ReadExactAsync(Stream stream, byte[] buf, CancellationToken ct)
	{
		var read = 0;
		while (read < buf.Length)
		{
			var n = await stream.ReadAsync(buf.AsMemory(read), ct);
			if (0 == n)
				return false;
			read += n;
		}
		return true;
	}
}
