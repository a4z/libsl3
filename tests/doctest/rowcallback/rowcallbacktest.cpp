#include "../testing.hpp"

#include <sl3/database.hpp>
#include <sl3/error.hpp>

#include <string>

SCENARIO("Check RowCallback and Callback return values")
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
}


SCENARIO("Check RowCallback start and end calls")
{

  sl3::Database db{":memory:"};

  REQUIRE_NOTHROW(
    db.execute ("CREATE TABLE tbltest (f INTEGER);"
                "INSERT INTO tbltest VALUES (1) ;"
                "INSERT INTO tbltest VALUES (2) ;"
                "INSERT INTO tbltest VALUES (3) ;")
  );

  GIVEN("a RowCallback that counts all calls and fantasi on start/end counts")
  {
    struct CB : sl3::RowCallback
    {
      size_t counter{0};
      size_t onstart{10000};
      size_t onend{10000};

      void onStart ()
      {
        onstart  = counter ;
      }

      void onEnd ()
      {
        onend  = counter ;
      }

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
      THEN("on start/end counts have logical values")
      {
        CHECK (cb.counter == 3) ;
        CHECK (cb.onstart == 0) ;
        CHECK (cb.onstart == 0) ;
        CHECK (cb.onend == 3) ;
      }
    }
  }
}


SCENARIO("testing column names and index properties")
{

  sl3::Database db{":memory:"};

  GIVEN("a statement that returns 2 columns with known names")
  {
    auto sql = "SELECT 1 AS first, 'hello' AS second;";
    REQUIRE_NOTHROW (db.execute(sql)) ;

    WHEN ("accessing columns via index")
    {
      THEN("valid indexes will not throw")
      {
        db.execute(sql, [](sl3::Columns cols){
          REQUIRE(cols.count () == 2) ;
          CHECK_NOTHROW((void)cols.at (0)) ;
          CHECK_NOTHROW((void)cols.at (0, sl3::Type::Int)) ;
          CHECK_NOTHROW((void)cols.at (1)) ;
          CHECK_NOTHROW((void)cols.at (1, sl3::Type::Text)) ;
          return false;
        });
      }

      THEN("invalid indexes will throw throw out of range")
      {
        db.execute(sql, [](sl3::Columns cols){
          REQUIRE(cols.count () == 2) ;
          CHECK_THROWS_AS(cols.at (2), sl3::ErrOutOfRange) ;
          CHECK_THROWS_AS(cols.at (2, sl3::Type::Variant), sl3::ErrOutOfRange);
          return false;
        });
      }
    }

    WHEN ("accessing columns via index")
    {
      THEN("accessing out of range will not throw")
      {
        db.execute(sql, [](sl3::Columns cols){
          REQUIRE(cols.count () == 2) ;
          CHECK_NOTHROW((void)cols (100)) ;
          CHECK_NOTHROW((void)cols (1000)) ;

          return true;
        });
      }

    }


    WHEN("accessing the names via index")
    {
      THEN ("valid indexes return expected results")
      {
        db.execute(sql, [](sl3::Columns cols){
          REQUIRE(cols.count () == 2) ;
          CHECK(cols.getName (0) == "first") ;
          CHECK(cols.getName (1) == "second") ;
          auto names = cols.getNames();
          CHECK(cols.getName (0) == names.at(0)) ;
          CHECK(cols.getName (1) == names.at(1)) ;
          return false;
        });
      }

      THEN ("invalid indexes throw out of range")
      {
        db.execute(sql, [](sl3::Columns cols){
          REQUIRE(cols.count () == 2) ;
          CHECK_THROWS_AS(cols.getName (2), sl3::ErrOutOfRange) ;
          return false;
        });
      }
    }
  }
}

