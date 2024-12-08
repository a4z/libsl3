include_guard(GLOBAL)

#http://www.sqlite.org/compile.html

#   SQLITE_DEFAULT_FILE_FORMAT=<1 or 4>
#   SQLITE_DEFAULT_FOREIGN_KEYS=<0 or 1>
#   SQLITE_THREADSAFE=<0 or 1 or 2>
#   SQLITE_TEMP_STORE=<0 through 3> 0 always files / 1 files - allow PRAGMA temp_store, 2 memory allow PRAGMA temp_store , 3 always memory
#   SQLITE_ENABLE_FTS3
#   SQLITE_ENABLE_FTS3_PARENTHESIS
#   SQLITE_ENABLE_ICU  for future...
#   SQLITE_SOUNDEX
#   SQLITE_ENABLE_RTREE
#   SQLITE_OMIT_LOAD_EXTENSION
#   SQLITE_ENABLE_STAT4   will be always set
#   SQLITE_ENABLE_JSON1   default on, but might change


# TODO covert the legacy BOOL to ON/OFF options


include_directories( BEFORE SYSTEM sqlite)
set( SQLITE3_FILES sqlite/sqlite3.h sqlite/sqlite3ext.h sqlite/sqlite3.c )

# No warnings for sqlite3.c
if(CMAKE_LANG_COMPILER_ID STREQUAL "MSVC")
  set(OPTION_PREFIX "/")
else()
  set(OPTION_PREFIX "-")
endif()
set_source_files_properties(sqlite/sqlite3.c  PROPERTIES COMPILE_FLAGS "${OPTION_PREFIX}w")

set( SQLITE_THREADSAFE "1" CACHE STRING "defines SQLITE_THREADSAFE=<0 or 1 or 2>")
set(SQLITE_OMIT_LOAD_EXTENSION ON CACHE BOOL "defines SQLITE_OMIT_LOAD_EXTENSION if ON")

# TODO add FTS 4 and 5
set( SQLITE_ENABLE_FTS3 ${SQLITE_ENABLE_FTS3} CACHE BOOL "defines SQLITE_ENABLE_FTS3 if on")
set( SQLITE_ENABLE_FTS3 ${SQLITE_ENABLE_FTS4} CACHE BOOL "defines SQLITE_ENABLE_FTS3 if on")
set( SQLITE_ENABLE_FTS3 ${SQLITE_ENABLE_FTS5} CACHE BOOL "defines SQLITE_ENABLE_FTS3 if on")

set( SQLITE_ENABLE_FTS3_PARENTHESIS OFF CACHE BOOL
    "defines SQLITE_ENABLE_FTS3_PARENTHESIS if on")

set( SQLITE_SOUNDEX ${SQLITE_SOUNDEX} CACHE BOOL "defines SQLITE_SOUNDEX if on")
set( SQLITE_ENABLE_ICU OFF CACHE BOOL "defines SQLITE_ENABLE_ICU if on")
#   <0 through 3> 0 always files / 1 files - allow PRAGMA temp_store, 2 memory allow PRAGMA temp_store , 3 always memory
set(SQLITE_TEMP_STORE 1 CACHE STRING "defines SQLITE_TEMP_STORE=<0 or 1 or 2>")

# TODO , other as required....
set( sl3_sqlite3LIBS  pthread dl  ) # what on windows ???
# add defaults first

set(mysqlt3_DEFINES ${SL3_SQLITE_USERDEFINES})

# add this always to be sure that it is enabled
# stat2 is no op and stat3 can be overwritten by stat 4
# use stat4 as default
list( APPEND mysqlt3_DEFINES  SQLITE_ENABLE_STAT4 )


if (SQLITE_THREADSAFE EQUAL 0)
    list( APPEND mysqlt3_DEFINES SQLITE_THREADSAFE=0 )
    list( REMOVE_ITEM sl3_sqlite3LIBS pthread )
elseif( SQLITE_THREADSAFE GREATER 0 AND SQLITE_THREADSAFE LESS 3 )
    list( APPEND mysqlt3_DEFINES SQLITE_THREADSAFE=${SQLITE_THREADSAFE} )
