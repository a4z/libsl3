#include "../testing.hpp"

#include <sl3/database.hpp>
#include <sl3/error.hpp>

#include <string>
#include <utility>

SCENARIO("creating a database")
{
  GIVEN ("a valid and an invalid database name/path")
  {
    auto good = ":memory:" ;
    auto bad = "/this/does/not/exist/123" ;

    WHEN ("using the valid name/path")
    {
      THEN ("constructing the db works")
      {
        CHECK_NOTHROW(sl3::Database{good});
      }
    }

    WHEN ("using the invalid name/path")
    {
      THEN ("constructing the db works")
      {
        CHECK_THROWS_AS(sl3::Database{bad}, sl3::SQLite3Error) ;
      }
    }
  }
}



SCENARIO("creating some test data")
{
  GIVEN ("an im memory database")
  {
    sl3::Database db{":memory:"};

    WHEN ("executing invalid SQL")
    {
      auto wrong_sql = "CRATE NABLE foo (f1, f2);" ;
      THEN ("execute triggers an exception")
      {
        CHECK_THROWS_AS (db.execute (wrong_sql), sl3::SQLite3Error) ;
      }

    }

    WHEN ("creating a table and some test data")
    {
      db.execute (
          "CREATE TABLE tbltest (intFld INTEGER,txtFld TEXT, dblFld real );"
          "INSERT INTO tbltest VALUES (1, 'eins', 1.111) ;"
          "INSERT INTO tbltest VALUES (2, 'zwei', 2.22) ;"
          "INSERT INTO tbltest VALUES (3, NULL, NULL) ;"
      );

      THEN ("I can veryfy that the table and some test data exists")
      {
        CHECK (db.getTotalChanges () == 3) ;
        CHECK_NOTHROW (db.execute (std::string{"DELETE FROM tbltest;"})) ;
        CHECK (db.getTotalChanges () == 6) ;
        CHECK (db.getRecentlyChanged () == 3) ;
      }

    }
  }
}


SCENARIO("move construct a database")
{
  GIVEN("a db with a table")
  {
    sl3::Database db{":memory:"};

    db.execute ("CREATE TABLE tbltest (f INTEGER);"
                  "INSERT INTO tbltest VALUES (1) ;");

    CHECK_NOTHROW (db.execute ("SELECT COUNT(*) FROM tbltest;")) ;

    WHEN ("Moveconstruct a new db from an existing one")
    {
      sl3::Database db1{std::move(db)} ;
      THEN("the new constructed db has data")
      {
        CHECK_NOTHROW (db1.execute ("SELECT COUNT(*) FROM tbltest;")) ;
      }

      AND_THEN("the moved db is disconnected")
      {
        CHECK_THROWS_AS (db.execute ("SELECT COUNT(*) FROM tbltest;"),
                          sl3::ErrNoConnection) ;

//        those are protected, does this make sense?
//        adding a derivating from a database
//        CHECK(db.db() == nullptr) ;
//        CHECK(db1.db() != nullptr) ;

      }
    }
  }
}


SCENARIO("check the connections and if derivate works")
{
  GIVEN("a derivated db that let me access db property")
  {
    struct MyDb : public sl3::Database
    {
      using sl3::Database::Database ;
      using sl3::Database::db;
    };

    MyDb db{":memory:"};

    db.execute ("CREATE TABLE tbltest (f INTEGER);"
                  "INSERT INTO tbltest VALUES (1) ;");

    CHECK_NOTHROW (db.execute ("SELECT COUNT(*) FROM tbltest;")) ;

    WHEN ("Moveconstruct a new db from an existing one")
    {
      MyDb db1{std::move(db)} ;
      THEN("the new constructed db has data")
      {
        CHECK_NOTHROW (db1.execute ("SELECT COUNT(*) FROM tbltest;")) ;
      }

      AND_THEN("the moved db is disconnected")
      {
        CHECK_THROWS_AS (db.execute ("SELECT COUNT(*) FROM tbltest;"),
                          sl3::ErrNoConnection) ;

        CHECK(db.db() == nullptr) ;
        CHECK(db1.db() != nullptr) ;
      }
    }
  }
}



