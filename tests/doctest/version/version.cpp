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

    WHEN ("calculating the expected version")
    {
      THEN ("it is the compile version number")
      {
        auto expectedVersion =  3*1000000 + sl3::PATCH_VERSION ;
        CHECK (expectedVersion == sl3::sqliteCompiledVersionNumber()) ;
      }
    }
  }
}
