#include <ExistsEntryKey.hpp>
#include <KeyPrefix.hpp>


const int64_t ExistsEntryKey::kSpecialIndexNumber = kExistsEntryIndexId;

bool ExistsEntryKey::Decode(StringPiece* slice, ExistsEntryKey* result) {
    KeyPrefix prefix;
    if (!KeyPrefix::Decode(slice, &prefix))
        return false;
    assert(prefix.database_id_);
    assert(prefix.object_store_id_);
    assert(prefix.index_id_ == kSpecialIndexNumber);
    if (!ExtractEncodedIDBKey(slice, &result->encoded_user_key_))
        return false;
    return true;
}

std::string ExistsEntryKey::Encode(int64_t database_id,
                                   int64_t object_store_id,
                                   const std::string& encoded_key) {
    KeyPrefix prefix(KeyPrefix::CreateWithSpecialIndex(
                         database_id, object_store_id, kSpecialIndexNumber));
    std::string ret = prefix.Encode();
    ret.append(encoded_key);
    return ret;
}

std::string ExistsEntryKey::Encode(int64_t database_id,
                                   int64_t object_store_id,
                                   const IndexedDBKey& user_key) {
    std::string encoded_key;
    EncodeIDBKey(user_key, &encoded_key);
    return Encode(database_id, object_store_id, encoded_key);
}
/*
std::string ExistsEntryKey::DebugString() const {
    std::unique_ptr<IndexedDBKey> key = user_key();
    std::stringstream result;
    result << "ExistsEntryKey{user_key: "
           << (key ? key->DebugString() : "Invalid") << "}";
    return result.str();
}*/

std::unique_ptr<IndexedDBKey> ExistsEntryKey::user_key() const {
    std::unique_ptr<IndexedDBKey> key;
    StringPiece slice(encoded_user_key_);
    if (!DecodeIDBKey(&slice, &key)) {
        // TODO(jsbell): Return error.
    }
    return key;
}
