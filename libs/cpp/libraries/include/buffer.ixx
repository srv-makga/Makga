module;

#include <cstring>

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

    virtual ~Buffer() { buffer_.reset(); }

    bool Initialize()
    {
        WriteLock lock(mutex_);
        buffer_size_ = 0;
        write_offset_ = 0;
        read_offset_ = 0;
        return true;
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
        if (data == nullptr || count == 0) return 0;
        WriteLock lock(mutex_);
        if (buffer_ == nullptr || count > buffer_size_ - write_offset_) return 0;
        std::memcpy(buffer_.get() + write_offset_, data, sizeof(T) * count);
        write_offset_ += count;
        return count;
    }

    std::size_t Read(T* data, std::size_t count)
    {
        if (data == nullptr || count == 0) return 0;
        WriteLock lock(mutex_);
        if (buffer_ == nullptr || count > write_offset_ - read_offset_) return 0;
        std::memcpy(data, buffer_.get() + read_offset_, sizeof(T) * count);
        read_offset_ += count;
        return count;
    }

    // Linear, non-wrapping buffer. Call PullBuffer() after consuming a prefix.
    std::size_t AvailableWriteSize() const
    {
        ReadLock lock(mutex_);
        return buffer_ == nullptr ? 0 : buffer_size_ - write_offset_;
    }

    std::size_t UsingSize() const
    {
        ReadLock lock(mutex_);
        return buffer_ == nullptr ? 0 : write_offset_ - read_offset_;
    }

    void AllocateBuffer(std::size_t size)
    {
        if (size == 0) return;
        WriteLock lock(mutex_);
        buffer_ = std::make_unique<T[]>(size);
        buffer_size_ = size;
        write_offset_ = 0;
        read_offset_ = 0;
    }

    bool ValidBuffer() const
    {
        ReadLock lock(mutex_);
        return buffer_ != nullptr;
    }

    // Preserve the unread tail, not the already consumed prefix.
    void PullBuffer()
    {
        WriteLock lock(mutex_);
        if (buffer_ == nullptr || read_offset_ == 0) return;
        const std::size_t unread = write_offset_ - read_offset_;
        if (unread != 0)
            std::memmove(buffer_.get(), buffer_.get() + read_offset_, sizeof(T) * unread);
        write_offset_ = unread;
        read_offset_ = 0;
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

    void AddWriteOffset(std::size_t size)
    {
        WriteLock lock(mutex_);
        if (buffer_ == nullptr || size > buffer_size_ - write_offset_) return;
        write_offset_ += size;
    }

    void AddReadOffset(std::size_t size)
    {
        WriteLock lock(mutex_);
        if (buffer_ == nullptr || size > write_offset_ - read_offset_) return;
        read_offset_ += size;
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
