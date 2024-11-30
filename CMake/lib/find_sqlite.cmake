
#[[

in an optimal world, this would just work:

  find_package(SQLite3 REQUIRED)
  target_link_libraries(sl3 PUBLIC ${SQLite3_LIBRARIES})
  target_include_directories(sl3 SYSTEM BEFORE PRIVATE ${SQLite3_INCLUDE_DIRS})

but it does not

this happened on Mac CI,

/Users/runner/work/libsl3/libsl3/tests/version/versiontest.cpp:6:
  Scenario: check sqlite versions for library and app
     Given: cmake genertated config
      When: comparing the compile/runtime version numbers
      Then: they are the same

/Users/runner/work/libsl3/libsl3/tests/version/versiontest.cpp:27: ERROR: CHECK_EQ( sl3::sqliteCompiledVersionNumber (), sl3::sqliteRuntimeVersionNumber () ) is NOT correct!
  values: CHECK_EQ( 3026000, 3043002 )

~~~~~
~~~~~

package-manager could be helpful, but are sometimes not

vcpkg, for example, does the following:
------
sqlite3 provides pkgconfig bindings.
sqlite3 provides CMake targets:

    find_package(unofficial-sqlite3 CONFIG REQUIRED)
    target_link_libraries(main PRIVATE unofficial::sqlite3::sqlite3)
------

How to deliver a library that should work / be able to recompile in multiple contexts

What if conan is used, instead of vcpkg
Or someone whats to rely whats on the system (root fs on x compile for yocto)

# ]]


include(lib/debug)
print_all_variables()

if (USE_INTERNAL_SQLITE3)
    include( sqlite/setup_sqlite3.cmake )
else(USE_INTERNAL_SQLITE3)

  if (_VCPKG_INSTALLED_DIR)
    find_package(unofficial-sqlite3 CONFIG REQUIRED)
    set(SQLITE_LINK_NAME unofficial::sqlite3::sqlite3)
  else()
    find_package(SQLite3 CONFIG REQUIRED)
    set(SQLITE_LINK_NAME SQLite::SQLite3)
  endif()


endif(USE_INTERNAL_SQLITE3)