SCENARIO("prepare commands")
{
  GIVEN("a db with a table")
  {
    sl3::Database db{":memory:"};

    db.execute ("CREATE TABLE tbltest (f INTEGER);"
                  "INSERT INTO tbltest VALUES (1) ;");

    REQUIRE_NOTHROW (db.execute ("SELECT COUNT(*) FROM tbltest;")) ;

    WHEN ("compiling a valid sql statement")
    {
      auto sql = "SELECT * FROM tbltest;" ;

      THEN("a new command is created")
      {
        CHECK_NOTHROW(
          auto cmd = db.prepare (sql) ;
          (void)cmd;
        );
      }
    }

    WHEN ("compiling an invalid sql statement")
    {
      auto sql = "SELECT_X * X_FROM tbltest;" ;

      THEN("creating a command throws")
      {
        CHECK_THROWS_AS((void)db.prepare (sql), sl3::SQLite3Error);
      }
    }

    WHEN ("compiling on a disconnected db")
    {
      auto db1 = std::move (db) ;
      auto sql = "SELECT * FROM tbltest;" ;
      auto sql1 = "INSERT INTO tbltest VALUES (?);" ;

      THEN("creating a command throws")
      {
        CHECK_THROWS_AS((void)db.prepare (sql), sl3::ErrNoConnection);

        sl3::DbValues vals{ sl3::DbValue{1} } ;
        CHECK_THROWS_AS((void)db.prepare (sql1, vals), sl3::ErrNoConnection);
      }
    }

    WHEN ("compiling sql with the correct number of parameters")
    {
      auto sql = "INSERT INTO tbltest VALUES (?);" ;
      THEN("creating a command succeeds")
      {
        sl3::DbValues vals{ sl3::DbValue{1} } ;
        CHECK_NOTHROW((void)db.prepare (sql,vals));
      }
    }

    WHEN ("compiling sql with a wrong number of parameters")
    {
      auto sql = "INSERT INTO tbltest VALUES (?);" ;

      THEN("creating a command throws")
      {
        sl3::DbValues vals{ sl3::DbValue{1}, sl3::DbValue{2} } ;
        CHECK_THROWS((void)db.prepare (sql,sl3::DbValues{}));
        CHECK_THROWS((void)db.prepare (sql,vals));
        // Details about the exception in the command tests
      }
    }
  }
}

