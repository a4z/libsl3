#include "../testing.hpp"
#include <sl3/database.hpp>

#include <string>

SCENARIO("ckeck dataset's type safty")
{
  using namespace sl3 ;
  GIVEN ("a database, a table and knowen data")
  {

    Database db{ ":memory:" };
    db.execute (
        "CREATE TABLE t (int INTEGER,txt TEXT, db real );"
        "INSERT INTO t VALUES (1, 'eins', 1.111) ;"
        "INSERT INTO t VALUES (2, 'zwei', 2.22) ;"
        "INSERT INTO t VALUES (3, NULL, NULL) ;"
    );

    WHEN ("having an empty dataset")
    {
      Dataset ds ;
      THEN ("assinging new dataset is possible whithout problems")
      {
        CHECK_NOTHROW(ds = db.select("SELECT * FROM t;"));
      }
    }


    WHEN ("having a typed dataset")
    {
      Dataset ds({Type::Int,Type::Text, Type::Real}) ;
      THEN ("merging compatible types work like charm")
      {
        CHECK_NOTHROW(ds.merge (db.select("SELECT 1,'hello', 2.2;",
                {Type::Int, Type::Text, Type::Real})));
      }

      THEN ("merging compatible types work not if ")
      {
        CHECK_NOTHROW(ds.merge (db.select("SELECT 1,'hello', 2.2;",
                {Type::Int, Type::Text, Type::Real})));
      }
      THEN ("merging incompatible types throws")
      {
        CHECK_THROWS_AS(ds.merge(db.select("SELECT 'he', 'll', 'o';")),
            ErrTypeMisMatch);
      }


      THEN ("assingin different types works")
      {
        auto ds1 = db.select("SELECT 'he', 'll', 'o';") ;

        CHECK_NOTHROW(ds = ds1);
        auto ds2 = db.select("SELECT * FROM t;") ;

        CHECK_NOTHROW(ds = std::move(ds2));

      }

    }

  }
}
