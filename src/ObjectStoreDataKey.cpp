#include <ObjectStoreDataKey.hpp>
#include <CodingUtils.hpp>
#include <KeyPrefix.hpp>



const int64_t ObjectStoreDataKey::kSpecialIndexNumber = kObjectStoreDataIndexId;

bool ObjectStoreDataKey::Decode(StringPiece* slice,
                                ObjectStoreDataKey* result) {
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

std::string ObjectStoreDataKey::Encode(int64_t database_id,
                                       int64_t object_store_id,
                                       const std::string encoded_user_key) {
  KeyPrefix prefix(KeyPrefix::CreateWithSpecialIndex(
      database_id, object_store_id, kSpecialIndexNumber));
  std::string ret = prefix.Encode();
  ret.append(encoded_user_key);

  return ret;
}

std::string ObjectStoreDataKey::Encode(int64_t database_id,
                                       int64_t object_store_id,
                                       const IndexedDBKey& user_key) {
  std::string encoded_key;
  EncodeIDBKey(user_key, &encoded_key);
  return Encode(database_id, object_store_id, encoded_key);
}

/*
std::string ObjectStoreDataKey::DebugString() const {
  std::unique_ptr<blink::IndexedDBKey> key = user_key();
  std::stringstream result;
  result << "ObjectStoreDataKey{user_key: "
         << (key ? key->DebugString() : "Invalid") << "}";
  return result.str();
}*/

std::unique_ptr<IndexedDBKey> ObjectStoreDataKey::user_key() const {
  std::unique_ptr<IndexedDBKey> key;
  StringPiece slice(encoded_user_key_);
  if (!DecodeIDBKey(&slice, &key)) {
    // TODO(jsbell): Return error.
  }
  return key;
}




