#pragma once

#include <cstdint>
#include <StringPiece.hpp>


class KeyPrefix {
public:
    // These are serialized to disk; any new items must be appended, and none can
    // be deleted.
    enum Type {
        GLOBAL_METADATA   = 0,
        DATABASE_METADATA = 1,
        OBJECT_STORE_DATA = 2,
        EXISTS_ENTRY      = 3,
        INDEX_DATA        = 4,
        INVALID_TYPE      = 5,
        BLOB_ENTRY        = 6
    };

    static const size_t kMaxDatabaseIdSizeBits = 3;
    static const size_t kMaxObjectStoreIdSizeBits = 3;
    static const size_t kMaxIndexIdSizeBits = 2;

    static const size_t kMaxDatabaseIdSizeBytes =
            1ULL << kMaxDatabaseIdSizeBits;  // 8
    static const size_t kMaxObjectStoreIdSizeBytes =
            1ULL << kMaxObjectStoreIdSizeBits;                               // 8
    static const size_t kMaxIndexIdSizeBytes = 1ULL << kMaxIndexIdSizeBits;  // 4

    static const size_t kMaxDatabaseIdBits =
            kMaxDatabaseIdSizeBytes * 8 - 1;  // 63
    static const size_t kMaxObjectStoreIdBits =
            kMaxObjectStoreIdSizeBytes * 8 - 1;                          // 63
    static const size_t kMaxIndexIdBits = kMaxIndexIdSizeBytes * 8 - 1;  // 31

    static const int64_t kMaxDatabaseId =
            (1ULL << kMaxDatabaseIdBits) - 1;  // max signed int64_t
    static const int64_t kMaxObjectStoreId =
            (1ULL << kMaxObjectStoreIdBits) - 1;  // max signed int64_t
    static const int64_t kMaxIndexId =
            (1ULL << kMaxIndexIdBits) - 1;  // max signed int32_t

    static const int64_t kInvalidId = -1;

    KeyPrefix();
    explicit KeyPrefix(int64_t database_id);
    KeyPrefix(int64_t database_id, int64_t object_store_id);
    KeyPrefix(int64_t database_id, int64_t object_store_id, int64_t index_id);
    static KeyPrefix CreateWithSpecialIndex(int64_t database_id,
                                            int64_t object_store_id,
                                            int64_t index_id);

    static bool Decode(StringPiece* slice, KeyPrefix* result); /// !!!
    std::string Encode() const;
    static std::string EncodeEmpty();
    int Compare(const KeyPrefix& other) const;

    //CONTENT_EXPORT
    static bool IsValidDatabaseId(int64_t database_id);
    static bool IsValidObjectStoreId(int64_t index_id);
    static bool IsValidIndexId(int64_t index_id);
    static bool ValidIds(int64_t database_id,
                         int64_t object_store_id,
                         int64_t index_id) {
        return IsValidDatabaseId(database_id) &&
                IsValidObjectStoreId(object_store_id) && IsValidIndexId(index_id);
    }
    static bool ValidIds(int64_t database_id, int64_t object_store_id) {
        return IsValidDatabaseId(database_id) &&
                IsValidObjectStoreId(object_store_id);
    }

    std::string DebugString();

    Type type() const;

    int64_t database_id_;
    int64_t object_store_id_;
    int64_t index_id_;

private:
    // Special constructor for CreateWithSpecialIndex()
    KeyPrefix(enum Type,
              int64_t database_id,
              int64_t object_store_id,
              int64_t index_id);

    static std::string EncodeInternal(int64_t database_id, int64_t object_store_id, int64_t index_id);
};
