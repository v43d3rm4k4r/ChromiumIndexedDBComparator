# ChromiumIndexedDBComparator
A library that allows you to open IndexedDB databases of applications based on the Chromium engine via the levelDB C++ API.

## Using
```cpp
#include <leveldb/db.h>

leveldb::DB* db = nullptr;
leveldb::Options options;

options.create_if_missing = false;
options.paranoid_checks   = true;

ChromiumIndexedDBComparator comparator;
options.comparator        = &comparator;

leveldb::Status status = leveldb::DB::Open(options, "/tmp/testdb", &db);

// ...

```
