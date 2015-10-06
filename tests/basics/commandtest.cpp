

#include <sl3/command.hpp>

#include <a4testing.hpp>
#include <sl3/columns.hpp>
#include <sl3/database.hpp>
#include <sl3/dataset.hpp>
#include <sl3/error.hpp>
#include "testdb.hpp"

namespace sl3 {
namespace {


   const auto name1="auto";
   const auto name2="numbered";
   const auto name3="named";


TestDb
testdb ()
{

  TestDb db { ":memory:" };
  db.execute( "CREATE TABLE tbl (fld1  , fld2 , fld3 );" );


  db.createStoredCommand (
      name1, "insert into tbl (fld1  , fld2 , fld3)"
      " VALUES (? , ? , ? ); ");


  db.createStoredCommand (
      name2, "insert into tbl (fld1  , fld2 , fld3)"
      " VALUES (?3 , ?2 , ?1 ); ");

// works with  :VVV  @VVV  $VVV

  db.createStoredCommand (
      name3, "insert into tbl (fld1  , fld2 , fld3)"
      " VALUES (:eins , @zwei , $drei ); ");


  return db;
}


void
TestTestDb ()
{
  BOOST_REQUIRE_NO_THROW(testdb());
}//-----------------------------------------------------------------------------


void
RunTestCommands ()
{
  BOOST_REQUIRE_NO_THROW(testdb());

  auto db = testdb();

  BOOST_REQUIRE_NO_THROW(db.getCommand (name1));
  BOOST_REQUIRE_NO_THROW(db.getCommand (name2));
  BOOST_REQUIRE_NO_THROW(db.getCommand (name3));

  auto& cmd1 = db.getCommand (name1) ;
  auto& cmd2 = db.getCommand (name2) ;
  auto& cmd3 = db.getCommand (name3) ;

  BOOST_REQUIRE_NO_THROW( cmd1.select( { 1,2,3 } ) );
  BOOST_REQUIRE_NO_THROW( cmd2.select( { 1,2,3 } ) );
  BOOST_REQUIRE_NO_THROW( cmd3.select( { 1,2,3 } ) );

  BOOST_REQUIRE(db.selectValue("SELECT COUNT(*) FROM tbl;").getInt() == 3);
}//-----------------------------------------------------------------------------

void
RunInvalidSql ()
{
  BOOST_REQUIRE_THROW(testdb().prepare("SELECT * FROM FOOOOO;"),
          SQLite3Error);

}//-----------------------------------------------------------------------------


void
ParameterNames()
{
  auto db = testdb();

  {
    auto& params = db.getCommand (name1).getParameters ();
    BOOST_CHECK_EQUAL (params.size (), 3);
    auto names = db.getCommand (name1).getParameterNames () ;
    BOOST_CHECK_EQUAL (names.at (0), "");
    BOOST_CHECK_EQUAL (names.at (1), "");
    BOOST_CHECK_EQUAL (names.at (2), "");
    BOOST_REQUIRE_THROW (names.at (3), std::out_of_range);
  }

  {
    auto& params = db.getCommand (name2).getParameters ();
    BOOST_CHECK_EQUAL (params.size (), 3);
    auto names = db.getCommand (name2).getParameterNames () ;
    BOOST_CHECK_EQUAL (names.at (0), "?1");
    BOOST_CHECK_EQUAL (names.at (1), "?2");
    BOOST_CHECK_EQUAL (names.at (2), "?3");
    BOOST_REQUIRE_THROW (names.at (3), std::out_of_range);
  }

  {
    auto& params = db.getCommand (name3).getParameters ();
    BOOST_CHECK_EQUAL (params.size (), 3);
    auto names = db.getCommand(name3).getParameterNames () ;
    BOOST_CHECK_EQUAL (names.at (0), ":eins");
    BOOST_CHECK_EQUAL (names.at (1), "@zwei");
    BOOST_CHECK_EQUAL (names.at (2), "$drei");
    BOOST_REQUIRE_THROW (names.at (100), std::out_of_range);
  }

}//-----------------------------------------------------------------------------

void
CreateWithParameters()
{
  auto db = testdb();

  auto toless = { DbValue(1), DbValue(2)} ;
  auto justright = { DbValue(1), DbValue(2), DbValue(3)} ;
  auto tomuch = { DbValue(1), DbValue(2), DbValue(3), DbValue(4)} ;
  auto sql = "insert into tbl (fld1  , fld2 , fld3) VALUES (?3 , ?2 , ?1 ); ";

  BOOST_REQUIRE_THROW( db.prepare(sql, toless), ErrTypeMisMatch );
  BOOST_REQUIRE_NO_THROW( db.prepare(sql, justright));
  BOOST_REQUIRE_THROW( db.prepare(sql, tomuch), ErrTypeMisMatch );

}//-----------------------------------------------------------------------------


void
RebindParameters()
{
  auto db = testdb();

  auto& cmd1 = db.getCommand (name1) ;
  BOOST_REQUIRE_NO_THROW( cmd1.select( { 1,2,3 } ) );

  BOOST_REQUIRE_NO_THROW( cmd1.execute() );

  BOOST_REQUIRE(db.selectValue("SELECT COUNT(*) FROM tbl;").getInt() == 2);
  for (auto&& row : db.select ("SELECT * FROM tbl;"))
    {
      BOOST_REQUIRE (row.size () == 3);
      BOOST_REQUIRE (row [0].getInt () == 1);
      BOOST_REQUIRE (row [1].getInt () == 2);
      BOOST_REQUIRE (row [2].getInt () == 3);
    }


}//-----------------------------------------------------------------------------

void
SetParameters()
{

  auto toless = { DbValue(1), DbValue(2)} ;
  auto justright = { DbValue(1), DbValue(2), DbValue(3)} ;
  auto tomuch = { DbValue(1), DbValue(2), DbValue(3), DbValue(4)} ;

  auto db = testdb();
  auto& cmd1 = db.getCommand (name1) ;

  BOOST_REQUIRE_THROW( cmd1.setParameters(toless), ErrTypeMisMatch );
  BOOST_REQUIRE_NO_THROW( cmd1.setParameters(justright) );
  BOOST_REQUIRE_THROW( cmd1.setParameters(tomuch), ErrTypeMisMatch );


  auto othertypes = { DbValue("1"), DbValue(2), DbValue(3) } ;
// get rid of the variants,
  BOOST_REQUIRE_NO_THROW( cmd1.resetParameters(justright) );
  BOOST_REQUIRE_THROW( cmd1.setParameters(othertypes), ErrTypeMisMatch );

  BOOST_REQUIRE_NO_THROW( cmd1.resetParameters(othertypes) );
  BOOST_REQUIRE_THROW( cmd1.setParameters(justright), ErrTypeMisMatch );
  BOOST_REQUIRE_THROW( cmd1.resetParameters(tomuch), ErrTypeMisMatch );


}//-----------------------------------------------------------------------------


void
UseQueryCallback()
{
  auto db = testdb();


  BOOST_REQUIRE_NO_THROW( db.getCommand(name1).select( { 1,2,3 } ) );
  BOOST_REQUIRE_NO_THROW( db.getCommand(name1).select( { 1,2,3 } ) );
  BOOST_REQUIRE_NO_THROW( db.getCommand(name2).select( { 1,2,3 } ) );
  BOOST_REQUIRE_NO_THROW( db.getCommand(name2).select( { 1,2,3 } ) );

  {
    DbValues::container_type vals;
    auto cb = [&vals](Columns cols) -> bool
      { // ony one hime here
        BOOST_REQUIRE_EQUAL(vals.size(),0) ;
        BOOST_REQUIRE_EQUAL(cols.count(),3) ;
        vals.emplace_back(cols(0));
        vals.emplace_back(cols(1));
        vals.emplace_back(cols(2));
        return false ; // stop after first ececution
      };
    auto cmd = db.prepare("SELECT * FROM tbl WHERE fld1=?;");
    cmd.execute(cb, {DbValue{1}} );
    BOOST_REQUIRE_EQUAL(vals.size(),3) ;
    BOOST_REQUIRE_EQUAL(vals[0].getInt(),1) ;
    BOOST_REQUIRE_EQUAL(vals[1].getInt(),2) ;
    BOOST_REQUIRE_EQUAL(vals[2].getInt(),3) ;

  }


  {
    int counter = 0;
    auto cb = [&counter](Columns cols) -> bool
      { // ony one hime here
        counter += 1;
        BOOST_REQUIRE_EQUAL(cols.count(),3) ;
        BOOST_REQUIRE_EQUAL(cols(0).getInt(),3);
        BOOST_REQUIRE_EQUAL(cols(1).getInt(),2);
        BOOST_REQUIRE_EQUAL(cols(2).getInt(),1);
        return true ;
      };
    auto cmd = db.prepare("SELECT * FROM tbl WHERE fld1=?;");
    cmd.execute(cb, {DbValue{3}} );
    BOOST_REQUIRE_EQUAL(counter,2) ;
    cmd.execute(cb ); // re run cmd
    BOOST_REQUIRE_EQUAL(counter,4) ;
  }




}//-----------------------------------------------------------------------------

void
Selects ()
{
  auto db = testdb ();
  {
    auto& cmd = db.getCommand (name1) ;
    BOOST_REQUIRE_NO_THROW (
      cmd.execute ({ DbValue{1},DbValue{"2"},DbValue{3.3}})
    );

    BOOST_REQUIRE_NO_THROW (
      cmd.execute ({ DbValue{4},DbValue{"5"},DbValue{6.6}})
    );


  }

  {
    auto cmd = db.prepare ("SELECT * FROM tbl;") ;
    auto ds = cmd.select () ;
    BOOST_REQUIRE (ds.size () == 2) ;
    Types oktypes {Type::Int, Type::Text, Type::Real};
    BOOST_REQUIRE_NO_THROW (cmd.select (oktypes)) ;

    Types falsetypes {Type::Int, Type::Text, Type::Text};
    BOOST_REQUIRE_THROW (cmd.select (falsetypes), ErrTypeMisMatch );
  }

  {
    auto cmd = db.prepare ("SELECT * FROM tbl WHERE fld1 = ?;") ;
    auto params = DbValues{DbValue{1}} ;
    auto ds = cmd.select (params) ;
    BOOST_REQUIRE (ds.size () == 1) ;
    Types oktypes {Type::Int,  Type::Text, Type::Real};
    BOOST_REQUIRE_NO_THROW (ds = cmd.select (params, oktypes)) ;
    BOOST_REQUIRE (ds.size () == 1) ;
    Types falsetypes {Type::Int,  Type::Text, Type::Text};
    BOOST_REQUIRE_THROW (cmd.select (params, falsetypes), ErrTypeMisMatch );
  }


}//-----------------------------------------------------------------------------


void SqliteError()
{
  auto db = testdb();
  auto cmd = db.prepare ("SELECT * FROM tbl;") ;
  db.execute ("DROP TABLE tbl;") ;
  BOOST_REQUIRE_THROW(cmd.select (), SQLite3Error);
}//-----------------------------------------------------------------------------


void
ForCoverage()
{
  auto db = testdb();
  {
    const auto& cmd1 = db.getCommand (name1) ;
    const auto& params = cmd1.getParameters();
    BOOST_CHECK_EQUAL(params.size(), 3);
    const auto& val = cmd1.getParameter(0);
    BOOST_CHECK(val.isNull());
    BOOST_REQUIRE_THROW(cmd1.getParameter(4), ErrOutOfRange );
  }

  {
    auto& cmd1 = db.getCommand (name1) ;
    auto& params = cmd1.getParameters();
    BOOST_CHECK_EQUAL(params.size(), 3);
    auto val = cmd1.getParameter(0);
    val.set("some value");
    BOOST_CHECK(!val.isNull());
  }

}//-----------------------------------------------------------------------------




a4TestAdd(
  a4test::suite("command")
  .addTest("testdb", TestTestDb)
  .addTest("testcommands", RunTestCommands)
  .addTest("invalidsql", RunInvalidSql)
  .addTest("parameternames", ParameterNames)
  .addTest("create with parameters", CreateWithParameters)
  .addTest("rebind parameters", RebindParameters)
  .addTest("setparameters", SetParameters)
  .addTest("user query calllback ", UseQueryCallback)
  .addTest("selects", Selects)
  .addTest("sqliteError", SqliteError)
  .addTest("codecoverage", ForCoverage)
) ;




}
}

