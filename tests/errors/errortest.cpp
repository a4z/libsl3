#include "../testing.hpp"
#include <sl3/error.hpp>
#include <sl3/container.hpp>
#include <map>
#include <vector>

// these tests are mainly to raise coverage, and protect against accidental code removal

SCENARIO ("checking error codes")
{
  using namespace sl3;
  GIVEN ("the expected error codes")
  {
    std::map<sl3::ErrCode, std::string> expected = {
        {ErrCode::SQL3Error, "SQLite3Error"},
        {ErrCode::NoConnection, "NoConnection"},
        {ErrCode::OutOfRange, "OutOfRange"},
        {ErrCode::TypeMisMatch, "TypeMisMatch"},
        {ErrCode::NullValueAccess , "NullValueAccess"},
        {ErrCode::UNEXPECTED , "UNEXPECTED"},
    };

    WHEN ("checking the expected error code names")
    {
        THEN("all names are as expected"){

            for(auto given: expected){
                const std::string wanted = sl3::ErrCodeName(given.first);
                CHECK (given.second == wanted);
            }
            

        }
    }
  }

  GIVEN ("an SQL3Error Type"){
    
    using Err = ErrType<ErrCode::SQL3Error>;

    WHEN ("creating an ErrType with some random SqLite error value")
    {
        const Err e{1, "hello"};
        THEN("the Error has the expected type")
        {
            CHECK(e.getId() == sl3::ErrCode::SQL3Error );
        }
    }
  }

  GIVEN ("the concrete sl3 error aliases")
  {
    const ErrNoConnection noConnection;
    const ErrOutOfRange outOfRange;
    const ErrTypeMisMatch typeMisMatch;
    const ErrNullValueAccess nullValueAccess;
    const ErrUnexpected unexpected;

    WHEN ("asking each error for its identifier")
    {
      THEN ("each alias reports the matching ErrCode")
      {
        CHECK(noConnection.getId() == sl3::ErrCode::NoConnection);
        CHECK(outOfRange.getId() == sl3::ErrCode::OutOfRange);
        CHECK(typeMisMatch.getId() == sl3::ErrCode::TypeMisMatch);
        CHECK(nullValueAccess.getId() == sl3::ErrCode::NullValueAccess);
        CHECK(unexpected.getId() == sl3::ErrCode::UNEXPECTED);
      }
    }
  }

}



SCENARIO ("Container index errors throw sl3 errors")
{
    GIVEN ("A Container") 
    {
        using iContainer = sl3::Container <std::vector<int>>;
        WHEN("access index out of range via at function")
        {
            iContainer ic { {1,2,3} };
            const iContainer cic = ic;

            THEN("exceptions are thrown")
            {
                CHECK_THROWS_AS(ic.at(3), sl3::ErrOutOfRange );
                CHECK_THROWS_AS(cic.at(3),
                 sl3::ErrOutOfRange );
            }
        }
    }

}
