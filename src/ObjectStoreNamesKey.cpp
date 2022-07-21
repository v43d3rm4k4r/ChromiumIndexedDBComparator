#include <ObjectStoreNamesKey.hpp>
#include <KeyPrefix.hpp>
#include <CodingUtils.hpp>



bool ObjectStoreNamesKey::Decode(StringPiece* slice,
                                 ObjectStoreNamesKey* result) {
    KeyPrefix prefix;
    if (!KeyPrefix::Decode(slice, &prefix))
        return false;
    assert(prefix.database_id_);
    assert(!prefix.object_store_id_);
    assert(!prefix.index_id_);
    unsigned char type_byte = 0;
    if (!DecodeByte(slice, &type_byte))
        return false;
    assert(type_byte == kObjectStoreNamesTypeByte);
    if (!DecodeStringWithLength(slice, &result->object_store_name_))
        return false;
    return true;
}

std::string ObjectStoreNamesKey::Encode(int64_t database_id, const std::u16string& object_store_name) {
    KeyPrefix prefix(database_id);
    std::string ret = prefix.Encode();
    ret.push_back(kObjectStoreNamesTypeByte);
    EncodeStringWithLength(object_store_name, &ret);
    return ret;
}

int ObjectStoreNamesKey::Compare(const ObjectStoreNamesKey& other) {
    return object_store_name_.compare(other.object_store_name_);
}


