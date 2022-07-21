#include <ChromiumIndexedDBComparator.hpp>

//#include "indexed_db/indexed_db_leveldb_coding.h"
#include <StringPiece.hpp>
#include <KeyPrefix.hpp>

#include <ObjectStoreMetaDataKey.hpp>
#include <ObjectStoreFreeListKey.hpp>
#include <ObjectStoreNamesKey.hpp>
#include <ObjectStoreDataKey.hpp>
#include <IndexMetaDataKey.hpp>
#include <IndexFreeListKey.hpp>
#include <IndexNamesKey.hpp>
#include <DatabaseMetaDataKey.hpp>
#include <DatabaseFreeListKey.hpp>
#include <DatabaseNameKey.hpp>
#include <ExistsEntryKey.hpp>
#include <BlobEntryKey.hpp>
#include <IndexDataKey.hpp>

#include <CodingUtils.hpp>


static int Compare(const StringPiece& a, const StringPiece& b, bool only_compare_index_keys);
static int Compare(const StringPiece& a, const StringPiece& b, bool only_compare_index_keys, bool* ok);
template <typename KeyType>
int Compare(const StringPiece& a, const StringPiece& b, bool only_compare_index_keys, bool* ok); // TODO: remove only_compare_index_keys ?
template <typename KeyType>
int CompareSuffix(StringPiece* a, StringPiece* b, bool only_compare_index_keys,bool* ok); // Shouldn't be here
template <>
int CompareSuffix<ObjectStoreDataKey>(StringPiece* slice_a, StringPiece* slice_b, bool only_compare_index_keys, bool* ok);
template <>
int CompareSuffix<ExistsEntryKey>(StringPiece* slice_a, StringPiece* slice_b, bool only_compare_index_keys, bool* ok);
template <>
int CompareSuffix<BlobEntryKey>(StringPiece* slice_a, StringPiece* slice_b, bool only_compare_index_keys, bool* ok);
template <>
int CompareSuffix<IndexDataKey>(StringPiece* slice_a, StringPiece* slice_b, bool only_compare_index_keys, bool* ok);
int CompareEncodedIDBKeys(StringPiece* slice_a, StringPiece* slice_b, bool* ok);



int ChromiumIndexedDBComparator::Compare(const leveldb::Slice &a, const leveldb::Slice &b) const
{
    StringPiece sa(a.data(), a.size());
    StringPiece sb(b.data(), b.size());

    return ::Compare(sa, sb, false);
}

static int Compare(const StringPiece& a, const StringPiece& b, bool only_compare_index_keys) {
    bool ok;
    int result = Compare(a, b, only_compare_index_keys, &ok);
    // TODO(dmurph): Report this somehow. https://crbug.com/913121
    assert(ok);
    if (!ok)
        return 0;
    return result;
}

