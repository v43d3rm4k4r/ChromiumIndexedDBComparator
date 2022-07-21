cmake_minimum_required(VERSION 3.10)

project(ChromiumIndexedDBComparator LANGUAGES CXX VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(leveldb CONFIG REQUIRED)
find_package(Crc32c CONFIG REQUIRED)
find_package(Snappy CONFIG REQUIRED)
find_package(Threads REQUIRED)

set(SOURCES
    ChromiumIndexedDBComparator.cpp
    StringPiece.cpp
    KeyPrefix.cpp
    CodingUtils.cpp
    DatabaseMetaDataKey.cpp
    DatabaseFreeListKey.cpp
    DatabaseNameKey.cpp
    ObjectStoreMetaDataKey.cpp
    ObjectStoreFreeListKey.cpp
    ObjectStoreNamesKey.cpp
    ObjectStoreDataKey.cpp
    IndexMetaDataKey.cpp
    IndexFreeListKey.cpp
    IndexNamesKey.cpp
    ExistsEntryKey.cpp
    IndexedDBKey.cpp
    BlobEntryKey.cpp
    IndexDataKey.cpp
    )

add_library(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC
        ${PROJECT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} leveldb::leveldb)

if (APPLE)
    target_compile_options(${PROJECT_NAME} PUBLIC "-Wno-error=#warnings" "-Wno-error=unused-result" "-Wno-error=deprecated-declarations")
endif()

if(WIN32)
    target_link_libraries(${PROJECT_NAME} ntdll Version)
endif()
