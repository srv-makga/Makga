module;

#include <vector>
#include <string>
#include <cstdint>
#include <cstring>
#include <type_traits>
#include <algorithm>

export module makga.lib.binary_writer;

namespace makga::lib
{
export class BinaryWriter
{
	using SizeType = uint16_t;
public:

	BinaryWriter(std::size_t capacity = 0)
	{
		if (0 < capacity)
		{
			data_.reserve(capacity);
		}
	}

	~BinaryWriter() = default;

	template<typename T>
	bool Write(const T& value)
	{
		static_assert(std::is_trivially_copyable_v<T>, "BinaryWriter::Write requires trivially copyable types");

		const std::size_t write_size = sizeof(T);
		const std::size_t old_size = data_.size();

		if (old_size + write_size > data_.capacity())
		{
			const std::size_t new_cap = std::max<std::size_t>(data_.capacity() ? data_.capacity() * 2 : write_size, old_size + write_size);
			data_.reserve(new_cap);
		}

		data_.resize(old_size + write_size);
		std::memcpy(data_.data() + old_size, &value, write_size);
		return true;
	}

	template<>
	bool Write(const std::string& value)
	{
		if (true == value.empty())
		{
			return false;
		}

		SizeType size_field = static_cast<SizeType>(value.size());
		const std::size_t write_size = sizeof(SizeType) + value.size();
		const std::size_t old_size = data_.size();

		if (old_size + write_size > data_.capacity())
		{
			const std::size_t new_cap = std::max<std::size_t>(data_.capacity() ? data_.capacity() * 2 : write_size, old_size + write_size);
			data_.reserve(new_cap);
		}

		data_.resize(old_size + write_size);
		std::memcpy(data_.data() + old_size, &size_field, sizeof(SizeType));
		std::memcpy(data_.data() + old_size + sizeof(SizeType), value.data(), value.size());
		return true;
	}

	bool Write(const char* value)
	{
		if (nullptr == value)
		{
			return false;
		}

		std::size_t str_size = std::strlen(value);
		if (0 == str_size)
		{
			return false;
		}

		SizeType size_field = static_cast<SizeType>(str_size);
		const std::size_t write_size = sizeof(SizeType) + str_size;
		const std::size_t old_size = data_.size();

		if (old_size + write_size > data_.capacity())
		{
			const std::size_t new_cap = std::max<std::size_t>(data_.capacity() ? data_.capacity() * 2 : write_size, old_size + write_size);
			data_.reserve(new_cap);
		}

		data_.resize(old_size + write_size);
		std::memcpy(data_.data() + old_size, &size_field, sizeof(SizeType));
		std::memcpy(data_.data() + old_size + sizeof(SizeType), value, str_size);
		return true;
	}

	template<typename T>
	bool Write(const std::vector<T>& values)
	{
		SizeType elem_count = static_cast<SizeType>(values.size());

		if (false == Write(elem_count))
		{
			return false;
		}

		// 이미 write된 값을 롤백할 수 있나
		for (const auto& value : values)
		{
			if (false == Write(value))
			{
				return false;
			}
		}

		return true;
	}

	template<typename T>
	bool Read(T& value)
	{
		static_assert(std::is_trivially_copyable_v<T>, "BinaryWriter::Read requires trivially copyable types");
		const std::size_t read_size = sizeof(T);
		const std::size_t old_size = data_.size();
		if (read_size > old_size)
		{
			return false;
		}

		std::memcpy(&value, data_.data() + read_pos_, read_size);
		read_pos_ += read_size;

		return true;
	}

	template<>
	bool Read(std::string& value)
	{
		SizeType str_size = 0;
		if (false == Read<SizeType>(str_size))
		{
			read_pos_ -= sizeof(SizeType);
			return false;
		}

		const std::size_t old_size = data_.size();
		if (read_pos_ + str_size > old_size)
		{
			read_pos_ -= sizeof(SizeType);
			return false;
		}

		value.assign(data_.data() + read_pos_, str_size);
		read_pos_ += str_size;
		return true;
	}

	bool Read(char* value, std::size_t length)
	{
		if (nullptr == value || 0 == length)
		{
			return false;
		}

		SizeType str_size = 0;
		if (false == Read<SizeType>(str_size))
		{
			read_pos_ -= sizeof(SizeType);
			return false;
		}

		return true;
	}

	template<typename T>
	bool Read(std::vector<T>& values)
	{
		SizeType elem_count = static_cast<SizeType>(values.size());
		if (false == Read<SizeType>(elem_count))
		{
			return false;
		}

		values.clear();

		for (SizeType i = 0; i < elem_count; ++i)
		{
			T value;
			if (false == Read<T>(value))
			{
				return false;
			}

			values.push_back(value);
		}

		return !values.empty();
	}

	char* GetData()
	{
		return data_.empty() ? nullptr : data_.data();
	}

	std::size_t GetSize() const
	{
		return data_.size();
	}

private:
	std::vector<char> data_;
	std::size_t read_pos_ = 0;
};
} // namespace makga::lib