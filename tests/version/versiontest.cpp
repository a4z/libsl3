#include "../testing.hpp"
#include <sl3/config.hpp>

#include <string>

// believe it or not, this is actually usefull,
// there are often many different version of sqlite on a system
// and which one cmake picks up depends ... so you want to know about that

/*

this happened on Mac CI,

/Users/runner/work/libsl3/libsl3/tests/version/versiontest.cpp:6:
  Scenario: check sqlite versions for library and app
     Given: cmake genertated config
      When: comparing the compile/runtime version numbers
      Then: they are the same

/Users/runner/work/libsl3/libsl3/tests/version/versiontest.cpp:27: ERROR: CHECK_EQ( sl3::sqliteCompiledVersionNumber (), sl3::sqliteRuntimeVersionNumber () ) is NOT correct!
  values: CHECK_EQ( 3026000, 3043002 )

*/



SCENARIO ("check sqlite versions for library and app")
{
  // this can be used as an example to add into your project
  // assure the libsl3 was compiled using the same header as you app

  GIVEN ("cmake genertated config")
  {
    WHEN ("comparing compile/runtime verion")
    {
      THEN ("these version are the same")
      {
        std::string compiledVersion = sl3::sqliteCompiledVersion ();
        CHECK (compiledVersion == sl3::sqliteRuntimeVersion ());
      }
    }

    WHEN ("comparing the compile/runtime version numbers")
    {
      THEN ("they are the same")
      {
        CHECK_EQ (sl3::sqliteCompiledVersionNumber (),
                  sl3::sqliteRuntimeVersionNumber ());
      }
    }

    WHEN ("doing something just for coverage")
    {
      THEN ("it should always pass")
      { // if this is ever important, just use as needed
        auto tsco = sl3::sqliteThreadSafeCompileOption ();
        auto ts   = tsco == 1 || tsco == 2 || tsco == 3;
        CHECK (ts);
      }
    }
  }
}
