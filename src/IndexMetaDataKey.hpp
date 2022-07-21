#pragma once

#include <cstdint>
#include <StringPiece.hpp>


class IndexMetaDataKey {
public:
    enum MetaDataType {
        NAME = 0,
        UNIQUE = 1,
        KEY_PATH = 2,
        MULTI_ENTRY = 3
    };

    IndexMetaDataKey();
    static bool Decode(StringPiece* slice, IndexMetaDataKey* result);
    /*CONTENT_EXPORT*/ static std::string Encode(int64_t database_id,
                                             int64_t object_store_id,
                                             int64_t index_id,
                                             uint8_t meta_data_type);
    /*CONTENT_EXPORT*/ static std::string EncodeMaxKey(int64_t database_id,
                                                   int64_t object_store_id);
    /*CONTENT_EXPORT*/ static std::string EncodeMaxKey(int64_t database_id,
                                                   int64_t object_store_id,
                                                   int64_t index_id);
    int Compare(const IndexMetaDataKey& other);
    //std::string DebugString() const;

    int64_t IndexId() const;
    uint8_t meta_data_type() const { return meta_data_type_; }

private:
    int64_t object_store_id_;
    int64_t index_id_;
    uint8_t meta_data_type_;
};
