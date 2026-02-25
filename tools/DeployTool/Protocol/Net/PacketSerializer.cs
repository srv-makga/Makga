using System.Buffers.Binary;
using System.Text;
using System.Text.Json;

namespace DeployTool.Protocol.Net;

// 7바이트 헤더: [packet_id: uint16 LE][packet_size: uint32 LE][packet_key: uint8]
public static class PacketHeader
{
	public const int Size = 7;

	public static void Write(Span<byte> buf, PacketId id, int payloadLen, byte key = 0)
	{
		BinaryPrimitives.WriteUInt16LittleEndian(buf[..2], (ushort)id);
		BinaryPrimitives.WriteUInt32LittleEndian(buf[2..6], (uint)payloadLen);
		buf[6] = key;
	}

	public static (PacketId id, int payloadLen, byte key) Read(ReadOnlySpan<byte> buf)
	{
		var id         = (PacketId)BinaryPrimitives.ReadUInt16LittleEndian(buf[..2]);
		var payloadLen = (int)BinaryPrimitives.ReadUInt32LittleEndian(buf[2..6]);
		var key        = buf[6];
		return (id, payloadLen, key);
	}
}

public static class PacketSerializer
{
	// @brief 객체를 JSON 직렬화하여 헤더+페이로드 바이트 배열 반환
	public static byte[] Serialize<T>(PacketId id, T payload, byte key = 0)
	{
		var json        = JsonSerializer.SerializeToUtf8Bytes(payload);
		var buf         = new byte[PacketHeader.Size + json.Length];
		PacketHeader.Write(buf.AsSpan(0, PacketHeader.Size), id, json.Length, key);
		json.CopyTo(buf, PacketHeader.Size);
		return buf;
	}

	// @brief 헤더만 있는 패킷 (페이로드 없음)
	public static byte[] SerializeEmpty(PacketId id, byte key = 0)
	{
		var buf = new byte[PacketHeader.Size];
		PacketHeader.Write(buf, id, 0, key);
		return buf;
	}

	// @brief JSON 페이로드를 T로 역직렬화
	public static T? Deserialize<T>(ReadOnlySpan<byte> payload)
	{
		if (null == payload || 0 == payload.Length)
			return default;
		return JsonSerializer.Deserialize<T>(payload);
	}

	// @brief 스트림에서 헤더+페이로드 1패킷 읽기 (비동기)
	public static async Task<(PacketId id, byte[] payload, byte key)?> ReadPacketAsync(
		Stream stream, CancellationToken ct = default)
	{
		var headerBuf = new byte[PacketHeader.Size];
		if (!await ReadExactAsync(stream, headerBuf, ct))
			return null;

		var (id, payloadLen, key) = PacketHeader.Read(headerBuf);

		if (0 == payloadLen)
			return (id, Array.Empty<byte>(), key);

		var payloadBuf = new byte[payloadLen];
		if (!await ReadExactAsync(stream, payloadBuf, ct))
			return null;

		return (id, payloadBuf, key);
	}

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
