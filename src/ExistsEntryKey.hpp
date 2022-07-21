#pragma once

#include <IndexedDBKey.hpp>
#include <CodingUtils.hpp>
#include <memory>


class ExistsEntryKey {
public:
    ExistsEntryKey() = default;
    ~ExistsEntryKey() = default;

    ExistsEntryKey(const ExistsEntryKey&) = delete;
    ExistsEntryKey& operator=(const ExistsEntryKey&) = delete;

    static bool Decode(StringPiece* slice, ExistsEntryKey* result);
    /*CONTENT_EXPORT*/ static std::string Encode(int64_t database_id,
                                             int64_t object_store_id,
                                             const std::string& encoded_key);
    static std::string Encode(int64_t database_id,
                              int64_t object_store_id,
                              const IndexedDBKey& user_key);
    //std::string DebugString() const;

    std::unique_ptr<IndexedDBKey> user_key() const;

private:
    static const int64_t kSpecialIndexNumber;

    std::string encoded_user_key_;
};