static int Compare(const StringPiece& a, const StringPiece& b, bool only_compare_index_keys, bool* ok) {
    StringPiece slice_a(a);
    StringPiece slice_b(b);
    KeyPrefix prefix_a;
    KeyPrefix prefix_b;
    bool ok_a = KeyPrefix::Decode(&slice_a, &prefix_a);
    bool ok_b = KeyPrefix::Decode(&slice_b, &prefix_b);
    if (!ok_a || !ok_b) {
        *ok = false;
        return 0;
    }

    *ok = true;
    if (int x = prefix_a.Compare(prefix_b))
        return x;

    switch (prefix_a.type()) {
    case KeyPrefix::GLOBAL_METADATA: {
        assert(!slice_a.empty());
        assert(!slice_b.empty());

        uint8_t type_byte_a;
        if (!DecodeByte(&slice_a, &type_byte_a)) {
            *ok = false;
            return 0;
        }

        uint8_t type_byte_b;
        if (!DecodeByte(&slice_b, &type_byte_b)) {
            *ok = false;
            return 0;
        }

        if (int x = type_byte_a - type_byte_b)
            return x;
        if (type_byte_a < kMaxSimpleGlobalMetaDataTypeByte)
            return 0;

        if (type_byte_a == kScopesPrefixByte)
            return slice_a.compare(slice_b);

        // Compare<> is used (which re-decodes the prefix) rather than an
        // specialized CompareSuffix<> because metadata is relatively uncommon
        // in the database.

        if (type_byte_a == kDatabaseFreeListTypeByte) {
            // TODO(jsbell): No need to pass only_compare_index_keys through here.
            return Compare<DatabaseFreeListKey>(a, b, only_compare_index_keys, ok);
        }
        if (type_byte_a == kDatabaseNameTypeByte) {
            return Compare<DatabaseNameKey>(a, b, false,
                                            ok);
        }
        break;
    }

    case KeyPrefix::DATABASE_METADATA: {
        assert(!slice_a.empty());
        assert(!slice_b.empty());

        uint8_t type_byte_a;
        if (!DecodeByte(&slice_a, &type_byte_a)) {
            *ok = false;
            return 0;
        }

        uint8_t type_byte_b;
        if (!DecodeByte(&slice_b, &type_byte_b)) {
            *ok = false;
            return 0;
        }

        if (int x = type_byte_a - type_byte_b)
            return x;
        if (type_byte_a < DatabaseMetaDataKey::MAX_SIMPLE_METADATA_TYPE)
            return 0;

        // Compare<> is used (which re-decodes the prefix) rather than an
        // specialized CompareSuffix<> because metadata is relatively uncommon
        // in the database.

        if (type_byte_a == kObjectStoreMetaDataTypeByte) {
            // TODO(jsbell): No need to pass only_compare_index_keys through here.
            return Compare<ObjectStoreMetaDataKey>(a, b, only_compare_index_keys, ok);
        }
        if (type_byte_a == kIndexMetaDataTypeByte) {
            return Compare<IndexMetaDataKey>(a, b, false, ok);
        }
        if (type_byte_a == kObjectStoreFreeListTypeByte) {
            return Compare<ObjectStoreFreeListKey>(a, b, only_compare_index_keys, ok);
        }
        if (type_byte_a == kIndexFreeListTypeByte) {
            return Compare<IndexFreeListKey>(a, b, false, ok);
        }
        if (type_byte_a == kObjectStoreNamesTypeByte) {
            // TODO(jsbell): No need to pass only_compare_index_keys through here.
            return Compare<ObjectStoreNamesKey>(a, b, only_compare_index_keys, ok);
        }
        if (type_byte_a == kIndexNamesKeyTypeByte) {
            return Compare<IndexNamesKey>(a, b, false, ok);
        }
        break;
    }

    case KeyPrefix::OBJECT_STORE_DATA: {
        // Provide a stable ordering for invalid data.
        if (slice_a.empty() || slice_b.empty())
            return CompareSizes(slice_a.size(), slice_b.size());

        return CompareSuffix<ObjectStoreDataKey>(&slice_a, &slice_b, false, ok);
    }

    case KeyPrefix::EXISTS_ENTRY: {
        // Provide a stable ordering for invalid data.
        if (slice_a.empty() || slice_b.empty())
            return CompareSizes(slice_a.size(), slice_b.size());

        return CompareSuffix<ExistsEntryKey>(&slice_a, &slice_b, false, ok);
    }

    case KeyPrefix::BLOB_ENTRY: {
        // Provide a stable ordering for invalid data.
        if (slice_a.empty() || slice_b.empty())
            return CompareSizes(slice_a.size(), slice_b.size());

        return CompareSuffix<BlobEntryKey>(&slice_a, &slice_b, false, ok);
    }

    case KeyPrefix::INDEX_DATA: {
        // Provide a stable ordering for invalid data.
        if (slice_a.empty() || slice_b.empty())
            return CompareSizes(slice_a.size(), slice_b.size());

        return CompareSuffix<IndexDataKey>(&slice_a, &slice_b, only_compare_index_keys, ok);
    }

    case KeyPrefix::INVALID_TYPE:
        break;
    }

    *ok = false;
    return 0;
}

template <typename KeyType>
int Compare(const StringPiece& a, const StringPiece& b, bool only_compare_index_keys, bool* ok) {
    (void)only_compare_index_keys;

    KeyType key_a;
    KeyType key_b;

    StringPiece slice_a(a);
    if (!KeyType::Decode(&slice_a, &key_a)) {
        *ok = false;
        return 0;
    }
    StringPiece slice_b(b);
    if (!KeyType::Decode(&slice_b, &key_b)) {
        *ok = false;
        return 0;
    }

    *ok = true;
    return key_a.Compare(key_b);
}

