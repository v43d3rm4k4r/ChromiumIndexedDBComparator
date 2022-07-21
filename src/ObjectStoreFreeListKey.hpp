#pragma once

#include <cstdint>
#include <StringPiece.hpp>


class ObjectStoreFreeListKey {
public:
    ObjectStoreFreeListKey();
    static bool Decode(StringPiece* slice, ObjectStoreFreeListKey* result);
    /*CONTENT_EXPORT*/ static std::string Encode(int64_t database_id,
                                             int64_t object_store_id);
    /*CONTENT_EXPORT*/ static std::string EncodeMaxKey(int64_t database_id);
    int64_t ObjectStoreId() const;
    int Compare(const ObjectStoreFreeListKey& other);
    //std::string DebugString() const;

private:
    int64_t object_store_id_;
};
