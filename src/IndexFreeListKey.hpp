#pragma once

#include <cstdint>
#include <StringPiece.hpp>


class IndexFreeListKey {
public:
    IndexFreeListKey();
    static bool Decode(StringPiece* slice, IndexFreeListKey* result);
    /*CONTENT_EXPORT*/ static std::string Encode(int64_t database_id,
                                             int64_t object_store_id,
                                             int64_t index_id);
    /*CONTENT_EXPORT*/ static std::string EncodeMaxKey(int64_t database_id,
                                                   int64_t object_store_id);
    int Compare(const IndexFreeListKey& other);
    int64_t ObjectStoreId() const;
    int64_t IndexId() const;
    //std::string DebugString() const;

private:
    int64_t object_store_id_;
    int64_t index_id_;
};