// Shouldn't be here
template <typename KeyType>
int CompareSuffix(StringPiece* a,
                  StringPiece* b,
                  bool only_compare_index_keys,
                  bool* ok) {
  return 0;
}

template <>
int CompareSuffix<ObjectStoreDataKey>(StringPiece* slice_a,
                                      StringPiece* slice_b,
                                      bool only_compare_index_keys,
                                      bool* ok) {
  return CompareEncodedIDBKeys(slice_a, slice_b, ok);
}

template <>
int CompareSuffix<ExistsEntryKey>(StringPiece* slice_a,
                                  StringPiece* slice_b,
                                  bool only_compare_index_keys,
                                  bool* ok) {
    assert(!slice_a->empty());
    assert(!slice_b->empty());
    return CompareEncodedIDBKeys(slice_a, slice_b, ok);
}

template <>
int CompareSuffix<BlobEntryKey>(StringPiece* slice_a,
                                StringPiece* slice_b,
                                bool only_compare_index_keys,
                                bool* ok) {
  return CompareEncodedIDBKeys(slice_a, slice_b, ok);
}

template <>
int CompareSuffix<IndexDataKey>(StringPiece* slice_a, StringPiece* slice_b, bool only_compare_index_keys, bool* ok) {
    // index key
    int result = CompareEncodedIDBKeys(slice_a, slice_b, ok);
    if (!*ok || result)
        return result;
    if (only_compare_index_keys)
        return 0;

    // sequence number [optional]
    int64_t sequence_number_a = -1;
    int64_t sequence_number_b = -1;
    if (!slice_a->empty() && !DecodeVarInt(slice_a, &sequence_number_a))
        return 0;
    if (!slice_b->empty() && !DecodeVarInt(slice_b, &sequence_number_b))
        return 0;

    if (slice_a->empty() || slice_b->empty())
        return CompareSizes(slice_a->size(), slice_b->size());

    // primary key [optional]
    result = CompareEncodedIDBKeys(slice_a, slice_b, ok);
    if (!*ok || result)
        return result;

    return CompareInts(sequence_number_a, sequence_number_b);
}

int CompareEncodedIDBKeys(StringPiece* slice_a, StringPiece* slice_b, bool* ok) {
    assert(!slice_a->empty());
    assert(!slice_b->empty());
    *ok = true;
    uint8_t type_a = (*slice_a)[0];
    uint8_t type_b = (*slice_b)[0];
    slice_a->remove_prefix(1);
    slice_b->remove_prefix(1);

    if (int x = CompareTypes(KeyTypeByteToKeyType(type_a), KeyTypeByteToKeyType(type_b)))
        return x;

    switch (type_a) {
    case kIndexedDBKeyNullTypeByte:
    case kIndexedDBKeyMinKeyTypeByte:
        // Null type or max type; no payload to compare.
        return 0;
    case kIndexedDBKeyArrayTypeByte: {
        int64_t length_a, length_b;
        if (!DecodeVarInt(slice_a, &length_a) ||
                !DecodeVarInt(slice_b, &length_b)) {
            *ok = false;
            return 0;
        }
        for (int64_t i = 0; i < length_a && i < length_b; ++i) {
            int result = CompareEncodedIDBKeys(slice_a, slice_b, ok);
            if (!*ok || result)
                return result;
        }
        return length_a - length_b;
    }
    case kIndexedDBKeyBinaryTypeByte:
        return CompareEncodedBinary(slice_a, slice_b, ok);
    case kIndexedDBKeyStringTypeByte:
        return CompareEncodedStringsWithLength(slice_a, slice_b, ok);
    case kIndexedDBKeyDateTypeByte:
    case kIndexedDBKeyNumberTypeByte: {
        double d, e;
        if (!DecodeDouble(slice_a, &d) || !DecodeDouble(slice_b, &e)) {
            *ok = false;
            return 0;
        }
        if (d < e)
            return -1;
        if (d > e)
            return 1;
        return 0;
    }
    }//switch

    //NOTREACHED();
    return 0;
}
