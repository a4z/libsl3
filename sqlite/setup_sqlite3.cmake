include_guard(GLOBAL)

# http://www.sqlite.org/compile.html

# Options
set(SQLITE_THREADSAFE "1" CACHE STRING "SQLITE_THREADSAFE=<0|1|2>")
set(SQLITE_TEMP_STORE "1" CACHE STRING "SQLITE_TEMP_STORE=<0|1|2|3>")
option(SQLITE_OMIT_LOAD_EXTENSION "Define SQLITE_OMIT_LOAD_EXTENSION" ON)
option(SQLITE_ENABLE_FTS3 "Define SQLITE_ENABLE_FTS3" OFF)
option(SQLITE_ENABLE_FTS4 "Define SQLITE_ENABLE_FTS4" ON)
option(SQLITE_ENABLE_FTS5 "Define SQLITE_ENABLE_FTS5" OFF)
option(SQLITE_ENABLE_FTS3_PARENTHESIS "Define SQLITE_ENABLE_FTS3_PARENTHESIS" OFF)
option(SQLITE_SOUNDEX "Define SQLITE_SOUNDEX" OFF)
option(SQLITE_ENABLE_ICU "Define SQLITE_ENABLE_ICU" OFF)

# Sources
set(SQLITE3_FILES sqlite/sqlite3.h sqlite/sqlite3ext.h sqlite/sqlite3.c)

# Suppress warnings for third-party sqlite3.c
set_source_files_properties(sqlite/sqlite3.c PROPERTIES COMPILE_FLAGS -w)

# Build compile definitions
set(sqlite3_defines ${SL3_SQLITE_USERDEFINES} SQLITE_ENABLE_STAT4)

if(SQLITE_THREADSAFE GREATER_EQUAL 0 AND SQLITE_THREADSAFE LESS 3)
  list(APPEND sqlite3_defines SQLITE_THREADSAFE=${SQLITE_THREADSAFE})
else()
  message(WARNING "Invalid SQLITE_THREADSAFE=${SQLITE_THREADSAFE}, using default 1")
  list(APPEND sqlite3_defines SQLITE_THREADSAFE=1)
endif()

if(SQLITE_TEMP_STORE GREATER_EQUAL 0 AND SQLITE_TEMP_STORE LESS 4)
  list(APPEND sqlite3_defines SQLITE_TEMP_STORE=${SQLITE_TEMP_STORE})
else()
  message(WARNING "Invalid SQLITE_TEMP_STORE=${SQLITE_TEMP_STORE}, skipping")
endif()

if(SQLITE_OMIT_LOAD_EXTENSION)
  list(APPEND sqlite3_defines SQLITE_OMIT_LOAD_EXTENSION)
  list(REMOVE_ITEM SQLITE3_FILES sqlite/sqlite3ext.h)
endif()

if(SQLITE_ENABLE_FTS3)
  list(APPEND sqlite3_defines SQLITE_ENABLE_FTS3)
endif()
if(SQLITE_ENABLE_FTS4)
  list(APPEND sqlite3_defines SQLITE_ENABLE_FTS4)
endif()
if(SQLITE_ENABLE_FTS5)
  list(APPEND sqlite3_defines SQLITE_ENABLE_FTS5)
endif()
if(SQLITE_ENABLE_FTS3_PARENTHESIS)
  list(APPEND sqlite3_defines SQLITE_ENABLE_FTS3_PARENTHESIS)
endif()
if(SQLITE_SOUNDEX)
  list(APPEND sqlite3_defines SQLITE_SOUNDEX)
endif()
if(SQLITE_ENABLE_ICU)
  list(APPEND sqlite3_defines SQLITE_ENABLE_ICU)
endif()

# Apply to target
target_sources(sl3 PRIVATE ${SQLITE3_FILES})
target_include_directories(sl3 SYSTEM BEFORE PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sqlite>
)
target_compile_definitions(sl3 PRIVATE ${sqlite3_defines})

find_package(Threads REQUIRED)
target_link_libraries(sl3 PUBLIC Threads::Threads)
