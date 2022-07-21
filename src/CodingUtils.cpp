#include <CodingUtils.hpp>
#include <StringPiece.hpp>

bool DecodeBinary(StringPiece* slice, std::string* value) {
    if (slice->empty())
        return false;

    int64_t length = 0;
    if (!DecodeVarInt(slice, &length) || length < 0)
        return false;
    size_t size = length;
    if (slice->size() < size)
        return false;

    value->assign(slice->data(), size); // slice->begin()
    slice->remove_prefix(size);
    return true;
}

void EncodeBinary(const std::string& value, std::string* into) {
    EncodeVarInt(value.length(), into);
    into->append(value.begin(), value.end());
    assert(into->size() >= value.size());
}

void EncodeInt(int64_t value, std::string* into) {
    // Exercised by unit tests in debug only.
    //DCHECK_GE(value, 0);

    uint64_t n = static_cast<uint64_t>(value);

    do {
        uint8_t c = n;
        into->push_back(c);
        n >>= 8;
    } while (n);
}

bool DecodeInt(StringPiece* slice, int64_t* value) {
    if (slice->empty())
        return false;

    StringPiece::const_iterator it = slice->begin();
    int shift = 0;
    int64_t ret = 0;
    while (it != slice->end()) {
        unsigned char c = *it++;
        ret |= static_cast<int64_t>(c) << shift;
        shift += 8;
    }
    *value = ret;
    slice->remove_prefix(it - slice->begin());
    return true;
}

bool DecodeByte(StringPiece* slice, unsigned char* value) {
    if (slice->empty())
        return false;

    *value = (*slice)[0];
    slice->remove_prefix(1);
    return true;
}

void EncodeByte(unsigned char value, std::string* into) {
    into->push_back(value);
}

bool DecodeDouble(StringPiece* slice, double* value) {
    if (slice->size() < sizeof(*value))
        return false;

    memcpy(value, slice->data(), sizeof(*value)); // slice->begin()
    slice->remove_prefix(sizeof(*value));
    return true;
}

void EncodeDouble(double value, std::string* into) {
    // This always has host endianness.
    const char* p = reinterpret_cast<char*>(&value);
    into->insert(into->end(), p, p + sizeof(value));
}

int CompareSizes(int a, int b) {
    if (a > b)
        return 1;
    if (b > a)
        return -1;
    return 0;
}

void EncodeVarInt(int64_t from, std::string* into) {
    assert(from >= 0);
    // A temporary array is used to amortize the costs of the string modification.
    static constexpr size_t kMaxBytesForUInt64VarInt = 10;
    char temp[kMaxBytesForUInt64VarInt];
    uint64_t n = static_cast<uint64_t>(from);
    size_t temp_index = 0;
    do {
        unsigned char c = n & 0x7f;
        n >>= 7;
        if (n)
            c |= 0x80;
        assert(temp_index < kMaxBytesForUInt64VarInt);
        temp[temp_index] = c;
        ++temp_index;
    } while (n);
    into->append(temp, temp_index);
}

bool DecodeVarInt(StringPiece* from, int64_t* into) {
    StringPiece::const_iterator it = from->begin();
    int shift = 0;
    uint64_t ret = 0;
    do {
        // Shifting 64 or more bits is undefined behavior.
        if (it == from->end() || shift >= 64)
            return false;

        unsigned char c = *it;
        ret |= static_cast<uint64_t>(c & 0x7f) << shift;
        shift += 7;
    } while (*it++ & 0x80);
    *into = static_cast<int64_t>(ret);
    from->remove_prefix(it - from->begin());
    return true;
}

void EncodeIntSafely(int64_t value, int64_t max, std::string* into) {
    //DCHECK_LE(value, max);
    return EncodeInt(value, into);
}

int CompareInts(int64_t a, int64_t b) {
    int64_t diff = a - b;
    if (diff < 0)
        return -1;
    if (diff > 0)
        return 1;
    return 0;
}

