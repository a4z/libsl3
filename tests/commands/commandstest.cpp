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


SCENARIO ("executing with callback and parameters")
{
  using namespace sl3 ;
  GIVEN ("a database with some test data and a RowCallBack ")
  {
    Database db{":memory:"};
    db.execute( "CREATE TABLE t (f);"
        "INSERT INTO  t VALUES (1);"
        "INSERT INTO  t VALUES (2);"
        "INSERT INTO  t VALUES (3);");


    struct : RowCallback
    {
      size_t counter{0};
      bool onRow(sl3::Columns)
      {
        ++counter;
        return true ;
      }
    } cb;
    WHEN ("executing a command with parameters that give me 1 record")
    {
      auto cmd = db.prepare ("SELECT * FROM t WHERE f = ?");
      cmd.execute (cb, parameters(2)) ;
      THEN ("then I got the one expected record")
      {
        CHECK (cb.counter == 1) ;
      }
    }
  }
}



SCENARIO ("handling constraint violations")
{
  using namespace sl3 ;
  GIVEN ("a database with a table that requires unique values")
  {
    Database db{":memory:"};
    db.execute( "CREATE TABLE t (f UNIQUE);");

    WHEN ("having a value")
    {
      auto cmd = db.prepare ("INSERT INTO t VALUES(?);");
      cmd.execute (parameters(1)) ;
      THEN ("then inserting a value again is a exception")
      {
        CHECK_THROWS_AS (cmd.execute (), SQLite3Error) ;
      }
    }
  }
}


SCENARIO ("binding values of all types and using select to check the result")
{
  using namespace sl3 ;
  GIVEN ("a database with a table that can take all types and")
  {
    Database db{":memory:"};
    db.execute( "CREATE TABLE t (fi, fr, fs, fb, fn);");
    auto sqin = "INSERT INTO t VALUES (?,?,?,?,?);";

    auto b = [](int v) -> std::byte { return std::byte(v); };

    DbValues params = {
      DbValue{1} ,
      DbValue {2.2},
      DbValue {"drei"},
      DbValue {Blob{b(1) , b(2) , b(3) , b(4)}},
      DbValue{Type::Variant}
    } ;

    auto cmd = db.prepare (sqin) ;

    WHEN ("inserting values of all types")
    {
      CHECK_NOTHROW (cmd.execute (params)) ;
      THEN ("selecting all data returns the inserted data")
      {
        auto ds = db.select ("SELECT * FROM t;") ;
        REQUIRE_EQ (ds.size (), 1);
        REQUIRE_EQ (ds[0].size () , params.size ()) ;
        REQUIRE_EQ (ds[0][0].getInt (), params[0].getInt());
        REQUIRE_EQ (ds[0][1].getReal (), params[1].getReal());
        REQUIRE_EQ (ds[0][2].getText (), params[2].getText());
        REQUIRE_EQ (ds[0][3].getBlob (), params[3].getBlob());
        REQUIRE (ds[0][4].isNull ()) ;
      }
      THEN ("selecting all data with concrete types will retun typed info")
      {
        auto types = Types {Type::Int, Type::Real, Type::Text, Type::Blob, Type::Variant} ;
        auto ds = db.select ("SELECT * FROM t;", types) ;
        REQUIRE_EQ (ds.size (), 1);
        REQUIRE_EQ (ds[0].size (), types.size ());
        for (size_t i = 0; i < ds[0].size (); ++i)
          REQUIRE_EQ (ds[0][i].dbtype () , types[i]) ;
      }
      THEN ("selecting all data with wrong types will throw")
      {
        auto types = Types {Type::Int, Type::Int, Type::Int, Type::Int, Type::Int} ;
        REQUIRE_THROWS_AS ((void)db.select ("SELECT * FROM t;", types), ErrTypeMisMatch) ;

      }
      THEN ("selecting all data with wrong numver of types will throw")
      {
        auto types = Types {Type::Int, Type::Real, Type::Text, Type::Blob} ;
        REQUIRE_THROWS_AS ((void)db.select ("SELECT * FROM t;", types), ErrTypeMisMatch) ;

      }
    }
  }
}
