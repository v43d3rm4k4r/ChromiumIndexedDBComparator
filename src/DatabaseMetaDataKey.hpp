#pragma once

#include <cstdint>
#include <string>

class DatabaseMetaDataKey {
 public:
  enum MetaDataType {
    ORIGIN_NAME = 0,
    DATABASE_NAME = 1,
    USER_STRING_VERSION = 2,  // Obsolete
    MAX_OBJECT_STORE_ID = 3,
    USER_VERSION = 4,
    BLOB_KEY_GENERATOR_CURRENT_NUMBER = 5,
    MAX_SIMPLE_METADATA_TYPE = 6
  };

//  /*CONTENT_EXPORT*/ static const int64_t kAllBlobsNumber;
//  static const int64_t kBlobNumberGeneratorInitialNumber;
//  // All keys <= 0 are invalid.  This one's just a convenient example.
//  static const int64_t kInvalidBlobNumber;

//  /*CONTENT_EXPORT*/ static bool IsValidBlobNumber(int64_t blob_number);
//  /*CONTENT_EXPORT*/ static std::string Encode(int64_t database_id, MetaDataType type);
};



