#include "../testing.hpp"
#include <sl3/config.hpp>

#include <string>

SCENARIO("check sqlite versions for library and app")
{
  // this can be used as an example to add into your project
  // assure the libsl3 was compiled using the same header as you app

  GIVEN ("cmake genertated config")
  {
    WHEN ("comparing compile/runtime verion")
    {
      THEN ("these version are the same")
      {
        std::string compiledVersion = sl3::sqliteCompiledVersion() ;
        CHECK(compiledVersion == sl3::sqliteRuntimeVersion());
      }
    }

    WHEN ("comparing the compile/runtime version numbers")
    {
      THEN ("they are the same")
      {
        CHECK_EQ ( sl3::sqliteCompiledVersionNumber(),
                    sl3::sqliteRuntimeVersionNumber());
      }
    }

    WHEN ("doing something just for coverage")
    {
      THEN ("it should always pass")
      { // if this is ever important, just use as needed
        auto tsco = sl3::sqliteThreadSafeCompileOption () ;
        auto ts = tsco == 1 || tsco == 2 || tsco == 3;
        CHECK (ts) ;
      }
    }
  }
}
