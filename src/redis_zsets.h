//  Copyright (c) 2017-present The blackwidow Authors.  All rights reserved.
//  This source code is licensed under the BSD-style license found in the
//  LICENSE file in the root directory of this source tree. An additional grant
//  of patent rights can be found in the PATENTS file in the same directory.

#ifndef SRC_REDIS_ZSETS_h
#define SRC_REDIS_ZSETS_h

#include <unordered_set>

#include "src/redis.h"
#include "src/custom_comparator.h"
#include "blackwidow/blackwidow.h"

namespace blackwidow {

class RedisZSets : public Redis {
  public:
    RedisZSets(BlackWidow* const bw, const DataType& type);
    ~RedisZSets();

    // Common Commands
    virtual Status Open(const BlackwidowOptions& bw_options,
                        const std::string& db_path) override;
    virtual Status CompactRange(const shannon::Slice* begin,
                                const shannon::Slice* end,
                                const ColumnFamilyType& type = kMetaAndData) override;
    virtual Status GetProperty(const std::string& property, uint64_t* out) override;
    Status ScanKeyNum(KeyInfo* key_info) override;
    virtual Status ScanKeys(const std::string& pattern,
                            std::vector<std::string>* keys) override;

    // ZSets Commands
    Status ZAdd(const Slice& key,
                const std::vector<ScoreMember>& score_members,
                int32_t* ret);
    Status ZCard(const Slice& key, int32_t* card);
    Status ZCount(const Slice& key,
                  double min,
                  double max,
                  bool left_close,
                  bool right_close,
                  int32_t* ret);
    Status ZIncrby(const Slice& key,
                   const Slice& member,
                   double increment,
                   double* ret);
    Status ZRange(const Slice& key,
                  int32_t start,
                  int32_t stop,
                  std::vector<ScoreMember>* score_members);
    Status ZRangebyscore(const Slice& key,
                         double min,
                         double max,
                         bool left_close,
                         bool right_close,
                         std::vector<ScoreMember>* score_members);
    Status ZRank(const Slice& key,
                 const Slice& member,
                 int32_t* rank);
    Status ZRem(const Slice& key,
                std::vector<std::string> members,
                int32_t* ret);
    Status ZRemrangebyrank(const Slice& key,
                           int32_t start,
                           int32_t stop,
                           int32_t* ret);
    Status ZRemrangebyscore(const Slice& key,
                            double min,
                            double max,
                            bool left_close,
                            bool right_close,
                            int32_t* ret);
    Status ZRevrange(const Slice& key,
                     int32_t start,
                     int32_t stop,
                     std::vector<ScoreMember>* score_members);
    Status ZRevrangebyscore(const Slice& key,
                            double min,
                            double max,
                            bool left_close,
                            bool right_close,
                            std::vector<ScoreMember>* score_members);
    Status ZRevrank(const Slice& key,
                    const Slice& member,
                    int32_t* rank);
    Status ZScore(const Slice& key, const Slice& member, double* score);
    Status ZUnionstore(const Slice& destination,
                       const std::vector<std::string>& keys,
                       const std::vector<double>& weights,
                       const AGGREGATE agg,
                       int32_t* ret);
    Status ZInterstore(const Slice& destination,
                       const std::vector<std::string>& keys,
                       const std::vector<double>& weights,
                       const AGGREGATE agg,
                       int32_t* ret);
    Status ZRangebylex(const Slice& key,
                       const Slice& min,
                       const Slice& max,
                       bool left_close,
                       bool right_close,
                       std::vector<std::string>* members);
    Status ZLexcount(const Slice& key,
                     const Slice& min,
                     const Slice& max,
                     bool left_close,
                     bool right_close,
                     int32_t* ret);
    Status ZRemrangebylex(const Slice& key,
                          const Slice& min,
                          const Slice& max,
                          bool left_close,
                          bool right_close,
                          int32_t* ret);
    Status ZScan(const Slice& key, int64_t cursor, const std::string& pattern,
                 int64_t count, std::vector<ScoreMember>* score_members, int64_t* next_cursor);
    Status PKScanRange(const Slice& key_start, const Slice& key_end,
                       const Slice& pattern, int32_t limit,
                       std::vector<std::string>* keys, std::string* next_key);
    Status PKRScanRange(const Slice& key_start, const Slice& key_end,
                        const Slice& pattern, int32_t limit,
                        std::vector<std::string>* keys, std::string* next_key);
    Status DelTimeout(BlackWidow * bw,std::string * key) ;
    Status RealDelTimeout(BlackWidow * bw,std::string * key) ;
    // Keys Commands
    virtual Status Expire(const Slice& key, int32_t ttl) override;
    virtual Status Del(const Slice& key) override;
    virtual bool Scan(const std::string& start_key, const std::string& pattern,
                      std::vector<std::string>* keys,
                      int64_t* count, std::string* next_key) override;
    virtual Status Expireat(const Slice& key, int32_t timestamp) override;
    virtual Status Persist(const Slice& key) override;
    virtual Status TTL(const Slice& key, int64_t* timestamp) override;
    virtual std::vector<shannon::ColumnFamilyHandle*> GetColumnFamilyHandles() override;
    virtual Status AddDelKey(BlackWidow * bw,const string & str);
    virtual Status LogAdd(const Slice& key, const Slice& value, int32_t cf_index) override;
    virtual Status LogDelete(const Slice& key, int32_t cf_index) override;
    virtual Status LogDeleteDB() override;
    virtual Status LogCreateDB(int32_t db_index = 0) override;
    virtual void CloseDB() override;
    // Iterate all data
    void ScanDatabase();

  private:
    std::vector<shannon::ColumnFamilyHandle*> handles_;
};

} // namespace blackwidow
#endif  //  SRC_REDIS_ZSETS_h
