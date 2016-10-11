#include <a4testing.hpp>

#include <sstream>
#include <iostream>

#include <sl3/columns.hpp>
#include <sl3/database.hpp>
#include <sl3/rowcallback.hpp>
#include "testdb.hpp"

namespace sl3 {
namespace {

void
OpenMemDb ()
{
  BOOST_CHECK_NO_THROW(Database{ ":memory:" });

  BOOST_CHECK_THROW(Database{"/no/such/file"}, SQLite3Error);

} //----------------------------------------------------------------------------

void
CreateTable ()
{
  Database db{ ":memory:" };
  BOOST_CHECK_NO_THROW(db.execute ("CREATE TABLE tbl(f1 INTEGER, f2 TEXT);"));

} //----------------------------------------------------------------------------

void
ExecuteInsert ()
{
  Database db{ ":memory:" };
  BOOST_CHECK_NO_THROW(db.execute ("CREATE TABLE tbl(f1 INTEGER, f2 TEXT);"));

  std::string bad = "INSERT INTO FOOO (f1, f2 ) VALUES (1,'one');";
  std::string good = "INSERT INTO tbl (f1, f2 ) VALUES (1,'one');";

  BOOST_CHECK_THROW(db.execute (bad), SQLite3Error);

  BOOST_CHECK_NO_THROW(db.execute (good));


} //----------------------------------------------------------------------------



void
SelectSingleValue ()
{
  Database db (":memory:");

  db.execute ("CREATE TABLE tbl(f1 INTEGER, f2 TEXT);");
  db.execute ("INSERT INTO tbl (f1, f2 ) VALUES (1,'one');"
              "INSERT INTO tbl (f1, f2 ) VALUES (2,'two');");

    {
      DbValue val = db.selectValue ("select count(*) from tbl;");
      BOOST_CHECK(val.getInt () == 2);
    }
    { // always first column of first row .. or should this throw ...?
      DbValue val = db.selectValue ("select * from tbl;");
      BOOST_CHECK(val.getInt () == 1);
    }

    { // check for null
      DbValue val = db.selectValue ("select * from tbl where f1 = 1000;");
      BOOST_CHECK(val.isNull ());
    }


    { // check typed
      DbValue val = db.selectValue ("select f1 from tbl where f1 = 1;",
            Type::Int);
      BOOST_CHECK (!val.isNull ());
      BOOST_CHECK_EQUAL (val.getType () ,Type::Int);
      BOOST_CHECK (val.getStorageType () == Type::Int);
    }

    { // check typed null
      DbValue val = db.selectValue ("select f1 from tbl where f1 = 1000;",
            Type::Int);
      BOOST_CHECK (val.isNull ());
      BOOST_CHECK (val.getType () == Type::Int);
      BOOST_CHECK (val.getStorageType () == Type::Null);
    }

    { // check typed throws
      BOOST_CHECK_THROW (
   db.selectValue ("select f1 from tbl where f1 = 1;", Type::Text),
   ErrTypeMisMatch);
    }

} //----------------------------------------------------------------------------

void
UseTransaction ()
{
  Database db (":memory:");
  db.execute ("CREATE TABLE tbl(f1 INTEGER, f2 TEXT);");

    {
      auto trans = db.beginTransaction ();
      db.execute ("INSERT INTO tbl (f1, f2 ) VALUES (1,'one');");
    }
  BOOST_CHECK(db.selectValue ("select count(*) from tbl;").getInt () == 0);

    {
      auto trans = db.beginTransaction ();
      db.execute ("INSERT INTO tbl (f1, f2 ) VALUES (1,'one');");
      trans.commit ();
    }
  BOOST_CHECK(db.selectValue ("select count(*) from tbl;").getInt () == 1);

    { // also test the move constructor
      auto trans = db.beginTransaction ();
      auto mtrans = std::move(trans) ;
      db.execute ("INSERT INTO tbl (f1, f2 ) VALUES (1,'one');");
    }
  BOOST_CHECK(db.selectValue ("select count(*) from tbl;").getInt () == 1);

    {
      auto trans = db.beginTransaction ();
      db.execute ("INSERT INTO tbl (f1, f2 ) VALUES (1,'one');");
      trans.commit ();
    }
  BOOST_CHECK(db.selectValue ("select count(*) from tbl;").getInt () == 2);
} //----------------------------------------------------------------------------


void
ChangesPorperties()
{

  Database db (":memory:");
  db.execute ("CREATE TABLE tbl(f1 INTEGER, f2 TEXT);");

  BOOST_REQUIRE(db.getTotalChanges()==0 );
  BOOST_REQUIRE(db.getRecentlyChanged()==0 );

  db.execute ("INSERT INTO tbl (f1, f2 ) VALUES (1,'one');");
  BOOST_REQUIRE(db.getTotalChanges()==1 );
  BOOST_REQUIRE(db.getRecentlyChanged()==1 );

  db.execute ("INSERT INTO tbl (f1, f2 ) VALUES (2,'two');");
  BOOST_REQUIRE(db.getTotalChanges()==2 );
  BOOST_REQUIRE(db.getRecentlyChanged()==1 );

  BOOST_REQUIRE(db.getLastInsertRowid()==2 );

  // this is just for code coverage, to have it here
  BOOST_REQUIRE(0 <= sqliteThreadSafeCompileOption() &&
                sqliteThreadSafeCompileOption() <= 2 );

  db.execute ("DELETE  FROM tbl;");
  BOOST_REQUIRE(db.getTotalChanges()==4 );
  BOOST_REQUIRE(db.getRecentlyChanged()==2 );

}//--------------------------------------------------------------------------


void
CommandCleanup()
{
  // this is hard to test, mostly to see in coverage that the
  // right paths are taken
  // create tb, get a command, make db close, than close command

  Database* db = new Database{ ":memory:" };

  db->execute ("CREATE TABLE tbl(f1 INTEGER, f2 TEXT);");

  try
    {
      db->prepare("SELECT * FROM tbl1111;");
    }
  catch(const sl3::Error& e)
    {
      BOOST_CHECK(e.getId() == ErrCode::SQL3Error);
    }


  {
    auto somecmd = db->prepare("SELECT * FROM tbl;");

    BOOST_CHECK_NO_THROW(delete db );
    // this close should call finalize ....
    db = nullptr;

    BOOST_CHECK_THROW(somecmd.execute(), ErrNoConnection );


  }
  // command is now in undefinde state....

}



void
ForCoverage2()
{
  struct MyDb : public  Database
  {
    using Database::Database ;
    bool check () {return db () != nullptr ;}
  };

  MyDb db { ":memory:" };

  try
    {
      BOOST_CHECK (db.check ());
      db.prepare("SELECT * FROM tbl111;");
    }
  catch(const sl3::Error& )
    {
      auto ec = db.getMostRecentErrCode ();
      BOOST_CHECK (ec != 0);
      BOOST_CHECK_EQUAL (db.getMostRecentErrMsg (),
        "no such table: tbl111" );

      BOOST_CHECK (db.getErrStr (ec).size  () > 10);
    }

}


void
execCallback()
{

  Database db { ":memory:" };

  std::string sql = "CREATE TABLE tbl(f1 INTEGER, f2 TEXT);"
              "INSERT INTO tbl (f1, f2 ) VALUES (1,'one');" ;

  BOOST_CHECK_NO_THROW (db.execute (sql));

  sql = "select * from tbl;" ;
  int counter = 0;
  BOOST_CHECK_EQUAL (counter , 0);
  db.execute (sql, [&counter](Columns cols) -> bool
    {
      BOOST_CHECK (cols (0).getInt () == 1); ;
      counter += 1 ;
      return true ;
    });

  BOOST_CHECK_EQUAL (counter , 1);

  sql = "INSERT INTO tbl (f1, f2 ) VALUES (1,'one');" ;
  BOOST_CHECK_NO_THROW (db.execute (sql));

  struct : public RowCallback
  {
    int rowcount = 0;
    bool
    onRow (Columns)
    {
      rowcount += 1;
      return true;
    }
  } cb ;

  sql = "select * from tbl;" ;

  BOOST_CHECK_NO_THROW (db.execute (sql ,cb));

  BOOST_CHECK_EQUAL (cb.rowcount , 2);

}

void checkTestdb ()
{
  TestDb db ;

  db.execute ("CREATE TABLE tbl(f1 INTEGER, f2 TEXT);"
              "INSERT INTO tbl (f1, f2 ) VALUES (1,'one');") ;

  BOOST_CHECK_NO_THROW(
  db.createStoredCommand ("foo", "SELECT * FROM tbl;") );

  BOOST_CHECK_THROW(
  db.createStoredCommand ("foo", "SELECT * FROM tbl;"), sl3::ErrUnexpected);

  BOOST_CHECK_NO_THROW(
  db.createStoredCommand ("bar", "SELECT * FROM tbl;") );


  BOOST_CHECK(db.eraseCommand ("foo")== true);
  BOOST_CHECK(db.eraseCommand ("foo")== false);

  BOOST_CHECK_NO_THROW(db.getCommand("bar"));
  BOOST_CHECK_NO_THROW(db.eraseCommands()) ;
  BOOST_CHECK_THROW(db.getCommand("bar"), sl3::ErrUnexpected);

}


a4TestAdd(
    a4test::suite("dbtest")
    .addTest("open memory db", OpenMemDb)
    .addTest("create table", CreateTable)
    .addTest("execut inster", ExecuteInsert)
    .addTest("select single value", SelectSingleValue)
    .addTest("transaction", UseTransaction)
    .addTest("changes proerties", ChangesPorperties)
    .addTest("command cleanup", CommandCleanup)
    .addTest("coverage2", ForCoverage2)
    .addTest("execCallback", execCallback)
    .addTest("testdb", checkTestdb)

);

}
}
