#pragma once

#include <ostream>

#include "writer.h"

CEREAL_RAPIDJSON_NAMESPACE_BEGIN

template <typename Stream = std::ostream, typename Allocator = CrtAllocator>
class BufferOStreamWrapper {
public:
  typedef typename Stream::char_type Ch;
  explicit BufferOStreamWrapper(Stream &stream, Allocator *allocator = nullptr)
      : stream_(stream), allocator_(allocator) {}

  ~BufferOStreamWrapper() {
    Flush();

    Allocator::Free(top_);
    CEREAL_RAPIDJSON_DELETE(ownAllocator_);
  }

  BufferOStreamWrapper(const BufferOStreamWrapper &) = delete;
  BufferOStreamWrapper &operator=(const BufferOStreamWrapper &) = delete;

  void Put(Ch c) {
    ReserveMore(1);
    PutUnsafe(c);
  }

  void PutUnsafe(Ch c) {
    *end_ = c;
    ++end_;
  }

  Ch *Push(size_t cnt) {
    ReserveMore(cnt);
    auto *const res = end_;
    end_ += cnt;
    return res;
  }

  void Pop(size_t cnt) { end_ -= cnt; }

  void Flush() {
    FlushInternal();
    stream_.flush();
  }

  // Not implemented
  [[nodiscard]] char Peek() const {
    CEREAL_RAPIDJSON_ASSERT(false);
    return 0;
  }

  char Take() {
    CEREAL_RAPIDJSON_ASSERT(false);
    return 0;
  }

  [[nodiscard]] size_t Tell() const {
    CEREAL_RAPIDJSON_ASSERT(false);
    return 0;
  }
  char *PutBegin() {
    CEREAL_RAPIDJSON_ASSERT(false);
    return nullptr;
  }
  size_t PutEnd(char *) {
    CEREAL_RAPIDJSON_ASSERT(false);
    return 0;
  }

  void ReserveMore(size_t extra_cnt) {
    if (CEREAL_RAPIDJSON_LIKELY(GetBufLeftCapacity() >= extra_cnt))
      return;

    const size_t cur_capacity = GetBufCapacity();

    if (cur_capacity >= MAX_BUF_SIZE) {
      FlushInternal();
      if (extra_cnt > cur_capacity) {
        Resize(extra_cnt);
      }
    } else if (top_ == nullptr) {
      FirstAllocMemory(extra_cnt);
    } else {
      Resize(extra_cnt);
    }
  }

private:
  static constexpr size_t MAX_BUF_SIZE = 1024 * 1;
  static constexpr size_t INITIAL_BUF_SIZE = 64;
  Stream &stream_;

  Ch *top_ = nullptr;
  Ch *end_ = nullptr;
  Ch *capacity_end_ = nullptr;

  Allocator *allocator_ = nullptr;
  Allocator *ownAllocator_ = nullptr;

  void ClearBuf() { end_ = top_; }

  void FirstAllocMemory(size_t cnt) {
    if (allocator_ == nullptr) {
      ownAllocator_ = allocator_ = CEREAL_RAPIDJSON_NEW(Allocator());
    }

    size_t capacity = INITIAL_BUF_SIZE;
    while (capacity < cnt) {
      capacity *= 2;
    }

    end_ = top_ =
        reinterpret_cast<Ch *>(allocator_->Malloc(capacity * sizeof(Ch)));
    capacity_end_ = top_ + capacity;
  }

  void Resize(size_t extra_cnt) {
    const size_t cur_size = GetBufSize();
    const size_t cur_cap = GetBufCapacity();
    size_t new_cap = cur_cap;
    const size_t required_cap = cur_size + extra_cnt;
    do {
      new_cap *= 2;
    } while (required_cap > new_cap);

    top_ = reinterpret_cast<Ch *>(allocator_->Realloc(top_, cur_cap, new_cap));
    end_ = top_ + cur_size;
    capacity_end_ = top_ + new_cap;
  }

