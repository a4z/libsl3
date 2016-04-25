#include <sl3/config.hpp>

#include <a4testing.hpp>





namespace sl3 {
namespace {



void
CheckLibVersion ()
{ // this is mostly to check the automated cmake generated config file
    BOOST_CHECK( sl3::MAJOR_VERSION == 1 );
    BOOST_CHECK( sl3::MINOR_VERSION == 12 );
    BOOST_CHECK( sl3::PATCH_VERSION == 2 );
} //----------------------------------------------------------------------------

void
CheckSqliteVersion ()
{
  BOOST_WARN_MESSAGE(build_internal_sqlite3, "not build_internal_sqlite3");

  if(build_internal_sqlite3)
    {
      using namespace sl3;
      BOOST_CHECK_EQUAL(
          sqliteCompiledVersion() , sqliteRuntimeVersion()
      );

      BOOST_CHECK(
          sqliteCompiledVersionNumber() == sqliteRuntimeVersionNumber()
      );
    }

} //--------------------------------------------------------------------------

//  assert( sqlite3_libversion_number()==SQLITE_VERSION_NUMBER );
//  assert( strcmp(sqlite3_sourceid(),SQLITE_SOURCE_ID)==0 );
//  assert( strcmp(sqlite3_libversion(),SQLITE_VERSION)==0 );


// ./sl3basics --on=config --log_level=all --run_test=whatever

a4TestAdd(
  a4test::suite("config")
  .addTest("check version", CheckLibVersion)
  .addTest("check version", CheckSqliteVersion)

) ;


}
}

