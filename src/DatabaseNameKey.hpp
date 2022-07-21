#pragma once

#include <string>
#include <StringPiece.hpp>


class DatabaseNameKey {
public:
    static bool Decode(StringPiece* slice, DatabaseNameKey* result);
    /*CONTENT_EXPORT*/ static std::string Encode(const std::string& origin_identifier,
                                             const std::u16string& database_name);
    static std::string EncodeMinKeyForOrigin(
            const std::string& origin_identifier);
    static std::string EncodeStopKeyForOrigin(
            const std::string& origin_identifier);
    std::u16string origin() const { return origin_; }
    std::u16string database_name() const { return database_name_; }
    int Compare(const DatabaseNameKey& other);
    //std::string DebugString() const;

private:
    std::u16string origin_;  // TODO(jsbell): Store encoded strings, or just pointers.
    std::u16string database_name_;
};
