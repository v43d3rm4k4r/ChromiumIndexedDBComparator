#include <ObjectStoreFreeListKey.hpp>
#include <CodingUtils.hpp>
#include <KeyPrefix.hpp>



ObjectStoreFreeListKey::ObjectStoreFreeListKey() : object_store_id_(-1) {}

bool ObjectStoreFreeListKey::Decode(StringPiece* slice,
                                    ObjectStoreFreeListKey* result) {
    KeyPrefix prefix;
    if (!KeyPrefix::Decode(slice, &prefix))
        return false;
    assert(prefix.database_id_);
    assert(!prefix.object_store_id_);
    assert(!prefix.index_id_);
    unsigned char type_byte = 0;
    if (!DecodeByte(slice, &type_byte))
        return false;
    assert(type_byte == kObjectStoreFreeListTypeByte);
    if (!DecodeVarInt(slice, &result->object_store_id_))
        return false;
    return true;
}

std::string ObjectStoreFreeListKey::Encode(int64_t database_id,
                                           int64_t object_store_id) {
    KeyPrefix prefix(database_id);
    std::string ret = prefix.Encode();
    ret.push_back(kObjectStoreFreeListTypeByte);
    EncodeVarInt(object_store_id, &ret);
    return ret;
}

std::string ObjectStoreFreeListKey::EncodeMaxKey(int64_t database_id) {
    return Encode(database_id, std::numeric_limits<int64_t>::max());
}

int64_t ObjectStoreFreeListKey::ObjectStoreId() const {
    assert(object_store_id_ >= 0);
    return object_store_id_;
}

int ObjectStoreFreeListKey::Compare(const ObjectStoreFreeListKey& other) {
    // TODO(jsbell): It may seem strange that we're not comparing database id's,
    // but that comparison will have been made earlier.
    // We should probably make this more clear, though...
    assert(object_store_id_ >= 0);
    return CompareInts(object_store_id_, other.object_store_id_);
}


