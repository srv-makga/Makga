module;

#include <iostream>
#include <memory>
#include <shared_mutex>

export module makga.lib.ring_buffer;

export namespace makga::lib {
export template<typename T>
class RingBuffer
{
	using Mutex_t = std::shared_mutex;
	using ReadLock = ::std::shared_lock<Mutex_t>;
	using WriteLock = ::std::unique_lock<Mutex_t>;

public:
	RingBuffer()
		: buffer_(nullptr)
		, buffer_size_(0)
		, write_offset_(0)
		, read_offset_(0)
	{
	}

	virtual ~RingBuffer()
	{
		buffer_.reset();
	}

	bool Initialize()
	{
		WriteLock lock(mutex_);

		buffer_size_ = 0;
		write_offset_ = 0;
		read_offset_ = 0;
	}

	void Finalize()
	{
		WriteLock lock(mutex_);

		buffer_.reset();
		buffer_size_ = 0;
		write_offset_ = 0;
		read_offset_ = 0;
	}

	std::size_t Write(T* data, std::size_t count)
	{
		if (nullptr == data || 0 == count)
		{
			return 0;
		}

		if (AvailableWriteSize() < count)
		{
			return 0;
		}

		WriteLock lock(mutex_);

		if (nullptr == buffer_)
		{
			return 0;
		}

		const std::size_t remain_size = buffer_size_ - write_offset_;
		// 버퍼 끝까지 남은 공간이 충분함
		if (remain_size > count)
		{
			::memcpy(WritePosition(), data, (sizeof(T) * count));
		}
		else
		{
			::memcpy(WritePosition(), data, (sizeof(T) * remain_size));
			::memcpy(buffer_, data + remain_size, (sizeof(T) * (count - remain_size)));
		}

		AddWriteOffset(count);
		return count;
	}

	std::size_t Read(T* data, std::size_t count)
	{
		if (nullptr == data || 0 == count)
		{
			return 0;
		}

		if (UsingSize() < count)
		{
			return 0;
		}

		WriteLock lock(mutex_);

		if (nullptr == buffer_)
		{
			return 0;
		}

		std::size_t remain_size = buffer_size_ - read_offset_;
		if (remain_size > count)
		{
			::memcpy(data, ReadPosition(), (sizeof(T) * count));
		}
		else
		{
			::memcpy(data, ReadPosition(), (sizeof(T) * remain_size));
			::memcpy(data + remain_size, buffer_, sizeof(T) * (count - remain_size));
		}

		AddReadOffset(count);
		return count;
	}

	// @brief 사용 가능한 사이즈
	std::size_t AvailableWriteSize() const
	{
		ReadLock lock(mutex_);

		if (nullptr == buffer_)
		{
			return 0;
		}

		if (write_offset_ >= read_offset_)
		{
			return buffer_size_ - (write_offset_ - read_offset_) - 1;
		}

		return read_offset_ - write_offset_ - 1;
	}

	// @brief 사용중인 사이즈
	std::size_t UsingSize() const
	{
		ReadLock lock(mutex_);

		if (nullptr == buffer_)
		{
			return 0;
		}

		if (write_offset_ >= read_offset_)
		{
			return write_offset_ - read_offset_;
		}
		
		return buffer_size_ - (read_offset_ - write_offset_);
	}
	
	// @brief 버퍼 할당
	void AllocateBuffer(std::size_t size)
	{
		if (0 == size)
		{
			return;
		}

		WriteLock lock(mutex_);

		buffer_ = std::make_unique<T[]>(size);
		buffer_size_ = size;
		write_offset_ = 0;
		read_offset_ = 0;
	}

	bool ValidBuffer() const
	{
		ReadLock lock(mutex_);
		return nullptr != buffer_;
	}

	std::size_t BufferSize() const
	{
		ReadLock lock(mutex_);
		return buffer_size_;
	}

	std::size_t WriteOffset() const
	{
		ReadLock lock(mutex_);
		return write_offset_;
	}

	std::size_t ReadOffset() const
	{
		ReadLock lock(mutex_);
		return read_offset_;
	}

private:
	void AddWriteOffset(std::size_t size)
	{
		write_offset_ += size;

		if (write_offset_ >= buffer_size_)
		{
			write_offset_ -= buffer_size_;
		}
	}

	void AddReadOffset(std::size_t size)
	{
		read_offset_ += size;

		if (read_offset_ >= buffer_size_)
		{
			read_offset_ -= buffer_size_;
		}
	}

	inline T* WritePosition() const { return buffer_[write_offset_]; }
	inline T* ReadPosition() const { return buffer_[read_offset_]; }

protected:
	mutable std::shared_mutex mutex_;
	std::unique_ptr<T> buffer_;
	std::size_t buffer_size_;
	std::size_t write_offset_;
	std::size_t read_offset_;
};
} // namespace makga::lib