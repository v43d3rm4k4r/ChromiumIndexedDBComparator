#include <IndexedDBKey.hpp>
#include <StringPiece.hpp>


static size_t CalculateArraySize(const IndexedDBKey::KeyArray& keys) {
    size_t size(0);
    for (const auto& key : keys)
        size += key.size_estimate();
    return size;
}


IndexedDBKey::IndexedDBKey()
    : type_(IDBKeyType::None), size_estimate_(kOverheadSize) {}

IndexedDBKey::IndexedDBKey(IDBKeyType type)
    : type_(type), size_estimate_(kOverheadSize) {
    assert(type == IDBKeyType::None ||
           type == IDBKeyType::Invalid || type == IDBKeyType::Min);
}

IndexedDBKey::IndexedDBKey(double number, IDBKeyType type)
    : type_(type),
      number_(number),
      size_estimate_(kOverheadSize + sizeof(number)) {
    assert(type == IDBKeyType::Number ||
           type == IDBKeyType::Date);
}

IndexedDBKey::IndexedDBKey(KeyArray array)
    : type_(IDBKeyType::Array),
      array_(std::move(array)),
      size_estimate_(kOverheadSize + CalculateArraySize(array_)) {}

IndexedDBKey::IndexedDBKey(std::string binary)
    : type_(IDBKeyType::Binary),
      binary_(std::move(binary)),
      size_estimate_(kOverheadSize +
                     (binary_.length() * sizeof(std::string::value_type))) {}

IndexedDBKey::IndexedDBKey(std::u16string string)
    : type_(IDBKeyType::String),
      string_(std::move(string)),
      size_estimate_(kOverheadSize +
                     (string_.length() * sizeof(std::u16string::value_type))) {}

IndexedDBKey::IndexedDBKey(const IndexedDBKey& other) = default;
IndexedDBKey::~IndexedDBKey() = default;
IndexedDBKey& IndexedDBKey::operator=(const IndexedDBKey& other) = default;

bool IndexedDBKey::IsValid() const {
    if (type_ == IDBKeyType::Invalid || type_ == IDBKeyType::None)
        return false;

    if (type_ == IDBKeyType::Array) {
        for (size_t i = 0; i < array_.size(); i++) {
            if (!array_[i].IsValid())
                return false;
        }
    }

    return true;
}

bool IndexedDBKey::IsLessThan(const IndexedDBKey& other) const {
    return CompareTo(other) < 0;
}

bool IndexedDBKey::Equals(const IndexedDBKey& other) const {
    return !CompareTo(other);
}

bool IndexedDBKey::HasHoles() const {
    if (type_ != IDBKeyType::Array)
        return false;

    for (const auto& subkey : array_) {
        if (subkey.type() == IDBKeyType::None)
            return true;
    }
    return false;
}

IndexedDBKey IndexedDBKey::FillHoles(const IndexedDBKey& primary_key) const {
    if (type_ != IDBKeyType::Array)
        return IndexedDBKey(*this);

    std::vector<IndexedDBKey> subkeys;
    subkeys.reserve(array_.size());
    for (const auto& subkey : array_) {
        if (subkey.type() == IDBKeyType::None) {
            subkeys.push_back(primary_key);
        } else {
            // "Holes" can only exist at the top level of an array key, as (1) they
            // are produced by an index's array keypath when a member matches the
            // store's keypath, and (2) array keypaths are flat (no
            // arrays-of-arrays).
            assert(!subkey.HasHoles());
            subkeys.push_back(subkey);
        }
    }
    return IndexedDBKey(subkeys);
}

void EncodeIDBKey(const IndexedDBKey& value, std::string* into) {
    size_t previous_size = into->size();
    assert(value.IsValid());
    switch (value.type()) {
    case IDBKeyType::Array: {
        EncodeByte(kIndexedDBKeyArrayTypeByte, into);
        size_t length = value.array().size();
        EncodeVarInt(length, into);
        for (size_t i = 0; i < length; ++i)
            EncodeIDBKey(value.array()[i], into);
        assert(into->size() > previous_size);
        return;
    }
    case IDBKeyType::Binary:
        EncodeByte(kIndexedDBKeyBinaryTypeByte, into);
        EncodeBinary(value.binary(), into);
        assert(into->size() > previous_size);
        return;
    case IDBKeyType::String:
        EncodeByte(kIndexedDBKeyStringTypeByte, into);
        EncodeStringWithLength(value.string(), into);
        assert(into->size() > previous_size);
        return;
    case IDBKeyType::Date:
        EncodeByte(kIndexedDBKeyDateTypeByte, into);
        EncodeDouble(value.date(), into);
        assert(9u == (static_cast<size_t>(into->size() - previous_size)));
        return;
    case IDBKeyType::Number:
        EncodeByte(kIndexedDBKeyNumberTypeByte, into);
        EncodeDouble(value.number(), into);
        assert(9u == (static_cast<size_t>(into->size() - previous_size)));
        return;
    case IDBKeyType::None:
    case IDBKeyType::Invalid:
    case IDBKeyType::Min:
    default:
        //NOTREACHED();
        EncodeByte(kIndexedDBKeyNullTypeByte, into);
        return;
    }
}

