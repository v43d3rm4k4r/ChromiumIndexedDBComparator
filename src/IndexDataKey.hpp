#pragma once

#include <vector>
#include <cassert>
#include <memory>

#include <CodingUtils.hpp>
#include <IndexedDBKey.hpp>


class IndexDataKey {
public:
    /*CONTENT_EXPORT*/ IndexDataKey();
    /*CONTENT_EXPORT*/ IndexDataKey(IndexDataKey&& other);

    IndexDataKey(const IndexDataKey&) = delete;
    IndexDataKey& operator=(const IndexDataKey&) = delete;

    /*CONTENT_EXPORT*/ ~IndexDataKey();

    /*CONTENT_EXPORT*/ static bool Decode(StringPiece* slice,
                                      IndexDataKey* result);
    /*CONTENT_EXPORT*/ static std::string Encode(
            int64_t database_id,
            int64_t object_store_id,
            int64_t index_id,
            const std::string& encoded_user_key,
            const std::string& encoded_primary_key,
            int64_t sequence_number);
    static std::string Encode(int64_t database_id,
                              int64_t object_store_id,
                              int64_t index_id,
                              const IndexedDBKey& user_key);
    /*CONTENT_EXPORT*/ static std::string Encode(
            int64_t database_id,
            int64_t object_store_id,
            int64_t index_id,
            const IndexedDBKey& user_key,
            const IndexedDBKey& user_primary_key);
    /*CONTENT_EXPORT*/ static std::string EncodeMinKey(int64_t database_id,
                                                   int64_t object_store_id,
                                                   int64_t index_id);

    // An index's keys are guaranteed to fall in [EncodeMinKey(), EncodeMaxKey()]
    /*CONTENT_EXPORT*/ static std::string EncodeMaxKey(int64_t database_id,
                                                   int64_t object_store_id,
                                                   int64_t index_id);
    int64_t DatabaseId() const;
    int64_t ObjectStoreId() const;
    int64_t IndexId() const;
    std::unique_ptr<IndexedDBKey> user_key() const;
    std::unique_ptr<IndexedDBKey> primary_key() const;

    /*CONTENT_EXPORT*/ std::string Encode() const;

    //std::string DebugString() const;

private:
    int64_t database_id_;
    int64_t object_store_id_;
    int64_t index_id_;
    std::string encoded_user_key_;
    std::string encoded_primary_key_;
    int64_t sequence_number_;
};