SCENARIO("getting automatic type info")
{
  GIVEN("a test db")
  {
    sl3::Database db{":memory:"};

    WHEN ("selecting an integer value")
    {
      auto sql  = "SELECT 1 ;" ;
      THEN ("columns will auto detect int")
      {
        db.execute(sql, [](sl3::Columns cols){
          CHECK(cols.getType (0) == sl3::Type::Int) ;
          return false;
        });
      }
    }
    WHEN ("selecting a real value")
    {
      auto sql  = "SELECT 1.23 ;" ;
      THEN ("columns will auto detect real")
      {
        db.execute(sql, [](sl3::Columns cols){
          CHECK(cols.getType (0) == sl3::Type::Real) ;
          return false;
        });
      }
    }
    WHEN ("selecting a text value")
    {
      auto sql  = "SELECT 'hello' ;" ;
      THEN ("columns will auto detect text")
      {
        db.execute(sql, [](sl3::Columns cols){
          CHECK(cols.getType (0) == sl3::Type::Text) ;
          return false;
        });
      }
    }
    WHEN ("selecting a blob value")
    {
      auto sql  = "SELECT x'12345678ABCDEF';" ;
      THEN ("columns will auto detect blob")
      {
        db.execute(sql, [](sl3::Columns cols){
          CHECK(cols.getType (0) == sl3::Type::Blob) ;
          return false;
        });
      }
    }
    WHEN ("selecting a NULL")
    {
      auto sql  = "SELECT NULL ;" ;
      THEN ("columns will auto detect NULL")
      {
        db.execute(sql, [](sl3::Columns cols){
          CHECK(cols.getType (0) == sl3::Type::Null) ;
          return false;
        });
      }
    }

    AND_WHEN ("accessing the type out of index")
    {
      auto sql  = "SELECT NULL ;" ;
      THEN ("an out of range exception is thrown")
      {
        db.execute(sql, [](sl3::Columns cols){
          CHECK_THROWS_AS(cols.getType (1), sl3::ErrOutOfRange) ;
          return false;
        });
      }
    }

  }

}

SCENARIO("getting DbValues from columns")
{
  GIVEN("a record with known data and known types")
  {
    sl3::Database db{":memory:"};
    auto sql = "SELECT 1 as int, "
        " 2.3 as real, "
        " 'hello' as text, "
        " x'1F' as blob, "
        " NULL as noval; ";

    WHEN("verify the test data")
    {
      THEN ("fields and types are as expected")
      {
        REQUIRE_NOTHROW(db.execute(sql)) ;
        db.execute(sql, [](sl3::Columns cols){
          CHECK(cols.getType (0) == sl3::Type::Int) ;
          CHECK(cols.getType (1) == sl3::Type::Real) ;
          CHECK(cols.getType (2) == sl3::Type::Text) ;
          CHECK(cols.getType (3) == sl3::Type::Blob) ;
          CHECK(cols.getType (4) == sl3::Type::Null) ;
          return false;
        });
      }

    }


    WHEN ("accessing values as they are")
    {
      THEN ("value type properties are set correct")
      {
          db.execute(sql, [](sl3::Columns cols){
            CHECK(cols(0).getType () == sl3::Type::Variant) ;
            CHECK(cols(0).getStorageType () == sl3::Type::Int) ;
            CHECK(cols(1).getType () == sl3::Type::Variant) ;
            CHECK(cols(1).getStorageType () == sl3::Type::Real) ;
            CHECK(cols(2).getType () == sl3::Type::Variant) ;
            CHECK(cols(2).getStorageType () == sl3::Type::Text) ;
            CHECK(cols(3).getType () == sl3::Type::Variant) ;
            CHECK(cols(3).getStorageType () == sl3::Type::Blob) ;
            CHECK(cols(4).getType () == sl3::Type::Variant) ;
            CHECK(cols(4).getStorageType () == sl3::Type::Null) ;
            return true;
          });
      }
    }

    WHEN ("accessing values unchecked out of range")
    {
      THEN ("values are always null")
      {   // unsure if this is not undefined
        // TODO , remove the unchecked access,
        // from SQLite docu or
        // 'if the column index is out of range, the result is undefined'

          db.execute(sql, [](sl3::Columns cols){
            CHECK(cols(100).getType () == sl3::Type::Variant) ;
            CHECK(cols(100).getStorageType () == sl3::Type::Null) ;
            return true;
          });
      }
    }


    WHEN ("accessing values with wanted types")
    {
      THEN ("requesting correct types will work")
      {

      }

      THEN ("requesting in correct types will always throw")
      {

      }

    }

  }
}


