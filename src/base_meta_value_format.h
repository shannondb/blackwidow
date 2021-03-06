//  Copyright (c) 2017-present The blackwidow Authors.  All rights reserved.
//  This source code is licensed under the BSD-style license found in the
//  LICENSE file in the root directory of this source tree. An additional grant
//  of patent rights can be found in the PATENTS file in the same directory.

#ifndef SRC_BASE_META_VALUE_FORMAT_H_
#define SRC_BASE_META_VALUE_FORMAT_H_

#include <string>

#include "src/base_value_format.h"

namespace blackwidow {

class BaseMetaValue : public InternalValue {
 public:
  explicit BaseMetaValue(const Slice& user_value) :
    InternalValue(user_value) {
  }
  virtual size_t AppendTimestampAndVersion() override {
    size_t usize = user_value_.size();
    char* dst = start_;
    memcpy(dst, user_value_.data(), usize);
    dst += usize;
    EncodeFixed32(dst, version_);
    dst += sizeof(int32_t);
    EncodeFixed32(dst, timestamp_);
    return usize + 2 * sizeof(int32_t);
  }

  int32_t UpdateVersion() {
    int64_t unix_time;
    shannon::Env::Default()->GetCurrentTime(&unix_time);
    if (version_ >= static_cast<int32_t>(unix_time)) {
      version_++;
    } else {
      version_ = static_cast<int32_t>(unix_time);
    }
    return version_;
  }

  virtual const Slice Encode() {
      if (user_value_.size() >= kDefaultValueSuffixLength + sizeof(int32_t)) {
          char* dst = const_cast<char *>(user_value_.data());
          EncodeFixed32(dst + sizeof(uint32_t), version_);
          EncodeFixed32(dst + sizeof(uint32_t) * 2, timestamp_);
      }
      return user_value_;
  }
};

class ParsedBaseMetaValue : public ParsedInternalValue {
 public:
  // Use this constructor after shannon::DB::Get();
  explicit ParsedBaseMetaValue(std::string* internal_value_str) :
    ParsedInternalValue(internal_value_str) {
    if (internal_value_str->size() >= kBaseMetaValueSuffixLength) {
      user_value_ = Slice(internal_value_str->data(),
          internal_value_str->size() - kBaseMetaValueSuffixLength);
      version_ = DecodeFixed32(internal_value_str->data() +
            internal_value_str->size() - sizeof(int32_t) * 2);
      timestamp_ = DecodeFixed32(internal_value_str->data() +
            internal_value_str->size() - sizeof(int32_t));
    }
    count_ = DecodeFixed32(internal_value_str->data());
  }

  // Use this constructor in shannon::CompactionFilter::Filter();
  explicit ParsedBaseMetaValue(const Slice& internal_value_slice) :
    ParsedInternalValue(internal_value_slice) {
    if (internal_value_slice.size() >= kBaseMetaValueSuffixLength) {
      user_value_ = Slice(internal_value_slice.data(),
          internal_value_slice.size() - kBaseMetaValueSuffixLength);
      version_ = DecodeFixed32(internal_value_slice.data() +
            internal_value_slice.size() - sizeof(int32_t) * 2);
      timestamp_ = DecodeFixed32(internal_value_slice.data() +
            internal_value_slice.size() - sizeof(int32_t));
    }
    count_ = DecodeFixed32(internal_value_slice.data());
  }

  virtual void StripSuffix() override {
    if (value_ != nullptr) {
      value_->erase(value_->size() - kBaseMetaValueSuffixLength,
          kBaseMetaValueSuffixLength);
    }
  }

  virtual void SetVersionToValue() override {
    if (value_ != nullptr) {
      char* dst = const_cast<char*>(value_->data()) + value_->size() -
        kBaseMetaValueSuffixLength;
      EncodeFixed32(dst, version_);
    }
  }

  virtual void SetTimestampToValue() override {
    if (value_ != nullptr) {
      char* dst = const_cast<char*>(value_->data()) + value_->size() -
        sizeof(int32_t);
      EncodeFixed32(dst, timestamp_);
    }
  }
  static const size_t kBaseMetaValueSuffixLength = 2 * sizeof(int32_t);

  int32_t InitialMetaValue() {
    this->set_count(0);
    this->set_timestamp(0);
    return this->UpdateVersion();
  }

  int32_t count() {
    return count_;
  }

