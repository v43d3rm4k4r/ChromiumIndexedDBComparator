#include <IndexDataKey.hpp>
#include <KeyPrefix.hpp>


IndexDataKey::IndexDataKey()
    : database_id_(-1),
      object_store_id_(-1),
      index_id_(-1),
      sequence_number_(-1) {}

IndexDataKey::IndexDataKey(IndexDataKey&& other) = default;

IndexDataKey::~IndexDataKey() {}

bool IndexDataKey::Decode(StringPiece* slice, IndexDataKey* result) {
    KeyPrefix prefix;
    if (!KeyPrefix::Decode(slice, &prefix))
        return false;
    if (prefix.database_id_ <= 0)
        return false;
    if (prefix.object_store_id_ <= 0)
        return false;
    if (prefix.index_id_ < kMinimumIndexId)
        return false;
    result->database_id_ = prefix.database_id_;
    result->object_store_id_ = prefix.object_store_id_;
    result->index_id_ = prefix.index_id_;
    result->sequence_number_ = -1;
    result->encoded_primary_key_ = MinIDBKey();

    if (!ExtractEncodedIDBKey(slice, &result->encoded_user_key_))
        return false;

    // [optional] sequence number
    if (slice->empty())
        return true;
    if (!DecodeVarInt(slice, &result->sequence_number_))
        return false;

    // [optional] primary key
    if (slice->empty())
        return true;
    if (!ExtractEncodedIDBKey(slice, &result->encoded_primary_key_))
        return false;
    return true;
}

std::string IndexDataKey::Encode(int64_t database_id,
                                 int64_t object_store_id,
                                 int64_t index_id,
                                 const std::string& encoded_user_key,
                                 const std::string& encoded_primary_key,
                                 int64_t sequence_number) {
    KeyPrefix prefix(database_id, object_store_id, index_id);
    std::string ret = prefix.Encode();
    ret.append(encoded_user_key);
    EncodeVarInt(sequence_number, &ret);
    ret.append(encoded_primary_key);
    return ret;
}

std::string IndexDataKey::Encode(int64_t database_id,
                                 int64_t object_store_id,
                                 int64_t index_id,
                                 const IndexedDBKey& user_key) {
    std::string encoded_key;
    EncodeIDBKey(user_key, &encoded_key);
    return Encode(database_id, object_store_id, index_id, encoded_key,
                  MinIDBKey(), 0);
}

std::string IndexDataKey::Encode(int64_t database_id,
                                 int64_t object_store_id,
                                 int64_t index_id,
                                 const IndexedDBKey& user_key,
                                 const IndexedDBKey& user_primary_key) {
    std::string encoded_key;
    EncodeIDBKey(user_key, &encoded_key);
    std::string encoded_primary_key;
    EncodeIDBKey(user_primary_key, &encoded_primary_key);
    return Encode(database_id, object_store_id, index_id, encoded_key,
                  encoded_primary_key, 0);
}

std::string IndexDataKey::EncodeMinKey(int64_t database_id,
                                       int64_t object_store_id,
                                       int64_t index_id) {
    return Encode(database_id, object_store_id, index_id, MinIDBKey(),
                  MinIDBKey(), 0);
}

std::string IndexDataKey::EncodeMaxKey(int64_t database_id,
                                       int64_t object_store_id,
                                       int64_t index_id) {
    return Encode(database_id, object_store_id, index_id, MaxIDBKey(),
                  MaxIDBKey(), std::numeric_limits<int64_t>::max());
}

std::string IndexDataKey::Encode() const {
    return Encode(database_id_, object_store_id_, index_id_, encoded_user_key_,
                  encoded_primary_key_, sequence_number_);
}

/*
std::string IndexDataKey::DebugString() const {
    std::unique_ptr<IndexedDBKey> user = user_key();
    std::unique_ptr<IndexedDBKey> primary = primary_key();
    std::stringstream result;
    result << "IndexDataKey{db: " << database_id_ << ", os: " << object_store_id_
           << ", idx: " << index_id_ << ", sequence_number: " << sequence_number_
           << ", user_key: " << (user ? user->DebugString() : "Invalid")
           << ", primary_key: " << (primary ? primary->DebugString() : "Invalid")
           << "}";
    return result.str();
}*/

int64_t IndexDataKey::DatabaseId() const {
    //DCHECK_GE(database_id_, 0);
    return database_id_;
}

int64_t IndexDataKey::ObjectStoreId() const {
    //DCHECK_GE(object_store_id_, 0);
    return object_store_id_;
}

int64_t IndexDataKey::IndexId() const {
    //DCHECK_GE(index_id_, 0);
    return index_id_;
}

std::unique_ptr<IndexedDBKey> IndexDataKey::user_key() const {
    std::unique_ptr<IndexedDBKey> key;
    StringPiece slice(encoded_user_key_);
    if (!DecodeIDBKey(&slice, &key)) {
        // TODO(jsbell): Return error.
    }
    return key;
}

std::unique_ptr<IndexedDBKey> IndexDataKey::primary_key() const {
    std::unique_ptr<IndexedDBKey> key;
    StringPiece slice(encoded_primary_key_);
    if (!DecodeIDBKey(&slice, &key)) {
        // TODO(jsbell): Return error.
    }
    return key;
}
