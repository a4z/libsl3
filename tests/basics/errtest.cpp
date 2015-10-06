#include <a4testing.hpp>

#include <sstream>
#include <sl3/database.hpp>
#include <sl3/error.hpp>

namespace sl3 {
namespace {

void
toStream ()
{


  std::stringstream ss ;

  ss << ErrTypeMisMatch ("Test");
  BOOST_CHECK_EQUAL ( ss.str(), "sl3::TypeMisMatch:Test" ) ;

  ss.str(std::string());
  ss << ErrUnexpected("Test") ;
  BOOST_CHECK_EQUAL ( ss.str(), "sl3::UNEXPECTED:Test" ) ;

  ss.str(std::string());
  ss << ErrOutOfRange("Test") ;
  BOOST_CHECK_EQUAL ( ss.str(), "sl3::OutOfRange:Test" ) ;

  ss.str(std::string());
  ss << ErrNullValueAccess("Test") ;
  BOOST_CHECK_EQUAL ( ss.str(), "sl3::NullValueAccess:Test" ) ;

  ss.str(std::string());
  ss << ErrNoConnection("Test") ;
  BOOST_CHECK_EQUAL ( ss.str(), "sl3::NoConnection:Test" ) ;

  ss.str(std::string());
  auto s3err = SQLite3Error(0,"sl3", "Test") ;
  ss << s3err ;
  BOOST_CHECK_EQUAL ( s3err.SQLiteErrorCode (), 0) ;
  BOOST_CHECK_EQUAL ( s3err.SQLiteErrorMessage (), "sl3" ) ;


} //----------------------------------------------------------------------------

void
Unexpected ()
{
  BOOST_REQUIRE_THROW (raiseErrUnexpected (), ErrUnexpected);
}


a4TestAdd(
    a4test::suite("errtest")
    .addTest("tostream", toStream)
    .addTest("unexpected", Unexpected)

);

}
}
