#pragma once

#include <cstdint>
#include <StringPiece.hpp>


class ObjectStoreMetaDataKey {
public:
    enum MetaDataType {
        NAME = 0,
        KEY_PATH = 1,
        AUTO_INCREMENT = 2,
        EVICTABLE = 3,
        LAST_VERSION = 4,
        MAX_INDEX_ID = 5,
        HAS_KEY_PATH = 6,
        KEY_GENERATOR_CURRENT_NUMBER = 7
    };

    // From the IndexedDB specification.
    static const int64_t kKeyGeneratorInitialNumber;

    ObjectStoreMetaDataKey();
    static bool Decode(StringPiece* slice, ObjectStoreMetaDataKey* result);
    /*CONTENT_EXPORT*/ static std::string Encode(int64_t database_id,
                                             int64_t object_store_id,
                                             unsigned char meta_data_type);
    /*CONTENT_EXPORT*/ static std::string EncodeMaxKey(int64_t database_id);
    /*CONTENT_EXPORT*/ static std::string EncodeMaxKey(int64_t database_id,
                                                   int64_t object_store_id);
    int64_t ObjectStoreId() const;
    unsigned char MetaDataType() const;
    int Compare(const ObjectStoreMetaDataKey& other);
    //std::string DebugString() const;

private:
    int64_t object_store_id_;
    unsigned char meta_data_type_;
};
