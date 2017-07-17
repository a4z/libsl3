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
        cmd.execute () ;
        auto val = db.selectValue("SELECT COUNT(*) FROM tbl WHERE fld1 = 1 ;") ;
        CHECK  (val.getInt () == 2) ;
      }
    }

    WHEN ("creating a compiled command using incorrect SQL")
    {
      THEN ("this will fail with an exception")
      {
      }
    }

    WHEN ("creating a command with parameters")
    {
      THEN ("the parameters can be set")
      {
      }
    }

    WHEN ("creating a command with names parameters")
    {
      THEN ("the parameter names can be queried")
      {
      }
    }

    WHEN ("creating a command with typed parameters")
    {
      THEN ("the dbvalue type rules apply")
      {
      }

      THEN ("the paramters can be reeted to different ones")
      {
      }

    }
 }
}
