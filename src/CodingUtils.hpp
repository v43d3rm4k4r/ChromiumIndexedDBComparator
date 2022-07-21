#pragma once

#include <cstdint>
#include <string>

#include <StringPiece.hpp>


void EncodeBinary(const std::string& value, std::string* into);
bool DecodeBinary(StringPiece* slice, std::string* value);
void EncodeInt(int64_t value, std::string* into);
bool DecodeInt(StringPiece* slice, int64_t* value);
void EncodeByte( uint8_t value, std::string* into);
bool DecodeByte(StringPiece* slice, uint8_t* value);
bool DecodeDouble(StringPiece* slice, double* value);
void EncodeDouble(double value, std::string* into);
int  CompareSizes(int a, int b);

void EncodeVarInt(int64_t from, std::string* into);
bool DecodeVarInt(StringPiece* from, int64_t* into);
void EncodeIntSafely(int64_t value, int64_t max, std::string* into);
int  CompareInts(int64_t a, int64_t b);
uint16_t ByteSwap(uint16_t x); // Returns a value with all bytes in |x| swapped, i.e. reverses the endianness.
uint16_t NetToHost16(uint16_t x); // Converts the bytes in |x| from network to host order (endianness), and returns the result.
uint16_t HostToNet16(uint16_t x); // Converts the bytes in |x| from host to network order (endianness), and returns the result.
bool DecodeString(StringPiece* slice, std::u16string* value);
bool DecodeStringWithLength(StringPiece* slice, std::u16string* value);
void EncodeString(const std::u16string& value, std::string* into);
void EncodeStringWithLength(const std::u16string& value, std::string* into);
int CompareEncodedStringsWithLength(StringPiece* slice1, StringPiece* slice2, bool* ok);


using MachineWord = uintptr_t;
inline bool IsMachineWordAligned(const void* pointer);
template <typename Char>
bool DoIsStringASCII(const Char* characters, size_t length);
bool IsStringASCII(StringPiece str);
std::u16string ASCIIToUTF16(StringPiece ascii);
int CompareEncodedBinary(StringPiece* slice1, StringPiece* slice2, bool* ok);

enum IDBKeyType {
    Invalid,
    Array,
    Binary,
    String,
    Date,
    Number,
    None,
    Min,
};
IDBKeyType KeyTypeByteToKeyType(uint8_t type);
int CompareTypes(IDBKeyType a, IDBKeyType b);


// As most of the IndexedDBKeys and encoded values are short, we
// initialize some std::vectors with a default inline buffer size to reduce
// the memory re-allocations when the std::vectors are appended.
constexpr size_t  kDefaultInlineBufferSize = 32;

constexpr uint8_t kObjectStoreDataIndexId = 1;
constexpr uint8_t kExistsEntryIndexId     = 2;
constexpr uint8_t kBlobEntryIndexId       = 3;

constexpr uint8_t kIndexedDBKeyNullTypeByte   = 0;
constexpr uint8_t kIndexedDBKeyStringTypeByte = 1;
constexpr uint8_t kIndexedDBKeyDateTypeByte   = 2;
constexpr uint8_t kIndexedDBKeyNumberTypeByte = 3;
constexpr uint8_t kIndexedDBKeyArrayTypeByte  = 4;
constexpr uint8_t kIndexedDBKeyMinKeyTypeByte = 5;
constexpr uint8_t kIndexedDBKeyBinaryTypeByte = 6;

constexpr uint8_t kDatabaseFreeListTypeByte = 100;
constexpr uint8_t kDatabaseNameTypeByte = 201;
constexpr uint8_t kMaxSimpleGlobalMetaDataTypeByte = 7;
constexpr uint8_t kScopesPrefixByte = 50;
constexpr uint8_t kObjectStoreMetaDataTypeByte = 50;
constexpr uint8_t kObjectStoreFreeListTypeByte = 150;
constexpr uint8_t kObjectStoreNamesTypeByte = 200;
constexpr uint8_t kObjectMetaDataTypeMaximum = 255;
constexpr uint8_t kIndexMetaDataTypeByte = 100;
constexpr uint8_t kIndexFreeListTypeByte = 151;
constexpr uint8_t kIndexNamesKeyTypeByte = 201;
constexpr uint8_t kIndexMetaDataTypeMaximum = 255;

constexpr uint8_t kMinimumIndexId = 30;