  size_t GetBufLeftCapacity() const {
    return static_cast<size_t>(capacity_end_ - end_) / sizeof(Ch);
  }
  size_t GetBufCapacity() const {
    return static_cast<size_t>(capacity_end_ - top_) / sizeof(Ch);
  }
  size_t GetBufSize() const {
    return static_cast<size_t>(end_ - top_) / sizeof(Ch);
  }

  void FlushInternal() {
    stream_.write(top_, GetBufSize());
    ClearBuf();
  }
};

template <typename Allocator>
inline void PutReserve(BufferOStreamWrapper<Allocator> &stream, size_t count) {
  stream.ReserveMore(count);
}

template <typename Allocator>
inline void PutUnsafe(BufferOStreamWrapper<Allocator> &stream,
                      typename BufferOStreamWrapper<Allocator>::Ch c) {
  stream.PutUnsafe(c);
}

//! Implement specialized version of PutN() with memset() for better
//! performance.
template <typename Allocator>
inline void PutN(BufferOStreamWrapper<Allocator> &stream,
                 typename BufferOStreamWrapper<Allocator>::Ch c, size_t n) {
  std::memset(stream.Push(n), c, n * sizeof(c));
}

template <> inline bool Writer<BufferOStreamWrapper<>>::WriteInt(int i) {
  char *buffer = os_->Push(11);
  const char *end = internal::i32toa(i, buffer);
  os_->Pop(static_cast<size_t>(11 - (end - buffer)));
  return true;
}

template <> inline bool Writer<BufferOStreamWrapper<>>::WriteUint(unsigned u) {
  char *buffer = os_->Push(10);
  const char *end = internal::u32toa(u, buffer);
  os_->Pop(static_cast<size_t>(10 - (end - buffer)));
  return true;
}

template <>
inline bool Writer<BufferOStreamWrapper<>>::WriteInt64(int64_t i64) {
  char *buffer = os_->Push(21);
  const char *end = internal::i64toa(i64, buffer);
  os_->Pop(static_cast<size_t>(21 - (end - buffer)));
  return true;
}

template <>
inline bool Writer<BufferOStreamWrapper<>>::WriteUint64(uint64_t u) {
  char *buffer = os_->Push(20);
  const char *end = internal::u64toa(u, buffer);
  os_->Pop(static_cast<size_t>(20 - (end - buffer)));
  return true;
}

template <> inline bool Writer<BufferOStreamWrapper<>>::WriteDouble(double d) {
  if (internal::Double(d).IsNanOrInf()) {
    // Note: This code path can only be reached if
    // (CEREAL_RAPIDJSON_WRITE_DEFAULT_FLAGS & kWriteNanAndInfFlag).
    if (!(kWriteDefaultFlags & kWriteNanAndInfFlag))
      return false;

    if (internal::Double(d).IsNan()) {
      PutReserve(*os_, 3);
      PutUnsafe(*os_, 'N');
      PutUnsafe(*os_, 'a');
      PutUnsafe(*os_, 'N');
      return true;
    }
    if (internal::Double(d).Sign()) {
      PutReserve(*os_, 9);
      PutUnsafe(*os_, '-');
    } else {
      PutReserve(*os_, 8);
    }
    PutUnsafe(*os_, 'I');
    PutUnsafe(*os_, 'n');
    PutUnsafe(*os_, 'f');
    PutUnsafe(*os_, 'i');
    PutUnsafe(*os_, 'n');
    PutUnsafe(*os_, 'i');
    PutUnsafe(*os_, 't');
    PutUnsafe(*os_, 'y');
    return true;
  }

  char *buffer = os_->Push(25);
  char *end = internal::dtoa(d, buffer, maxDecimalPlaces_);
  os_->Pop(static_cast<size_t>(25 - (end - buffer)));
  return true;
}

CEREAL_RAPIDJSON_NAMESPACE_END
