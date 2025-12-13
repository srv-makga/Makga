module;

export module makga.lib.buffer;

import <iostream>;
import <memory>;
import makga.lib.lock;

export namespace makga::lib {
export template<typename T>
class Buffer
{
public:
	Buffer()
		: buffer_(nullptr)
		, buffer_size_(0)
		, write_offset_(0)
		, read_offset_(0)
	{
	}

	virtual ~Buffer()
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

		::memcpy(WritePosition(), data, (sizeof(T) * count));

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

		::memcpy(data, ReadPosition(), (sizeof(T) * count));

		AddReadOffset(count);
		return count;
	}

	// @brief 사용 가능한 사이즈
	std::size_t AvailableWriteSize() const
	{
		ReadLock lock(mutex_);
		return BufferSize() - UsingSize();
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
		
		return (buffer_size_ + write_offset_) - read_offset_;
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

	// @brief 버퍼 사용 메모리 갱신
	void PullBuffer()
	{
		WriteLock lock(mutex_);
		if (0 == read_offset_)
		{
			return;
		}

		::memcpy(buffer_, buffer_ + read_offset_, sizeof(buffer_[0]) * read_offset_);
		m_write_offset -= m_read_offset;
		m_read_offset = 0;
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

protected:
	void AddWriteOffset(std::size_t size)
	{
		write_offset_ = ((write_offset_ + size) % buffer_size_);
	}

	void AddReadOffset(std::size_t size)
	{
		read_offset_ = ((read_offset_ + size) % buffer_size_);
	}

	inline T* WritePosition() const { return buffer_.get() + write_offset_; }
	inline T* ReadPosition() const { return buffer_.get() + read_offset_; }

protected:
	mutable SharedMutex mutex_;
	std::unique_ptr<T[]> buffer_;
	std::size_t buffer_size_;
	std::size_t write_offset_;
	std::size_t read_offset_;
};
} // namespace makga::lib