uint16_t ByteSwap(uint16_t x) {
#ifdef _WINDOWS //#if defined(COMPILER_MSVC) && !defined(__clang__)
    return _byteswap_ushort(x);
#else
    return __builtin_bswap16(x);
#endif
}

inline uint16_t NetToHost16(uint16_t x) {
#if defined(ARCH_CPU_LITTLE_ENDIAN) /// TODO: mb need to remove
    return ByteSwap(x);
#else
    return x;
#endif
}

inline uint16_t HostToNet16(uint16_t x) { // diff ??
#if defined(ARCH_CPU_LITTLE_ENDIAN)
    return ByteSwap(x);
#else
    return x;
#endif
}

bool DecodeString(StringPiece* slice, std::u16string* value) {
    if (slice->empty()) {
        value->clear();
        return true;
    }

    // Backing store is UTF-16BE, convert to host endianness.
    assert(!(slice->size() % sizeof(char16_t)));
    size_t length = slice->size() / sizeof(char16_t);
    std::u16string decoded;
    decoded.reserve(length);
    const char16_t* encoded = reinterpret_cast<const char16_t*>(slice->data()); // slice->begin()
    for (unsigned i = 0; i < length; ++i)
        decoded.push_back(NetToHost16(*encoded++));

    *value = decoded;
    slice->remove_prefix(length * sizeof(char16_t));
    return true;
}

bool DecodeStringWithLength(StringPiece* slice, std::u16string* value) {
    if (slice->empty())
        return false;

    int64_t length = 0;
    if (!DecodeVarInt(slice, &length) || length < 0)
        return false;
    size_t bytes = length * sizeof(char16_t);
    if (slice->size() < bytes)
        return false;

    StringPiece subpiece(slice->data(), bytes); // slice->begin()
    slice->remove_prefix(bytes);
    if (!DecodeString(&subpiece, value))
        return false;

    return true;
}

void EncodeString(const std::u16string& value, std::string* into) {
    if (value.empty())
        return;
    // Backing store is UTF-16BE, convert from host endianness.
    size_t length = value.length();
    size_t current = into->size();
    into->resize(into->size() + length * sizeof(char16_t));

    const char16_t* src = value.c_str();
    char16_t* dst = reinterpret_cast<char16_t*>(&*into->begin() + current);
    for (unsigned i = 0; i < length; ++i)
        *dst++ = HostToNet16(*src++);
}

void EncodeStringWithLength(const std::u16string& value, std::string* into) {
    EncodeVarInt(value.length(), into);
    EncodeString(value, into);
}

int CompareEncodedStringsWithLength(StringPiece* slice1,
                                    StringPiece* slice2,
                                    bool* ok) {
    int64_t len1, len2;
    if (!DecodeVarInt(slice1, &len1) || !DecodeVarInt(slice2, &len2)) {
        *ok = false;
        return 0;
    }
    if (len1 < 0 || len2 < 0) {
        *ok = false;
        return 0;
    }
    if (slice1->size() < len1 * sizeof(char16_t) ||
            slice2->size() < len2 * sizeof(char16_t)) {
        *ok = false;
        return 0;
    }

    // Extract the string data, and advance the passed slices.
    StringPiece string1(slice1->data(), len1 * sizeof(char16_t)); // slice1->begin()
    StringPiece string2(slice2->data(), len2 * sizeof(char16_t)); // slice2->begin()
    slice1->remove_prefix(len1 * sizeof(char16_t));
    slice2->remove_prefix(len2 * sizeof(char16_t));

    *ok = true;
    // Strings are UTF-16BE encoded, so a simple memcmp is sufficient.
    return string1.compare(string2);
}

inline bool IsMachineWordAligned(const void* pointer) {
    return !(reinterpret_cast<MachineWord>(pointer) & (sizeof(MachineWord) - 1));
}

