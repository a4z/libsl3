#include <a4testing.hpp>

#include <sl3/columns.hpp>
#include <sl3/database.hpp>
#include <sl3/error.hpp>
#include <sl3/rowcallback.hpp>

namespace sl3 {
namespace {

Database
testdb ()
{

  Database db { ":memory:" };
  db.execute ("CREATE TABLE tbl(f1 INTEGER, f2 TEXT);"
              "INSERT INTO tbl (f1, f2 ) VALUES (1,'one');"
              "INSERT INTO tbl (f1, f2 ) VALUES (2,'two');"
              "INSERT INTO tbl (f1, f2 ) VALUES (3,'three');");

  return db;
}


void
checkCallbackReturn ()
{

  auto db = testdb ();
  {
    struct : public RowCallback
    {
      int rowcount = 0;
      bool
      onRow (Columns)
      {
        rowcount += 1;
        return true;
      }
    } handleTrue;

    BOOST_REQUIRE_EQUAL(handleTrue.rowcount, 0);
    db.prepare ("SELECT * FROM tbl;").execute (handleTrue);
    BOOST_REQUIRE_EQUAL(handleTrue.rowcount, 3); // OnHandleRow called 3 times

    // return false after first call,
    struct : public RowCallback
    {
      int rowcount = 0;
      bool
      onRow (Columns)
      {
        rowcount += 1;
        return false;
      }
    } handleFalse;

    BOOST_REQUIRE_EQUAL(handleFalse.rowcount, 0);
    db.prepare ("SELECT * FROM tbl;").execute (handleFalse);
    BOOST_REQUIRE_EQUAL(handleFalse.rowcount, 1); // OnHandleRow  called once
  }

  // same with callback functions
  {
    int countTrue = 0;
    auto handleTrue = [&](Columns) -> bool
      {
        countTrue+=1;
        return true;
      };

    int countFalse = 0;
    auto handleFalse = [&](Columns) -> bool
      {
        countFalse+=1;
        return false;
      };

    auto cmd = db.prepare ("SELECT * FROM tbl;");
    cmd.execute (handleTrue);
    cmd.execute (handleFalse);

    BOOST_REQUIRE_EQUAL(countTrue, 3);
    BOOST_REQUIRE_EQUAL(countFalse, 1);
  }

}



void
checkColumnNamesAndCount ()
{

  auto testcb = [&](Columns columns) -> bool
    {
      BOOST_CHECK (columns.count () == 2);
      BOOST_CHECK (columns.getName (0) == "f1");
      BOOST_CHECK (columns.getName (1) == "f2");
      BOOST_CHECK_THROW (columns.getName (2) , ErrOutOfRange);

      return false;
    };
  auto db = testdb ();
  db.execute ("SELECT * FROM tbl;", testcb);

  db.execute ("SELECT 1 AS '2';", [&](Columns columns)
    {
      BOOST_CHECK (columns.count () == 1);
      BOOST_CHECK_EQUAL (columns.getName (0) ,"2");
      BOOST_CHECK_THROW (columns.getName (1) , ErrOutOfRange);
      return false;
    });
}


void
checkColumnAt ()
{
  auto testcb = [&](Columns columns) -> bool
    {
      auto names = columns.getNames();

      BOOST_CHECK_NO_THROW (columns.at(0));
      BOOST_CHECK (names.at(0) == "f1" );

      BOOST_CHECK_NO_THROW (columns.at(1));
      BOOST_CHECK (names.at (1) == "f2");

      BOOST_CHECK_THROW (columns.at (2), ErrOutOfRange);
      BOOST_CHECK_THROW (columns.at (2, Type::Int), ErrOutOfRange);
      BOOST_CHECK_THROW (columns.at (1, Type::Int), ErrTypeMisMatch);
      BOOST_CHECK_NO_THROW (columns.at (1, Type::Text)) ;
      BOOST_CHECK_NO_THROW (columns.at (0, Type::Int)) ;

      return false;
    };
  auto db = testdb ();
  db.execute ("SELECT * FROM tbl;", testcb);




}


void
checkGetRow ()
{
  auto testcb = [&](Columns columns) -> bool
    {
      BOOST_CHECK_THROW( columns.at(2) , ErrOutOfRange );

      BOOST_REQUIRE_NO_THROW ( columns.getRow ());
      BOOST_REQUIRE ( columns.getRow ().size () == 2);

      Types wrongTypes1 { Type::Real  };
      Types wrongTypes2 { {Type::Real, Type::Blob, Type::Blob} };
      Types wrongTypes3 { {Type::Real, Type::Blob} };
      Types rightTypes { {Type::Int, Type::Text} };

      BOOST_REQUIRE_THROW( columns.getRow (wrongTypes1), ErrTypeMisMatch );
      BOOST_REQUIRE_THROW( columns.getRow (wrongTypes2), ErrTypeMisMatch );
      BOOST_REQUIRE_THROW( columns.getRow (wrongTypes3), ErrTypeMisMatch );
      BOOST_REQUIRE_NO_THROW( columns.getRow (rightTypes));

      auto variants = columns.getRow ();
      BOOST_REQUIRE ( variants [0].getType () == Type::Variant);
      BOOST_REQUIRE ( variants [1].getType () == Type::Variant);

      auto typed = columns.getRow (rightTypes);
      BOOST_REQUIRE ( typed [0].getType () == Type::Int);
      BOOST_REQUIRE ( typed [1].getType () == Type::Text);
      return false;
    };
  auto db = testdb ();
  db.execute ("SELECT * FROM tbl;", testcb);
}


void
columnTypes()
{

  auto db = testdb ();
  db.execute ("CREATE TABLE Types(f1, f2, f3, f4, f5);");

  auto insert = db.prepare("INSERT INTO Types (f1, f2, f3, f4, f5) "
                                " VALUES (?, ?, ?, ?, ?)");

  DbValues types {DbValue (1), DbValue ("string"),
                  DbValue (1.2),  DbValue (Blob {1}), DbValue (Type::Int)} ;


  insert.execute (types)  ;


  db.execute ("SELECT * FROM Types;",
        [&](Columns cols)
        {
          BOOST_CHECK (cols.getType (0) == Type::Int);
          BOOST_CHECK (cols.getType (1) == Type::Text);
          BOOST_CHECK (cols.getType (2) == Type::Real);
          BOOST_CHECK (cols.getType (3) == Type::Blob);
          BOOST_CHECK (cols.getType (4) == Type::Null);

          BOOST_CHECK_THROW (cols.getType (5), ErrOutOfRange);

          BOOST_CHECK (cols.getInt (0) == 1);
          BOOST_CHECK_THROW (cols.getInt (10), ErrOutOfRange);

          BOOST_CHECK (cols.getInt64 (0) == 1);
          BOOST_CHECK_THROW (cols.getInt64 (10), ErrOutOfRange);

          BOOST_CHECK (cols.getText (1) == "string");
          BOOST_CHECK_THROW (cols.getText (10), ErrOutOfRange);


          BOOST_CHECK_THROW (cols.getReal (10), ErrOutOfRange);
          BOOST_CHECK_THROW (cols.getBlob (10), ErrOutOfRange);

          return false;
        } );

}


void
columnSizes()
{

  auto db = testdb ();
  db.execute ("CREATE TABLE Types(f1, f2, f3, f4, f5);");

  auto insert = db.prepare("INSERT INTO Types (f1, f2, f3, f4, f5) "
                                " VALUES (?, ?, ?, ?, ?)");

  DbValues types {DbValue (1), DbValue ("string"),
                  DbValue (1.2),  DbValue (Blob {1}), DbValue (Type::Int)} ;

  insert.execute (types)  ;


  db.execute ("SELECT * FROM Types;",
        [&](Columns cols)
        {
          BOOST_CHECK ( cols.getSize (1) == std::string("string").size ());

          BOOST_CHECK ( cols.getSize (3) == 1);
          BOOST_CHECK ( cols.getSize (4) == 0);

          BOOST_CHECK_THROW (cols.getSize (5), ErrOutOfRange);

          return false;
        } );

}




void
indexAccess()
{

  auto db = testdb ();
  db.execute ("SELECT * FROM tbl;",
        [&](Columns cols)
        {
          BOOST_CHECK_THROW (cols.at (0, Type::Text), ErrTypeMisMatch) ;
          BOOST_CHECK_NO_THROW (cols.at (1, Type::Text)) ;

          BOOST_CHECK_NO_THROW (cols (0, Type::Int)) ;
          BOOST_CHECK_THROW (cols (1 , Type::Int), ErrTypeMisMatch) ;

          return false;
        } );

}




void
forCoverage()
{

  auto db = testdb ();
  db.execute ("SELECT * FROM tbl;",
        [&](Columns qr)
        {
          BOOST_CHECK( qr.get_stmt () != nullptr );
          return false;
        } );

}



a4TestAdd(
    a4test::suite("callback")
    .addTest("returnval", checkCallbackReturn)
    .addTest("column names & count", checkColumnNamesAndCount)
    .addTest("column index", checkColumnAt)
    .addTest("getRow", checkGetRow)
    .addTest("columnTypes", columnTypes)
    .addTest("columnSizes", columnSizes)
    .addTest("indexAccess", indexAccess)
    .addTest("coverage", forCoverage)
);

}
}

