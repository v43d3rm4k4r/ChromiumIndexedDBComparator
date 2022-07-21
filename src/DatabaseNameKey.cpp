#include <DatabaseNameKey.hpp>
#include <CodingUtils.hpp>
#include <KeyPrefix.hpp>


bool DatabaseNameKey::Decode(StringPiece* slice, DatabaseNameKey* result) {
    KeyPrefix prefix;
    if (!KeyPrefix::Decode(slice, &prefix))
        return false;
    assert(!prefix.database_id_);
    assert(!prefix.object_store_id_);
    assert(!prefix.index_id_);
    unsigned char type_byte = 0;
    if (!DecodeByte(slice, &type_byte))
        return false;
    assert(type_byte == kDatabaseNameTypeByte);
    if (!DecodeStringWithLength(slice, &result->origin_))
        return false;
    if (!DecodeStringWithLength(slice, &result->database_name_))
        return false;
    return true;
}

std::string DatabaseNameKey::Encode(const std::string& origin_identifier,
                                    const std::u16string& database_name) {
    std::string ret = KeyPrefix::EncodeEmpty();
    ret.push_back(kDatabaseNameTypeByte);
    EncodeStringWithLength(ASCIIToUTF16(origin_identifier), &ret);
    EncodeStringWithLength(database_name, &ret);
    return ret;
}

int DatabaseNameKey::Compare(const DatabaseNameKey& other) {
    if (int x = origin_.compare(other.origin_))
        return x;
    return database_name_.compare(other.database_name_);
}

std::string DatabaseNameKey::EncodeMinKeyForOrigin(
        const std::string& origin_identifier) {
    return Encode(origin_identifier, std::u16string());
}

std::string DatabaseNameKey::EncodeStopKeyForOrigin(
        const std::string& origin_identifier) {
    // just after origin in collation order
    return EncodeMinKeyForOrigin(origin_identifier + '\x01');
}

