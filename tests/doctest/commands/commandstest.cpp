#include "../testing.hpp"
#include <sl3/database.hpp>

#include <string>

SCENARIO("using precompiled commands")
{
  using namespace sl3 ;
  GIVEN ("cmake genertated config")
  {
    Database db{":memory:"};
    db.execute( "CREATE TABLE tbl (fld1  , fld2 , fld3 );" );

    WHEN ("creating a simple compiled command")
    {
      auto cmd = db.prepare ( "insert into tbl (fld1  , fld2 , fld3)"
                              " VALUES (? , ? , ? ); ");

      THEN ("parameters are detected and set to Null")
      {
        CHECK (cmd.getParameters().size () == 3) ;
      }


      THEN ("it can be executed")
      {
        CHECK (db.getTotalChanges () == 0) ;
        cmd.execute (parameters(1,2,3)) ;
        CHECK (db.getTotalChanges () == 1) ;
        cmd.execute () ; // same are applied again
        auto val = db.selectValue("SELECT COUNT(*) FROM tbl WHERE fld1 = 1 ;") ;
        CHECK  (val.getInt () == 2) ;
      }
    }

    WHEN ("creating a command via invalid sql")
    {
      THEN("this throws en error")
      {
          REQUIRE_THROWS_AS(db.prepare("SELECT * FROM FOOOOO;"),
                  SQLite3Error);
      }
    }

    WHEN ("creating a command with parameters")
    {
      auto cmd = db.prepare ( "insert into tbl (fld1  , fld2 , fld3)"
                              " VALUES (? , ? , ? ); ",
                              parameters(1,2,3));
      THEN ("the parameters are set")
      {
        CHECK (cmd.getParameters().size () == 3) ;
        cmd.execute () ;
        CHECK (db.getTotalChanges () == 1) ;
        cmd.execute () ;
        auto val = db.selectValue("SELECT COUNT(*) FROM tbl WHERE fld1 = 1 ;") ;
        CHECK  (val.getInt () == 2) ;
      }

      THEN ("the parameters can be re-set")
      {
        cmd.resetParameters(parameters(6,7,8)) ;
        cmd.execute () ;
        auto val = db.selectValue("SELECT COUNT(*) FROM tbl WHERE fld1 = 6 ;") ;
        CHECK  (val.getInt () == 1) ;
      }

    }

    WHEN ("applying  wrong numbers of parameters")
    {
      auto sql = "insert into tbl (fld1  , fld2 , fld3)"
          " VALUES (? , ? , ? ); " ;
      auto param2 = parameters(1,2) ;
      auto param4 = parameters(1,2,3,4) ;

      THEN ("this will throw a type miss match")
      {
        CHECK_THROWS_AS ((void)db.prepare (sql, param2), ErrTypeMisMatch) ;
        CHECK_THROWS_AS ((void)db.prepare (sql, param4), ErrTypeMisMatch) ;
        auto cmd = db.prepare(sql) ;
        CHECK_THROWS_AS (cmd.resetParameters(parameters(1,2)), ErrTypeMisMatch);
        CHECK_THROWS_AS (cmd.resetParameters(param4), ErrTypeMisMatch) ;

        CHECK_THROWS_AS (cmd.setParameters(parameters(1,2)), ErrTypeMisMatch);
        CHECK_THROWS_AS (cmd.setParameters(param4), ErrTypeMisMatch) ;
       }
    }


    WHEN ("creating a command with names parameters")
    {
      auto sql1 = "insert into tbl (fld1  , fld2 , fld3)"
          " VALUES (:eins , @zwei , $drei); " ;

      auto sql2 = "insert into tbl (fld1  , fld2 , fld3)"
          " VALUES (?1 , ?2 , ?1); " ;

      THEN ("the parameter names can be queried")
      {
        auto cmd1 = db.prepare(sql1) ;
        auto names1 = cmd1.getParameterNames () ;
        CHECK(names1.size() == 3) ;
        CHECK_EQ (names1.at (0), ":eins");
        CHECK_EQ (names1.at (1), "@zwei");
        CHECK_EQ (names1.at (2), "$drei");
        REQUIRE_THROWS_AS (names1.at (100), std::out_of_range) ;

        auto cmd2 = db.prepare(sql2) ;
        auto names2 = cmd2.getParameterNames () ;
        CHECK(names2.size() == 2) ;
        CHECK_EQ (names2.at (0), "?1");
        CHECK_EQ (names2.at (1), "?2");
      }
    }


    WHEN ("creating a command typed parameters")
    {
      auto sql1 = "insert into tbl (fld1  , fld2 , fld3)"
          " VALUES (:eins , @zwei , $drei); " ;

      THEN ("the command is movable")
      {
        auto cmd1 = db.prepare(sql1) ;
        cmd1.getParameter(0).set(100) ;
        cmd1.getParameter(1).set("hello") ;
        const Command cmd2 {std::move(cmd1)} ;
        auto names = cmd2.getParameterNames () ;
        CHECK(names.size() == 3) ;
        CHECK_EQ (names.at (0), ":eins");
        CHECK_EQ (names.at (1), "@zwei");
        CHECK_EQ (names.at (2), "$drei");
        CHECK_EQ (cmd2.getParameters().size(), 3);
        CHECK_EQ (cmd2.getParameter(0).getInt(), 100);
        CHECK_EQ (cmd2.getParameter(1).getText(), "hello");

      }


    }
 }
}