  void set_count(int32_t count) {
    count_ = count;
    if (value_ != nullptr) {
      char* dst = const_cast<char*>(value_->data());
      EncodeFixed32(dst, count_);
    }
  }

  void ModifyCount(int32_t delta) {
    count_ += delta;
    if (value_ != nullptr) {
      char* dst = const_cast<char*>(value_->data());
      EncodeFixed32(dst, count_);
    }
  }

  int32_t UpdateVersion() {
    int64_t unix_time;
    shannon::Env::Default()->GetCurrentTime(&unix_time);
    if (version_ >= static_cast<int32_t>(unix_time)) {
      version_++;
    } else {
      version_ = static_cast<int32_t>(unix_time);
    }
    SetVersionToValue();
    return version_;
  }

 private:
  int32_t count_;
};


class ParsedSetsMetaValue : public ParsedInternalValue {
 public:
  // Use this constructor after shannon::DB::Get();
  explicit ParsedSetsMetaValue(std::string* internal_value_str) :
    ParsedInternalValue(internal_value_str) {
    if (internal_value_str->size() >= kBaseSetsValueSuffixLength) {
      user_value_ = Slice(internal_value_str->data(),
          internal_value_str->size() - kBaseSetsValueSuffixLength);
      version_ = DecodeFixed32(internal_value_str->data() + sizeof(int32_t) );
      timestamp_ = DecodeFixed32(internal_value_str->data() +  sizeof(int32_t) * 2 );
    }
    count_ = DecodeFixed32(internal_value_str->data());
  }

  // Use this constructor in shannon::CompactionFilter::Filter();
  explicit ParsedSetsMetaValue(const Slice& internal_value_slice) :
    ParsedInternalValue(internal_value_slice) {
    if (internal_value_slice.size() >= kBaseSetsValueSuffixLength) {
      user_value_ = Slice(internal_value_slice.data(),
          internal_value_slice.size() - kBaseSetsValueSuffixLength);
      version_ = DecodeFixed32(internal_value_slice.data() + sizeof(int32_t) );
      timestamp_ = DecodeFixed32(internal_value_slice.data() + sizeof(int32_t) * 2 );
    }
    count_ = DecodeFixed32(internal_value_slice.data());
  }

  virtual void StripSuffix() override {
    if (value_ != nullptr) {
      value_->erase(value_->size() ,
          kBaseSetsValueSuffixLength);
    }
  }

  virtual void SetVersionToValue() override {
    if (value_ != nullptr) {
      char* dst = const_cast<char*>(value_->data()) + sizeof(int32_t);
      EncodeFixed32(dst, version_);
    }
  }

  virtual void SetTimestampToValue() override {
    if (value_ != nullptr) {
      char* dst = const_cast<char*>(value_->data()) + sizeof(int32_t) * 2 ;
      EncodeFixed32(dst, timestamp_);
    }
  }
  static const size_t kBaseSetsValueSuffixLength = 3 * sizeof(int32_t);

  int32_t InitialMetaValue() {
    this->set_count(0);
    this->set_timestamp(0);
    return this->UpdateVersion();
  }

  int32_t count() {
    return count_;
  }

  void set_count(int32_t count) {
    count_ = count;
    if (value_ != nullptr) {
      char* dst = const_cast<char*>(value_->data());
      EncodeFixed32(dst, count_);
    }
  }

  void ModifyCount(int32_t delta) {
    count_ += delta;
    if (value_ != nullptr) {
      char* dst = const_cast<char*>(value_->data());
      EncodeFixed32(dst, count_);
    }
  }

  int32_t UpdateVersion() {
    int64_t unix_time;
    shannon::Env::Default()->GetCurrentTime(&unix_time);
    if (version_ >= static_cast<int32_t>(unix_time)) {
      version_++;
    } else {
      version_ = static_cast<int32_t>(unix_time);
    }
    SetVersionToValue();
    return version_;
  }

 private:
  int32_t count_;
};



typedef BaseMetaValue HashesMetaValue;
typedef ParsedBaseMetaValue ParsedHashesMetaValue;
typedef BaseMetaValue SetsMetaValue;
// typedef ParsedBaseMetaValue ParsedSetsMetaValue;
typedef BaseMetaValue ZSetsMetaValue;
typedef ParsedBaseMetaValue ParsedZSetsMetaValue;

}  //  namespace blackwidow
#endif  // SRC_BASE_META_VALUE_FORMAT_H_
