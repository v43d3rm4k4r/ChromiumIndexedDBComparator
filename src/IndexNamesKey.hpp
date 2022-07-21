#pragma once

#include <cstdint>
#include <StringPiece.hpp>


class IndexNamesKey {
public:
    IndexNamesKey();
    // TODO(jsbell): We never use this to look up index ids, because a mapping
    // is kept at a higher level.
    static bool Decode(StringPiece* slice, IndexNamesKey* result);
    /*CONTENT_EXPORT*/static std::string Encode(int64_t database_id,
                                             int64_t object_store_id,
                                             const std::u16string& index_name);
    int Compare(const IndexNamesKey& other);
    //std::string DebugString() const;

    std::u16string index_name() const { return index_name_; }

private:
    int64_t object_store_id_;
    std::u16string index_name_;
};
