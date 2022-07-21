#pragma once

#include <CodingUtils.hpp>

#include <vector>
#include <cassert>
#include <memory>


constexpr size_t kOverheadSize = 16;

class IndexedDBKey {
public:
    using KeyArray = std::vector<IndexedDBKey>;

    // Non-standard limits, selected to avoid breaking real-world use of the API
    // while also preventing buggy (or malicious) code from causing crashes.
    static constexpr size_t kMaximumDepth = 2000;
    static constexpr size_t kMaximumArraySize = 1000000;

    IndexedDBKey();  // Defaults to IDBKeyType::Invalid.
    explicit IndexedDBKey(IDBKeyType);  // must be Null or Invalid
    explicit IndexedDBKey(KeyArray array);
    explicit IndexedDBKey(std::string binary);
    explicit IndexedDBKey(std::u16string string);
    IndexedDBKey(double number,
                 IDBKeyType type);  // must be date or number
    IndexedDBKey(const IndexedDBKey& other);
    ~IndexedDBKey();
    IndexedDBKey& operator=(const IndexedDBKey& other);

    bool IsValid() const;

    bool IsLessThan(const IndexedDBKey& other) const;
    bool Equals(const IndexedDBKey& other) const;

    IDBKeyType type() const { return type_; }
    const std::vector<IndexedDBKey>& array() const {
        assert(type_ == IDBKeyType::Array);
        return array_;
    }
    const std::string& binary() const {
        assert(type_ == IDBKeyType::Binary);
        return binary_;
    }
    const std::u16string& string() const {
        assert(type_ == IDBKeyType::String);
        return string_;
    }
    double date() const {
        assert(type_ == IDBKeyType::Date);
        return number_;
    }
    double number() const {
        assert(type_ == IDBKeyType::Number);
        return number_;
    }

    size_t size_estimate() const { return size_estimate_; }

    // Tests if this array-type key has "holes". Used in cases where a compound
    // key references an auto-generated primary key.
    bool HasHoles() const;

    // Returns a copy of this array-type key, but with "holes" replaced by the
    // given primary key. Used in cases where a compound key references an
    // auto-generated primary key.
    [[nodiscard]] IndexedDBKey FillHoles(const IndexedDBKey&) const;

    //std::string DebugString() const;

private:
    int CompareTo(const IndexedDBKey& other) const;

    IDBKeyType type_;
    std::vector<IndexedDBKey> array_;
    std::string binary_;
    std::u16string string_;
    double number_ = 0;

    size_t size_estimate_;

};

void EncodeIDBKey(const IndexedDBKey& value, std::string* into);
bool DecodeIDBKey(StringPiece* slice, std::unique_ptr<IndexedDBKey>* value);
bool DecodeIDBKeyRecursive(StringPiece* slice, std::unique_ptr<IndexedDBKey>* value, size_t recursion);
bool ExtractEncodedIDBKey(StringPiece* slice, std::string* result);
bool ConsumeEncodedIDBKey(StringPiece* slice);
std::string MinIDBKey();
std::string MaxIDBKey();
