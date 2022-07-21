#include <IndexNamesKey.hpp>
#include <CodingUtils.hpp>
#include <KeyPrefix.hpp>



IndexNamesKey::IndexNamesKey() : object_store_id_(-1) {}

// TODO(jsbell): We never use this to look up index ids, because a mapping
// is kept at a higher level.
bool IndexNamesKey::Decode(StringPiece* slice, IndexNamesKey* result) {
    KeyPrefix prefix;
    if (!KeyPrefix::Decode(slice, &prefix))
        return false;
    assert(prefix.database_id_);
    assert(!prefix.object_store_id_);
    assert(!prefix.index_id_);
    uint8_t type_byte = 0;
    if (!DecodeByte(slice, &type_byte))
        return false;
    assert(type_byte == kIndexNamesKeyTypeByte);
    if (!DecodeVarInt(slice, &result->object_store_id_))
        return false;
    if (!DecodeStringWithLength(slice, &result->index_name_))
        return false;
    return true;
}

std::string IndexNamesKey::Encode(int64_t database_id,
                                  int64_t object_store_id,
                                  const std::u16string& index_name) {
    KeyPrefix prefix(database_id);
    std::string ret = prefix.Encode();
    ret.push_back(kIndexNamesKeyTypeByte);
    EncodeVarInt(object_store_id, &ret);
    EncodeStringWithLength(index_name, &ret);
    return ret;
}

int IndexNamesKey::Compare(const IndexNamesKey& other) {
    assert(object_store_id_ >= 0);
    if (int x = CompareInts(object_store_id_, other.object_store_id_))
        return x;
    return index_name_.compare(other.index_name_);
}