SCENARIO("using execute with RowCallback")
{

  GIVEN("a db with some data and the most simple RowCallback ever")
  {
    sl3::Database db{":memory:"};

    REQUIRE_NOTHROW(
      db.execute ("CREATE TABLE tbltest (f INTEGER);"
                  "INSERT INTO tbltest VALUES (1) ;")
    );

    struct : sl3::RowCallback
    {
      bool onRow(sl3::Columns /*unusedForNow*/)
      {
        return true ;
      }
    } cb;

    WHEN("executed with valid sql")
    {
      auto sql = "SELECT * FROM tbltest;" ;
      THEN("it works without an error")
      {
        CHECK_NOTHROW(db.execute(sql, cb)) ;
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
      sl3::Database db1{std::move(db)} ;
      auto sql = "SELECT * FROM tbltest;" ;
      THEN("a ErrNoConnection is thrown")
      {
        CHECK_THROWS_AS(db.execute(sql, cb), sl3::ErrNoConnection) ;
        CHECK_NOTHROW(db1.execute(sql, cb)) ;
      }
    }
  }

  // for details about how to use RowCallback, see (row)callback tests
}




SCENARIO("using execute with Callback")
{

  GIVEN("a db with some test data and the most simple Callback ")
  {
    sl3::Database db{":memory:"};

    REQUIRE_NOTHROW(
      db.execute ("CREATE TABLE tbltest (f INTEGER);"
                  "INSERT INTO tbltest VALUES (1) ;")
    );

    sl3::Database::Callback cb = [](sl3::Columns /*ignoreForNow*/)
    {
      return true ;
    };

    WHEN("executed with valid SQL")
    {
      auto sql = "SELECT * FROM tbltest;" ;
      THEN("it works without an error")
      {
        CHECK_NOTHROW(db.execute(sql, cb)) ;
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

  // for details about how to use callbacks,see (row)callback tests
}





SCENARIO("selecting datasets")
{

  GIVEN("a db with some test data")
  {
    sl3::Database db{":memory:"};

    REQUIRE_NOTHROW(
      db.execute ("CREATE TABLE tbltest (f INTEGER);"
                  "INSERT INTO tbltest VALUES (1) ;")
    );

    using namespace sl3;

    WHEN("select with valid SQL")
    {
      auto sql = "SELECT * FROM tbltest;" ;
      THEN("it works without an error")
      {
        CHECK_NOTHROW((void)db.select(sql)) ;
        CHECK_NOTHROW((void)db.select(sql, {Type::Variant})) ;
      }
    }

    WHEN("executed with incorrect sql")
    {
      auto sql = "SELECT_X * X_FROM tbltest;" ;
      THEN("a SQLite3Error is thrown")
      {
        CHECK_THROWS_AS((void)db.select(sql), sl3::SQLite3Error) ;
        CHECK_THROWS_AS((void)db.select(sql,{Type::Variant}), sl3::SQLite3Error) ;
      }
    }

    WHEN("executed on a disconnected database")
    {
      auto db1 = std::move(db) ;
      auto sql = "SELECT * FROM tbltest;" ;
      THEN("a ErrNoConnection is thrown")
      {
        CHECK_THROWS_AS((void)db.select(sql), sl3::ErrNoConnection) ;
        CHECK_NOTHROW((void)db1.select(sql)) ;
        CHECK_THROWS_AS((void)db.select(sql,{Type::Variant}), sl3::ErrNoConnection) ;
        CHECK_NOTHROW((void)db1.select(sql,{Type::Variant})) ;
      }
    }
  }

  // for details about how to use datasets,see dataset tests
}


SCENARIO("selecting values")
{

  GIVEN("a db")
  {
    sl3::Database db{":memory:"};

    REQUIRE_NOTHROW(
      db.execute ("CREATE TABLE tbltest (f INTEGER);"
                  "INSERT INTO tbltest VALUES (1) ;")
    );


    using namespace sl3 ;;


    WHEN("select a value with valid SQL")
    {
      auto sql = "SELECT 1;" ;
      THEN("it works without an error")
      {
        CHECK_NOTHROW((void)db.selectValue(sql)) ;
        CHECK_NOTHROW((void)db.selectValue(sql, Type::Int)) ;
      }
    }

    WHEN("executed with incorrect sql")
    {
      auto sql = "SELECT_X 1;" ;
      THEN("a SQLite3Error is thrown")
      {
        CHECK_THROWS_AS((void)db.selectValue(sql), SQLite3Error) ;
        CHECK_THROWS_AS((void)db.selectValue(sql, Type::Int), SQLite3Error) ;
      }
    }

    WHEN("executed on a disconnected database")
    {
      auto db1 = std::move(db) ;
      auto sql = "SELECT 1;" ;
      THEN("a ErrNoConnection is thrown")
      {
        CHECK_THROWS_AS((void)db.selectValue(sql), ErrNoConnection) ;
        CHECK_NOTHROW((void)db1.selectValue(sql)) ;

        CHECK_THROWS_AS((void)db.selectValue(sql, Type::Int), ErrNoConnection) ;
        CHECK_NOTHROW((void)db1.selectValue(sql, Type::Int)) ;

      }
    }

    WHEN("select a value with the wrong type")
    { // this is actually a Columns test, but I let it here for now
      auto sql = "SELECT 1;" ;
      THEN("a type mis match is thrown")
      {
        CHECK_THROWS_AS((void)db.selectValue(sql, Type::Text), ErrTypeMisMatch);
      }
    }

  }

  // for details about how to use dbvalues,see dbvalues tests
}




SCENARIO("accessing database change properties")
{

  GIVEN("an database with an empty table")
  {
    sl3::Database db{":memory:"};

    WHEN ("no action is done")
    {
      THEN ("no changes have been counted")
      {
          CHECK(db.getTotalChanges() == 0) ;
          CHECK(db.getRecentlyChanged() == 0) ;
          CHECK(db.getLastInsertRowid() == 0) ;
      }
    }

    WHEN ("inserting 2 rows")
    {
      REQUIRE_NOTHROW(
        db.execute ("CREATE TABLE tbltest (f INTEGER);"
                    "INSERT INTO tbltest VALUES (1) ;"
                    "INSERT INTO tbltest VALUES (1) ;"));

      THEN ("changes have been counted")
      {
          CHECK(db.getTotalChanges() == 2) ;
          CHECK(db.getRecentlyChanged() == 1) ;
          CHECK(db.getLastInsertRowid() == 2) ;
      }
    }

    WHEN ("accessing the properties on a moved from db")
    {
      REQUIRE_NOTHROW(
        db.execute ("CREATE TABLE tbltest (f INTEGER);"
                    "INSERT INTO tbltest VALUES (1) ;"
                    "INSERT INTO tbltest VALUES (1) ;"));

      sl3::Database db1 {std::move(db)} ;

      THEN ("no undefined behavior but exceptions are thorwn")
      {

        CHECK_THROWS_AS((void)db.getTotalChanges(), sl3::ErrNoConnection) ;
        CHECK_THROWS_AS((void)db.getRecentlyChanged(), sl3::ErrNoConnection) ;
        CHECK_THROWS_AS((void)db.getLastInsertRowid(), sl3::ErrNoConnection) ;

        CHECK(db1.getTotalChanges() == 2) ;
        CHECK(db1.getRecentlyChanged() == 1) ;
        CHECK(db1.getLastInsertRowid() == 2) ;
      }
    }
  }
}


SCENARIO("accessing database error properties")
{

  GIVEN("an database an empty datbase")
  {
    sl3::Database db{":memory:"};

    WHEN ("nothing has been done wrong")
    {
      THEN ("no errors are reported")
      {
          CHECK(db.getMostRecentErrCode() == 0) ;
          CHECK(db.getMostRecentErrMsg() == sl3::getErrStr(0)) ;
      }
    }

    WHEN ("something has been done wrong")
    {
      REQUIRE_THROWS(db.execute ("CREATE SCHNALBE tbltest (f INTEGER);"));
      THEN ( "errors are reported")
      {
          CHECK(db.getMostRecentErrCode() != 0) ;
          CHECK(db.getMostRecentErrMsg() != sl3::getErrStr(0)) ;
      }
    }

    WHEN ("something has been done wrong, and than something that worked")
    {
      REQUIRE_THROWS(db.execute ("CREATE SCHNALBE tbltest (f INTEGER);"));
      REQUIRE_NOTHROW(db.execute ("CREATE TABLE tbltest (f INTEGER);"));
      THEN ( "errors are cleared")
      {
          CHECK(db.getMostRecentErrCode() == 0) ;
          CHECK(db.getMostRecentErrMsg() == sl3::getErrStr(0)) ;
      }
    }

    WHEN ("accessing the properties on a moved from db")
    {
      REQUIRE_THROWS(db.execute ("CREATE SCHNALBE tbltest (f INTEGER);"));

      sl3::Database db1 {std::move(db)} ;

      THEN ("exceptions are thrown, but not fot ")
      {
        CHECK_THROWS_AS((void)db.getMostRecentErrCode(), sl3::ErrNoConnection) ;
        CHECK_THROWS_AS((void)db.getMostRecentErrMsg(), sl3::ErrNoConnection) ;

        CHECK(db1.getMostRecentErrCode() != 0) ;
        CHECK(db1.getMostRecentErrMsg() != sl3::getErrStr(0)) ;

      }
    }
  }
}


SCENARIO("using transactions")
{

  GIVEN("an empty database ")
  {
    sl3::Database db{":memory:"};

    WHEN ("using a transaction to create table and data but do not commit")
    {
      {
        auto trans = db.beginTransaction() ;
        REQUIRE_NOTHROW(
           db.execute ("CREATE TABLE tbltest (f INTEGER);"
                       "INSERT INTO tbltest VALUES (1) ;"
                       "INSERT INTO tbltest VALUES (1) ;"));
      }
      THEN ("no changes have been made and the table does not exist")
      {
        REQUIRE_THROWS(db.execute ("SELECT * FROM tbltest;"));
      }
    }

    WHEN ("using a transaction to create table and data and call commit")
    {
      {
        auto trans = db.beginTransaction() ;
        REQUIRE_NOTHROW(
           db.execute ("CREATE TABLE tbltest (f INTEGER);"
                       "INSERT INTO tbltest VALUES (1) ;"
                       "INSERT INTO tbltest VALUES (1) ;"));
        trans.commit() ;
      }
      THEN ("changes have been made and the table exists")
      {
        REQUIRE_NOTHROW(db.execute ("SELECT * FROM tbltest;"));
      }
    }


    WHEN ("moving a transaction and commit on the moved from trans")
     {
        {
           auto trans = db.beginTransaction() ;
           REQUIRE_NOTHROW(
              db.execute ("CREATE TABLE tbltest (f INTEGER);"
                          "INSERT INTO tbltest VALUES (1) ;"
                          "INSERT INTO tbltest VALUES (1) ;"));
           sl3::Database::Transaction trans1 = std::move(trans) ;

           trans.commit() ;
        }
       THEN ("commit on the moved from object does not effect the database")
       {
         REQUIRE_THROWS(db.execute ("SELECT * FROM tbltest;"));
       }

     }

    WHEN ("moving a transaction and commit")
     {
        {
           auto trans = db.beginTransaction() ;
           REQUIRE_NOTHROW(
              db.execute ("CREATE TABLE tbltest (f INTEGER);"
                          "INSERT INTO tbltest VALUES (1) ;"
                          "INSERT INTO tbltest VALUES (1) ;"));
           sl3::Database::Transaction trans1 = std::move(trans) ;

           trans1.commit() ;
        }
       THEN ("commit did effect the database")
       {
         REQUIRE_NOTHROW(db.execute ("SELECT * FROM tbltest;"));
       }

     }

  }
}


