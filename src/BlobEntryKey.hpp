#pragma once

#include <StringPiece.hpp>
#include <IndexedDBKey.hpp>


class /*CONTENT_EXPORT*/ BlobEntryKey {
public:
    BlobEntryKey() : database_id_(0), object_store_id_(0) {}
    static bool Decode(StringPiece* slice, BlobEntryKey* result);
    static bool FromObjectStoreDataKey(StringPiece* slice,
                                       BlobEntryKey* result);
    static std::string ReencodeToObjectStoreDataKey(StringPiece* slice);
    static std::string EncodeMinKeyForObjectStore(int64_t database_id,
                                                  int64_t object_store_id);
    static std::string EncodeStopKeyForObjectStore(int64_t database_id,
                                                   int64_t object_store_id);
    static std::string Encode(int64_t database_id,
                              int64_t object_store_id,
                              const IndexedDBKey& user_key);
    std::string Encode() const;
    std::string DebugString() const;

    int64_t database_id() const { return database_id_; }
    int64_t object_store_id() const { return object_store_id_; }

private:
    static const int64_t kSpecialIndexNumber;

    static std::string Encode(int64_t database_id,
                              int64_t object_store_id,
                              const std::string& encoded_user_key);
    int64_t database_id_;
    int64_t object_store_id_;
    // This is the user's ObjectStoreDataKey, not the BlobEntryKey itself.
    std::string encoded_user_key_;
};

