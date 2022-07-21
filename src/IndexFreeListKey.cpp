#include <IndexFreeListKey.hpp>
#include <CodingUtils.hpp>
#include <KeyPrefix.hpp>



IndexFreeListKey::IndexFreeListKey() : object_store_id_(-1), index_id_(-1) {}

bool IndexFreeListKey::Decode(StringPiece* slice, IndexFreeListKey* result) {
    KeyPrefix prefix;
    if (!KeyPrefix::Decode(slice, &prefix))
        return false;
    assert(prefix.database_id_);
    assert(!prefix.object_store_id_);
    assert(!prefix.index_id_);
    unsigned char type_byte = 0;
    if (!DecodeByte(slice, &type_byte))
        return false;
    assert(type_byte == kIndexFreeListTypeByte);
    if (!DecodeVarInt(slice, &result->object_store_id_))
        return false;
    if (!DecodeVarInt(slice, &result->index_id_))
        return false;
    return true;
}

std::string IndexFreeListKey::Encode(int64_t database_id,
                                     int64_t object_store_id,
                                     int64_t index_id) {
    KeyPrefix prefix(database_id);
    std::string ret = prefix.Encode();
    ret.push_back(kIndexFreeListTypeByte);
    EncodeVarInt(object_store_id, &ret);
    EncodeVarInt(index_id, &ret);
    return ret;
}

std::string IndexFreeListKey::EncodeMaxKey(int64_t database_id,
                                           int64_t object_store_id) {
    return Encode(database_id, object_store_id,
                  std::numeric_limits<int64_t>::max());
}

int IndexFreeListKey::Compare(const IndexFreeListKey& other) {
    assert(object_store_id_ >= 0);
    assert(index_id_ >= 0);
    if (int x = CompareInts(object_store_id_, other.object_store_id_))
        return x;
    return CompareInts(index_id_, other.index_id_);
}

/*std::string IndexFreeListKey::DebugString() const {
    std::stringstream result;
    result << "IndexFreeListKey{os: " << object_store_id_
           << ", idx: " << index_id_ << "}";
    return result.str();
}*/

int64_t IndexFreeListKey::ObjectStoreId() const {
    assert(object_store_id_ >= 0);
    return object_store_id_;
}

int64_t IndexFreeListKey::IndexId() const {
  assert(index_id_ >= 0);
  return index_id_;
}