else( SQLITE_THREADSAFE EQUAL 0 )
    #MESSAGE( FATAL_ERROR "invalid value for "${SQLITE_THREADSAFE} )
    MESSAGE( WARNING "invalid value ${SQLITE_THREADSAFE} for SQLITE_THREADSAFE, use default 1" )
    list( APPEND mysqlt3_DEFINES SQLITE_THREADSAFE=${SQLITE_THREADSAFE} )
endif( SQLITE_THREADSAFE EQUAL 0 )

if( SQLITE_TEMP_STORE GREATER -1 AND SQLITE_THREADSAFE LESS 3 )
    list( APPEND mysqlt3_DEFINES SQLITE_TEMP_STORE=${SQLITE_TEMP_STORE} )
else( SQLITE_TEMP_STORE GREATER -1 AND SQLITE_THREADSAFE LESS 3 )
    MESSAGE( WARING "invalid value ${SQLITE_TEMP_STORE} for SQLITE_TEMP_STORE, define will not be defined" )
endif( SQLITE_TEMP_STORE GREATER -1 AND SQLITE_THREADSAFE LESS 3 )

if ( SQLITE_OMIT_LOAD_EXTENSION )

    list( REMOVE_ITEM SQLITE3_FILES sqlt3/sqlite3ext.h )
    list( APPEND mysqlt3_DEFINES  SQLITE_OMIT_LOAD_EXTENSION )
    list( REMOVE_ITEM sl3_sqlite3LIBS dl )

endif ( SQLITE_OMIT_LOAD_EXTENSION )

# make stat4 default
set (SQLITE_ENABLE_FTS4 true)

if ( SQLITE_ENABLE_FTS3 )
    list( APPEND mysqlt3_DEFINES  SQLITE_ENABLE_FTS3 )
endif ( SQLITE_ENABLE_FTS3 )

if ( SQLITE_ENABLE_FTS4 )
    list( APPEND mysqlt3_DEFINES  SQLITE_ENABLE_FTS4 )
endif ( SQLITE_ENABLE_FTS4 )

if ( SQLITE_ENABLE_FTS5 )
    list( APPEND mysqlt3_DEFINES  SQLITE_ENABLE_FTS5 )
endif ( SQLITE_ENABLE_FTS5 )



if ( SQLITE_ENABLE_FTS3_PARENTHESIS )
    list( APPEND mysqlt3_DEFINES  SQLITE_ENABLE_FTS3_PARENTHESIS )
endif ( SQLITE_ENABLE_FTS3_PARENTHESIS )

if ( SQLITE_SOUNDEX )
    list( APPEND mysqlt3_DEFINES  SQLITE_SOUNDEX )
endif ( SQLITE_SOUNDEX )

if ( SQLITE_ENABLE_ICU )
    list( APPEND mysqlt3_DEFINES  SQLITE_ENABLE_ICU )
endif ( SQLITE_ENABLE_ICU )

if(WIN32)
        # TODO, since WSL, MSYS and Cygwin might not be that relevant anymore
        if(NOT MSYS) #and not cygwin ?
                set(sl3_sqlite3LIBS "")
        endif(NOT MSYS)
endif(WIN32)

target_sources(sl3 PRIVATE ${SQLITE3_FILES})
target_compile_definitions( sl3 PRIVATE ${mysqlt3_DEFINES} )
find_package( Threads )
target_link_libraries( sl3 PUBLIC ${CMAKE_THREAD_LIBS_INIT} )

# TODO, make it the same as find_package(SQLite3 REQUIRED) does

#message(STATUS "**DEFINES ARE: "${mysqlt3_DEFINES} )
#message(STATUS "**LIBRARIES ARE: "${sl3_sqlite3LIBS} )

# else (sl3_USE_INTERNAL_SQLITE3)
#         INCLUDE(FindPkgConfig)

#         # TODO define a minimum version 3.3.8

#     # TODO what if I am on windows ...
#         # SQLITE3_INCLUDE_DIRS
#         # SQLITE3_LIBRARIES
#         # SQLITE3_LIBRARY_DIRS
#         pkg_check_modules(SQLITE3 REQUIRED sqlite3)
#         set(sl3_sqlite3LIBS ${SQLITE3_LIBRARIES} CACHE STRING "sqlite3 libraries")




