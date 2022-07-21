#include <KeyPrefix.hpp>
#include <CodingUtils.hpp>


KeyPrefix::KeyPrefix()
    : database_id_(INVALID_TYPE),
      object_store_id_(INVALID_TYPE),
      index_id_(INVALID_TYPE) {}

KeyPrefix::KeyPrefix(int64_t database_id)
    : database_id_(database_id), object_store_id_(0), index_id_(0) {
    ////DCHECK(KeyPrefix::IsValidDatabaseId(database_id));
}

KeyPrefix::KeyPrefix(int64_t database_id, int64_t object_store_id)
    : database_id_(database_id),
      object_store_id_(object_store_id),
      index_id_(0) {
    ////DCHECK(KeyPrefix::IsValidDatabaseId(database_id));
    ////DCHECK(KeyPrefix::IsValidObjectStoreId(object_store_id));
}

KeyPrefix::KeyPrefix(int64_t database_id,
                     int64_t object_store_id,
                     int64_t index_id)
    : database_id_(database_id),
      object_store_id_(object_store_id),
      index_id_(index_id) {
    ////DCHECK(KeyPrefix::IsValidDatabaseId(database_id));
    ////DCHECK(KeyPrefix::IsValidObjectStoreId(object_store_id));
    ////DCHECK(KeyPrefix::IsValidIndexId(index_id));
}

KeyPrefix::KeyPrefix(enum Type type,
                     int64_t database_id,
                     int64_t object_store_id,
                     int64_t index_id)
    : database_id_(database_id),
      object_store_id_(object_store_id),
      index_id_(index_id) {
    ////DCHECK_EQ(type, INVALID_TYPE);
    ////DCHECK(KeyPrefix::IsValidDatabaseId(database_id));
    ////DCHECK(KeyPrefix::IsValidObjectStoreId(object_store_id));
}

bool KeyPrefix::Decode(StringPiece* slice, KeyPrefix* result) {

    uint8_t first_byte;
    if (!DecodeByte(slice, &first_byte))
        return false;

    size_t database_id_bytes = ((first_byte >> 5) & 0x7) + 1;
    size_t object_store_id_bytes = ((first_byte >> 2) & 0x7) + 1;
    size_t index_id_bytes = (first_byte & 0x3) + 1;

    if (database_id_bytes + object_store_id_bytes + index_id_bytes >
            slice->size())
        return false;

    {
        StringPiece tmp(slice->data(), database_id_bytes); // slice->begin() !!
        if (!DecodeInt(&tmp, &result->database_id_))
            return false;
    }
    slice->remove_prefix(database_id_bytes);
    {
        StringPiece tmp(slice->data(), object_store_id_bytes); // slice->begin() !!
        if (!DecodeInt(&tmp, &result->object_store_id_))
            return false;
    }
    slice->remove_prefix(object_store_id_bytes);
    {
        StringPiece tmp(slice->data(), index_id_bytes); // slice->begin() !!
        if (!DecodeInt(&tmp, &result->index_id_))
            return false;
    }
    slice->remove_prefix(index_id_bytes);
    return true;
}

int KeyPrefix::Compare(const KeyPrefix &other) const {
    //DCHECK(database_id_, kInvalidId);
    //DCHECK(object_store_id_, kInvalidId);
    //DCHECK(index_id_, kInvalidId);

    if (database_id_ != other.database_id_)
        return CompareInts(database_id_, other.database_id_);
    if (object_store_id_ != other.object_store_id_)
        return CompareInts(object_store_id_, other.object_store_id_);
    if (index_id_ != other.index_id_)
        return CompareInts(index_id_, other.index_id_);
    return 0;
}

KeyPrefix::Type KeyPrefix::type() const {
    //DCHECK_NE(database_id_, kInvalidId);
    //DCHECK_NE(object_store_id_, kInvalidId);
    //DCHECK_NE(index_id_, kInvalidId);

    if (!database_id_)
      return GLOBAL_METADATA;
    if (!object_store_id_)
      return DATABASE_METADATA;
    if (index_id_ == kObjectStoreDataIndexId)
      return OBJECT_STORE_DATA;
    if (index_id_ == kExistsEntryIndexId)
      return EXISTS_ENTRY;
    if (index_id_ == kBlobEntryIndexId)
      return BLOB_ENTRY;
    if (index_id_ >= kMinimumIndexId)
      return INDEX_DATA;

    //NOTREACHED();
    return INVALID_TYPE;
}

std::string KeyPrefix::EncodeEmpty() {
    const std::string result(4, 0);
    //DCHECK_EQ(EncodeInternal(0, 0, 0), std::string(4, 0));
    return result;
}

std::string KeyPrefix::Encode() const {
    //DCHECK_NE(database_id_, kInvalidId);
    //DCHECK_NE(object_store_id_, kInvalidId);
    //DCHECK_NE(index_id_, kInvalidId);
    return EncodeInternal(database_id_, object_store_id_, index_id_);
}

std::string KeyPrefix::EncodeInternal(int64_t database_id,
                                      int64_t object_store_id,
                                      int64_t index_id) {
    std::string database_id_string;
    std::string object_store_id_string;
    std::string index_id_string;

    EncodeIntSafely(database_id, kMaxDatabaseId, &database_id_string);
    EncodeIntSafely(object_store_id, kMaxObjectStoreId, &object_store_id_string);
    EncodeIntSafely(index_id, kMaxIndexId, &index_id_string);

    //DCHECK_LE(database_id_string.size(), kMaxDatabaseIdSizeBytes);
    //DCHECK_LE(object_store_id_string.size(), kMaxObjectStoreIdSizeBytes);
    //DCHECK_LE(index_id_string.size(), kMaxIndexIdSizeBytes);

    unsigned char first_byte =
            (database_id_string.size() - 1)
            << (kMaxObjectStoreIdSizeBits + kMaxIndexIdSizeBits) |
               (object_store_id_string.size() - 1) << kMaxIndexIdSizeBits |
               (index_id_string.size() - 1);
    static_assert(kMaxDatabaseIdSizeBits + kMaxObjectStoreIdSizeBits +
            kMaxIndexIdSizeBits ==
            sizeof(first_byte) * 8,
            "cannot encode ids");
    std::string ret;
    ret.reserve(kDefaultInlineBufferSize);
    ret.push_back(first_byte);
    ret.append(database_id_string);
    ret.append(object_store_id_string);
    ret.append(index_id_string);

    //DCHECK_LE(ret.size(), kDefaultInlineBufferSize);
    return ret;
}


