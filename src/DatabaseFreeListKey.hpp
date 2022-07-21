#pragma once

#include <StringPiece.hpp>


class DatabaseFreeListKey {
public:
    DatabaseFreeListKey();
    static bool Decode(StringPiece* slice, DatabaseFreeListKey* result);
    /*CONTENT_EXPORT*/ static std::string Encode(int64_t database_id);
    /*CONTENT_EXPORT*/ static std::string EncodeMaxKey();
    int64_t DatabaseId() const;
    int Compare(const DatabaseFreeListKey& other) const;
    //std::string DebugString() const;

private:
    int64_t database_id_;
};


