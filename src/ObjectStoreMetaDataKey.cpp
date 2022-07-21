#include <ObjectStoreMetaDataKey.hpp>
#include <CodingUtils.hpp>
#include <KeyPrefix.hpp>



const int64_t ObjectStoreMetaDataKey::kKeyGeneratorInitialNumber = 1;

ObjectStoreMetaDataKey::ObjectStoreMetaDataKey()
    : object_store_id_(-1), meta_data_type_(0xFF) {}

bool ObjectStoreMetaDataKey::Decode(StringPiece* slice,
                                    ObjectStoreMetaDataKey* result) {
    KeyPrefix prefix;
    if (!KeyPrefix::Decode(slice, &prefix))
        return false;
    assert(prefix.database_id_);
    assert(!prefix.object_store_id_);
    assert(!prefix.index_id_);
    unsigned char type_byte = 0;
    if (!DecodeByte(slice, &type_byte))
        return false;
    assert(type_byte == kObjectStoreMetaDataTypeByte);
    if (!DecodeVarInt(slice, &result->object_store_id_))
        return false;
    assert(result->object_store_id_);
    if (!DecodeByte(slice, &result->meta_data_type_))
        return false;
    return true;
}

std::string ObjectStoreMetaDataKey::Encode(int64_t database_id,
                                           int64_t object_store_id,
                                           unsigned char meta_data_type) {
    KeyPrefix prefix(database_id);
    std::string ret = prefix.Encode();
    ret.push_back(kObjectStoreMetaDataTypeByte);
    EncodeVarInt(object_store_id, &ret);
    ret.push_back(meta_data_type);
    return ret;
}

std::string ObjectStoreMetaDataKey::EncodeMaxKey(int64_t database_id) {
    return Encode(database_id, std::numeric_limits<int64_t>::max(),
                  kObjectMetaDataTypeMaximum);
}

std::string ObjectStoreMetaDataKey::EncodeMaxKey(int64_t database_id,
                                                 int64_t object_store_id) {
    return Encode(database_id, object_store_id, kObjectMetaDataTypeMaximum);
}

int64_t ObjectStoreMetaDataKey::ObjectStoreId() const {
    assert(object_store_id_ >= 0);
    return object_store_id_;
}
unsigned char ObjectStoreMetaDataKey::MetaDataType() const {
    return meta_data_type_;
}

int ObjectStoreMetaDataKey::Compare(const ObjectStoreMetaDataKey& other) {
    assert(object_store_id_ >= 0);
    if (int x = CompareInts(object_store_id_, other.object_store_id_))
        return x;
    return meta_data_type_ - other.meta_data_type_;
}


