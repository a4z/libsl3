#include "../testing.hpp"
#include <sl3/database.hpp>
#include <sl3/columns.hpp>
#include <sl3/error.hpp>
#include <sl3/rowcallback.hpp>

#include <string>

SCENARIO("using execute with RowCallback")
{

  sl3::Database db{":memory:"};

  REQUIRE_NOTHROW(
    db.execute ("CREATE TABLE tbltest (f INTEGER);"
                "INSERT INTO tbltest VALUES (1) ;"
                "INSERT INTO tbltest VALUES (2) ;"
                "INSERT INTO tbltest VALUES (3) ;")
  );

  GIVEN("a RowCallback that counts all calls")
  {
    struct CB : sl3::RowCallback
    {
      size_t counter{0};
      bool onRow(sl3::Columns /*unusedForNow*/)
      {
        ++counter;
        return true ;
      }
    };

    CB cb;
    WHEN("executed for the test data")
    {
      db.execute ("SELECT * FROM tbltest;", cb) ;
      THEN("it was called for each of the 3 records")
      {
        CHECK (cb.counter == 3) ;
      }
    }

    WHEN("executed with incorrect sql")
    {
      auto sql = "SELECT_X * X_FROM tbltest;" ;
      THEN("a SQLite3Error is thrown")
      {
        CHECK_THROWS_AS(db.execute(sql, cb), sl3::SQLite3Error) ;
      }
    }

    WHEN("executed on a disconnected database")
    {
      auto db1 = std::move(db) ;
      auto sql = "SELECT * FROM tbltest;" ;
      THEN("a ErrNoConnection is thrown")
      {
        CHECK_THROWS_AS(db.execute(sql, cb), sl3::ErrNoConnection) ;
        CHECK_NOTHROW(db1.execute(sql, cb)) ;
      }
    }
  }

  GIVEN("a RowCallback that returns false after the second call")
  {
    struct  : sl3::RowCallback
    {
      size_t counter{0};
      bool onRow(sl3::Columns /*unusedForNow*/)
      {
        ++counter;
        if(counter == 2) return false ;
        return true ;
      }
    } cb;

    WHEN("executed for the test data")
    {
      db.execute ("SELECT * FROM tbltest;", cb) ;
      THEN("it was called 2 time")
      {
        CHECK (cb.counter == 2) ;
      }
    }
  }
  // for details about how to access data incallbacks,see callback tests
}




SCENARIO("using execute with Callback")
{

  sl3::Database db{":memory:"};

  REQUIRE_NOTHROW(
    db.execute ("CREATE TABLE tbltest (f INTEGER);"
                "INSERT INTO tbltest VALUES (1) ;"
                "INSERT INTO tbltest VALUES (2) ;"
                "INSERT INTO tbltest VALUES (3) ;")
  );

  GIVEN("a Callback that counts all calls")
  {
    size_t counter {0} ;
    auto cb = [&counter](sl3::Columns /*ignoreForNow*/) ->bool
    {
      ++counter;
      return true ;
    };

    WHEN("executed for the test data")
    {
      db.execute ("SELECT * FROM tbltest;", cb) ;
      THEN("it was called for each of the 3 records")
      {
        CHECK (counter == 3) ;
      }
    }

    WHEN("executed with incorrect sql")
    {
      auto sql = "SELECT_X * X_FROM tbltest;" ;
      THEN("a SQLite3Error is thrown")
      {
        CHECK_THROWS_AS(db.execute(sql, cb), sl3::SQLite3Error) ;
      }
    }

    WHEN("executed on a disconnected database")
    {
      auto db1 = std::move(db) ;
      auto sql = "SELECT * FROM tbltest;" ;
      THEN("a ErrNoConnection is thrown")
      {
        CHECK_THROWS_AS(db.execute(sql, cb), sl3::ErrNoConnection) ;
        CHECK_NOTHROW(db1.execute(sql, cb)) ;
      }
    }
  }

  GIVEN("a Callback that returns false after the second call")
  {
    size_t counter {0} ;
    auto cb = [&counter](sl3::Columns /*ignoreForNow*/) ->bool
    {
      ++counter;
      return counter == 2 ? false : true ;
    };
    WHEN("executed for the test data")
    {
      db.execute ("SELECT * FROM tbltest;", cb) ;
      THEN("it was called 2 time")
      {
        CHECK (counter == 2) ;
      }
    }
  }

  // for details about how to access data incallbacks,see callback tests
}








