# ChromiumIndexedDBComparator
A library that allows you to open IndexedDB databases of applications based on the Chromium engine via the levelDB C++ API. This can be useful in exploring many applications data such as WhatsApp Desktop, Google Chrome and so on.

## Usage
```cpp
#include <leveldb/db.h>
#include <ChromiumIndexedDBComparator.hpp>

leveldb::DB* db = nullptr;
leveldb::Options options;

options.create_if_missing = false;
options.paranoid_checks   = true;

ChromiumIndexedDBComparator comparator;
options.comparator        = &comparator;

leveldb::Status status = leveldb::DB::Open(options, "/tmp/testdb", &db);

// ...

```