template <typename Char>
bool DoIsStringASCII(const Char* characters, size_t length) {
    // Bitmasks to detect non ASCII characters for character sizes of 8, 16 and 32
    // bits.
    constexpr MachineWord NonASCIIMasks[] = {
        0, MachineWord(0x8080808080808080ULL), MachineWord(0xFF80FF80FF80FF80ULL),
        0, MachineWord(0xFFFFFF80FFFFFF80ULL),
    };

    if (!length)
        return true;
    constexpr MachineWord non_ascii_bit_mask = NonASCIIMasks[sizeof(Char)];
    static_assert(non_ascii_bit_mask, "Error: Invalid Mask");
    MachineWord all_char_bits = 0;
    const Char* end = characters + length;

    // Prologue: align the input.
    while (!IsMachineWordAligned(characters) && characters < end)
        all_char_bits |= *characters++;
    if (all_char_bits & non_ascii_bit_mask)
        return false;

    // Compare the values of CPU word size.
    constexpr size_t chars_per_word = sizeof(MachineWord) / sizeof(Char);
    constexpr int batch_count = 16;
    while (characters <= end - batch_count * chars_per_word) {
        all_char_bits = 0;
        for (int i = 0; i < batch_count; ++i) {
            all_char_bits |= *(reinterpret_cast<const MachineWord*>(characters));
            characters += chars_per_word;
        }
        if (all_char_bits & non_ascii_bit_mask)
            return false;
    }

    // Process the remaining words.
    all_char_bits = 0;
    while (characters <= end - chars_per_word) {
        all_char_bits |= *(reinterpret_cast<const MachineWord*>(characters));
        characters += chars_per_word;
    }

    // Process the remaining bytes.
    while (characters < end)
        all_char_bits |= *characters++;

    return !(all_char_bits & non_ascii_bit_mask);
}

bool IsStringASCII(StringPiece str) {
    return DoIsStringASCII(str.data(), str.length());
}

std::u16string ASCIIToUTF16(StringPiece ascii) {
    //DCHECK(IsStringASCII(ascii)) << ascii; /// ???
    assert(IsStringASCII(ascii));

    return std::u16string(ascii.begin(), ascii.end());
}

int CompareEncodedBinary(StringPiece* slice1, StringPiece* slice2, bool* ok) {
    int64_t len1, len2;
    if (!DecodeVarInt(slice1, &len1) || !DecodeVarInt(slice2, &len2)) {
        *ok = false;
        return 0;
    }
    if (len1 < 0 || len2 < 0) {
        *ok = false;
        return 0;
    }
    size_t size1 = len1;
    size_t size2 = len2;

    if (slice1->size() < size1 || slice2->size() < size2) {
        *ok = false;
        return 0;
    }

    // Extract the binary data, and advance the passed slices.
    StringPiece binary1(slice1->data(), size1); // slice1->begin()
    StringPiece binary2(slice2->data(), size2); // slice2->begin()
    slice1->remove_prefix(size1);
    slice2->remove_prefix(size2);

    *ok = true;
    // This is the same as a memcmp()
    return binary1.compare(binary2);
}

IDBKeyType KeyTypeByteToKeyType(uint8_t type) {
    switch (type) {
    case kIndexedDBKeyNullTypeByte:
        return IDBKeyType::Invalid;
    case kIndexedDBKeyArrayTypeByte:
        return IDBKeyType::Array;
    case kIndexedDBKeyBinaryTypeByte:
        return IDBKeyType::Binary;
    case kIndexedDBKeyStringTypeByte:
        return IDBKeyType::String;
    case kIndexedDBKeyDateTypeByte:
        return IDBKeyType::Date;
    case kIndexedDBKeyNumberTypeByte:
        return IDBKeyType::Number;
    case kIndexedDBKeyMinKeyTypeByte:
        return IDBKeyType::Min;
    }

    //NOTREACHED() << "Got invalid type " << type;
    return  IDBKeyType::Invalid;
}

int CompareTypes(IDBKeyType a, IDBKeyType b) {
    return static_cast<int32_t>(b) - static_cast<int32_t>(a);
}
