/// Comporator from Chromium code base (https://source.chromium.org/chromium/chromium/src/+/main:content/browser/indexed_db/indexed_db_leveldb_operations.cc?q=%22idb_cmp%22&ss=chromium%2Fchromium%2Fsrc)
/// for IndexedDB opening
#pragma once

#include <leveldb/comparator.h>

class ChromiumIndexedDBComparator : public leveldb::Comparator {
public:
    using leveldb::Comparator::Comparator;
    ~ChromiumIndexedDBComparator() override = default;

    int Compare(const ::leveldb::Slice& a, const ::leveldb::Slice& b) const override;

    const char* Name() const override { return "idb_cmp1"; };

    void FindShortestSeparator(std::string* start, const ::leveldb::Slice& limit) const override {};
    void FindShortSuccessor(std::string* key) const override {};
};




