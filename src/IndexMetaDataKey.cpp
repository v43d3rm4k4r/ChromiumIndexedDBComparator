#include <IndexMetaDataKey.hpp>
#include <CodingUtils.hpp>
#include <KeyPrefix.hpp>



IndexMetaDataKey::IndexMetaDataKey()
    : object_store_id_(-1), index_id_(-1), meta_data_type_(0) {}

bool IndexMetaDataKey::Decode(StringPiece* slice, IndexMetaDataKey* result) {
    KeyPrefix prefix;
    if (!KeyPrefix::Decode(slice, &prefix))
        return false;
    assert(prefix.database_id_);
    assert(!prefix.object_store_id_);
    assert(!prefix.index_id_);
    uint8_t type_byte = 0;
    if (!DecodeByte(slice, &type_byte))
        return false;
    assert(type_byte == kIndexMetaDataTypeByte);
    if (!DecodeVarInt(slice, &result->object_store_id_))
        return false;
    if (!DecodeVarInt(slice, &result->index_id_))
        return false;
    if (!DecodeByte(slice, &result->meta_data_type_))
        return false;
    return true;
}

std::string IndexMetaDataKey::Encode(int64_t database_id,
                                     int64_t object_store_id,
                                     int64_t index_id,
                                     unsigned char meta_data_type) {
    KeyPrefix prefix(database_id);
    std::string ret = prefix.Encode();
    ret.push_back(kIndexMetaDataTypeByte);
    EncodeVarInt(object_store_id, &ret);
    EncodeVarInt(index_id, &ret);
    EncodeByte(meta_data_type, &ret);
    return ret;
}

std::string IndexMetaDataKey::EncodeMaxKey(int64_t database_id,
                                           int64_t object_store_id) {
    return Encode(database_id, object_store_id,
                  std::numeric_limits<int64_t>::max(), kIndexMetaDataTypeMaximum);
}

std::string IndexMetaDataKey::EncodeMaxKey(int64_t database_id,
                                           int64_t object_store_id,
                                           int64_t index_id) {
    return Encode(database_id, object_store_id, index_id,
                  kIndexMetaDataTypeMaximum);
}

int IndexMetaDataKey::Compare(const IndexMetaDataKey& other) {
    assert(object_store_id_ >= 0);
    assert(index_id_ >= 0);

    if (int x = CompareInts(object_store_id_, other.object_store_id_))
        return x;
    if (int x = CompareInts(index_id_, other.index_id_))
        return x;
    return meta_data_type_ - other.meta_data_type_;
}


