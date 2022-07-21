#include <BlobEntryKey.hpp>
#include <KeyPrefix.hpp>
#include <CodingUtils.hpp>
#include <ObjectStoreDataKey.hpp>


constexpr int64_t BlobEntryKey::kSpecialIndexNumber = kBlobEntryIndexId;

bool BlobEntryKey::Decode(StringPiece* slice, BlobEntryKey* result) {
    KeyPrefix prefix;
    if (!KeyPrefix::Decode(slice, &prefix))
        return false;
    assert(prefix.database_id_);
    assert(prefix.object_store_id_);
    assert(prefix.index_id_ == kSpecialIndexNumber);

    if (!ExtractEncodedIDBKey(slice, &result->encoded_user_key_))
        return false;
    result->database_id_ = prefix.database_id_;
    result->object_store_id_ = prefix.object_store_id_;

    return true;
}

bool BlobEntryKey::FromObjectStoreDataKey(StringPiece* slice,
                                          BlobEntryKey* result) {
    KeyPrefix prefix;
    if (!KeyPrefix::Decode(slice, &prefix))
        return false;
    assert(prefix.database_id_);
    assert(prefix.object_store_id_);
    assert(prefix.index_id_ == ObjectStoreDataKey::kSpecialIndexNumber);

    if (!ExtractEncodedIDBKey(slice, &result->encoded_user_key_))
        return false;
    result->database_id_ = prefix.database_id_;
    result->object_store_id_ = prefix.object_store_id_;
    return true;
}

std::string BlobEntryKey::ReencodeToObjectStoreDataKey(StringPiece* slice) {
    // TODO(ericu): We could be more efficient here, since the suffix is the same.
    BlobEntryKey key;
    if (!Decode(slice, &key))
        return std::string();

    return ObjectStoreDataKey::Encode(key.database_id_, key.object_store_id_,
                                      key.encoded_user_key_);
}

std::string BlobEntryKey::EncodeMinKeyForObjectStore(int64_t database_id,
                                                     int64_t object_store_id) {
    // Our implied encoded_user_key_ here is empty, the lowest possible key.
    return Encode(database_id, object_store_id, std::string());
}

std::string BlobEntryKey::EncodeStopKeyForObjectStore(int64_t database_id,
                                                      int64_t object_store_id) {
    assert(KeyPrefix::ValidIds(database_id, object_store_id));
    KeyPrefix prefix(KeyPrefix::CreateWithSpecialIndex(
                         database_id, object_store_id, kSpecialIndexNumber + 1));
    return prefix.Encode();
}

std::string BlobEntryKey::Encode() const {
    assert(!encoded_user_key_.empty());
    return Encode(database_id_, object_store_id_, encoded_user_key_);
}

std::string BlobEntryKey::Encode(int64_t database_id,
                                 int64_t object_store_id,
                                 const IndexedDBKey& user_key) {
    std::string encoded_key;
    EncodeIDBKey(user_key, &encoded_key);
    return Encode(database_id, object_store_id, encoded_key);
}

std::string BlobEntryKey::Encode(int64_t database_id,
                                 int64_t object_store_id,
                                 const std::string& encoded_user_key) {
    assert(KeyPrefix::ValidIds(database_id, object_store_id));
    KeyPrefix prefix(KeyPrefix::CreateWithSpecialIndex(
                         database_id, object_store_id, kSpecialIndexNumber));
    return prefix.Encode() + encoded_user_key;
}

