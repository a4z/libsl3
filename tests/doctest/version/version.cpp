#include "../testing.hpp"
#include <sl3/config.hpp>

#include <string>

/// [doxygen_test]
SCENARIO("check sqlite versions for library and app")
{
  // this can be used as an example to add into your project
  // assure the libsl3 was compiled using the same header as you app

  GIVEN ("the sqlite version libsl3 was comiled with")
  {
    std::string libUsedVersion = sl3::sqliteCompiledVersion () ;

    WHEN ("asking for runtime version")
    {
      std::string appUsedVersion = sl3::sqliteRuntimeVersion () ;

      THEN ("these version need to be the same")
      {
        // TODO what to do if we use dynamice versions of this ?
        CHECK (libUsedVersion == appUsedVersion) ;
      }

    }
  }
}
/// [doxygen_test]
//
// TODO those are not ported
//
//    BOOST_CHECK(
//        sqliteCompiledVersionNumber() == sqliteRuntimeVersionNumber()
//    );
//
//    // check that the cmake file was updated correct
//    auto expectedVersion =  3*1000000 + sl3::PATCH_VERSION ;
//
//    // check that the cmake file was updated correct
//    BOOST_CHECK(expectedVersion == sqliteCompiledVersionNumber()) ;