bool DecodeIDBKey(StringPiece* slice, std::unique_ptr<IndexedDBKey>* value) {
    return DecodeIDBKeyRecursive(slice, value, 0);
}

bool DecodeIDBKeyRecursive(StringPiece* slice,
                           std::unique_ptr<IndexedDBKey>* value,
                           size_t recursion) {
    if (slice->empty())
        return false;

    if (recursion > IndexedDBKey::kMaximumDepth)
        return false;

    unsigned char type = (*slice)[0];
    slice->remove_prefix(1);

    switch (type) {
    case kIndexedDBKeyNullTypeByte:
        *value = std::make_unique<IndexedDBKey>();
        return true;

    case kIndexedDBKeyArrayTypeByte: {
        int64_t length = 0;
        if (!DecodeVarInt(slice, &length) || length < 0)
            return false;
        IndexedDBKey::KeyArray array;
        while (length--) {
            std::unique_ptr<IndexedDBKey> key;
            if (!DecodeIDBKeyRecursive(slice, &key, recursion + 1))
                return false;
            array.push_back(*key);
        }
        *value = std::make_unique<IndexedDBKey>(std::move(array));
        return true;
    }
    case kIndexedDBKeyBinaryTypeByte: {
        std::string binary;
        if (!DecodeBinary(slice, &binary))
            return false;
        *value = std::make_unique<IndexedDBKey>(std::move(binary));
        return true;
    }
    case kIndexedDBKeyStringTypeByte: {
        std::u16string s;
        if (!DecodeStringWithLength(slice, &s))
            return false;
        *value = std::make_unique<IndexedDBKey>(std::move(s));
        return true;
    }
    case kIndexedDBKeyDateTypeByte: {
        double d;
        if (!DecodeDouble(slice, &d))
            return false;
        *value =
                std::make_unique<IndexedDBKey>(d, IDBKeyType::Date);
        return true;
    }
    case kIndexedDBKeyNumberTypeByte: {
        double d;
        if (!DecodeDouble(slice, &d))
            return false;
        *value =
                std::make_unique<IndexedDBKey>(d, IDBKeyType::Number);
        return true;
    }
    case kIndexedDBKeyMinKeyTypeByte: {
        *value = std::make_unique<IndexedDBKey>(IDBKeyType::Min);
        return true;
    }
    }//switch

    //NOTREACHED();
    return false;
}

bool ExtractEncodedIDBKey(StringPiece* slice, std::string* result) {
    const char* start = slice->data(); // slice->begin();
    if (!ConsumeEncodedIDBKey(slice))
        return false;

    if (result)
        result->assign(start, slice->data()); // slice->begin()
    return true;
}

bool ConsumeEncodedIDBKey(StringPiece* slice) {
    unsigned char type = (*slice)[0];
    slice->remove_prefix(1);

    switch (type) {
    case kIndexedDBKeyNullTypeByte:
    case kIndexedDBKeyMinKeyTypeByte:
        return true;
    case kIndexedDBKeyArrayTypeByte: {
        int64_t length;
        if (!DecodeVarInt(slice, &length) || length < 0)
            return false;
        while (length--) {
            if (!ConsumeEncodedIDBKey(slice))
                return false;
        }
        return true;
    }
    case kIndexedDBKeyBinaryTypeByte: {
        int64_t length = 0;
        if (!DecodeVarInt(slice, &length) || length < 0)
            return false;
        if (slice->size() < static_cast<size_t>(length))
            return false;
        slice->remove_prefix(length);
        return true;
    }
    case kIndexedDBKeyStringTypeByte: {
        int64_t length = 0;
        if (!DecodeVarInt(slice, &length) || length < 0)
            return false;
        if (slice->size() < static_cast<size_t>(length) * sizeof(char16_t))
            return false;
        slice->remove_prefix(length * sizeof(char16_t));
        return true;
    }
    case kIndexedDBKeyDateTypeByte:
    case kIndexedDBKeyNumberTypeByte:
        if (slice->size() < sizeof(double))
            return false;
        slice->remove_prefix(sizeof(double));
        return true;
    }
    //NOTREACHED();
    return false;
}

std::string MinIDBKey() {
  std::string ret;
  EncodeByte(kIndexedDBKeyMinKeyTypeByte, &ret);
  return ret;
}

std::string MaxIDBKey() {
  std::string ret;
  EncodeByte(kIndexedDBKeyNullTypeByte, &ret);
  return ret;
}
