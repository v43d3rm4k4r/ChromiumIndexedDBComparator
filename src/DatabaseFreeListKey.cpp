#include <DatabaseFreeListKey.hpp>
#include <KeyPrefix.hpp>
#include <CodingUtils.hpp>


DatabaseFreeListKey::DatabaseFreeListKey() : database_id_(-1) {}

bool DatabaseFreeListKey::Decode(StringPiece* slice,
                                 DatabaseFreeListKey* result) {
    KeyPrefix prefix;
    if (!KeyPrefix::Decode(slice, &prefix))
        return false;
    assert(!prefix.database_id_);
    assert(!prefix.object_store_id_);
    assert(!prefix.index_id_);
    unsigned char type_byte = 0;
    if (!DecodeByte(slice, &type_byte))
        return false;
    assert(type_byte == kDatabaseFreeListTypeByte);
    if (!DecodeVarInt(slice, &result->database_id_))
        return false;
    return true;
}

std::string DatabaseFreeListKey::Encode(int64_t database_id) {
    std::string ret = KeyPrefix::EncodeEmpty();
    ret.push_back(kDatabaseFreeListTypeByte);
    EncodeVarInt(database_id, &ret);
    return ret;
}

std::string DatabaseFreeListKey::EncodeMaxKey() {
    return Encode(std::numeric_limits<int64_t>::max());
}

int64_t DatabaseFreeListKey::DatabaseId() const {
    assert(database_id_ >= 0);
    return database_id_;
}

int DatabaseFreeListKey::Compare(const DatabaseFreeListKey& other) const {
    assert(database_id_ >= 0);
    return CompareInts(database_id_, other.database_id_);
}
