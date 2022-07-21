#pragma once

#include <cstdint>
#include <memory>

#include <StringPiece.hpp>
#include <CodingUtils.hpp>
#include <IndexedDBKey.hpp>


class ObjectStoreDataKey {
public:
    static const int64_t kSpecialIndexNumber;

    ObjectStoreDataKey() = default;
    ~ObjectStoreDataKey() = default;

    static bool Decode(StringPiece* slice, ObjectStoreDataKey* result);
    /*CONTENT_EXPORT*/ static std::string Encode(int64_t database_id,
                                             int64_t object_store_id,
                                             const std::string encoded_user_key);
    /*CONTENT_EXPORT*/ static std::string Encode(int64_t database_id,
                                             int64_t object_store_id,
                                             const IndexedDBKey& user_key);
    //std::string DebugString() const;

    std::unique_ptr<IndexedDBKey> user_key() const;

private:
    std::string encoded_user_key_;
};
