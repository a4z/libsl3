#include "../testing.hpp"
#include <sl3/database.hpp>

#include <string>

SCENARIO("dataset creation and defautl operatores")
{
  using namespace sl3 ;
  GIVEN ("a database, a table and knowen data")
  {

    Database db{ ":memory:" };
    db.execute (
        "CREATE TABLE t (int INTEGER,txt TEXT, dbl real );"
        "INSERT INTO t VALUES (1, 'eins', 1.111) ;"
        "INSERT INTO t VALUES (2, 'zwei', 2.22) ;"
        "INSERT INTO t VALUES (3, NULL, NULL) ;"
    );

    const Types types{Type::Int, Type::Text, Type::Real} ;

    WHEN ("having an empty dataset")
    {
      Dataset ds ;
      THEN ("assinging new dataset is possible whithout problems")
      {
        CHECK_NOTHROW(ds = db.select("SELECT * FROM t;"));
        CHECK_EQ(ds.getIndex("int"), 0) ;
        CHECK_EQ(ds.getIndex("txt"), 1) ;
        CHECK_EQ(ds.getIndex("dbl"), 2) ;

        CHECK_EQ(ds.size (), 3) ;
        CHECK_EQ(ds.at(0).size (), 3) ;

      }
    }


    WHEN ("having a typed dataset")
    {
      auto ds = db.select("SELECT * FROM t;", types) ;

      THEN ("the fieds are not variants")
      {
        for(const auto &row : ds)
          {
            REQUIRE(row.size() == types.size()) ;
            REQUIRE(row.at(0).getType() == types[0]) ;
            REQUIRE(row.at(1).getType() == types[1]) ;
            REQUIRE(row.at(2).getType() == types[2]) ;
          }
      }
    }


    WHEN ("creating a dataset via a rvalue")
    {
      CHECK_NOTHROW(Dataset ds{std::move(db.select("SELECT * FROM t;"))});

      THEN ("the ds include all data and names")
      {
        Dataset ds{std::move(db.select("SELECT * FROM t;"))} ;

        CHECK_EQ(ds.getIndex("int"), 0) ;
        CHECK_EQ(ds.getIndex("txt"), 1) ;
        CHECK_EQ(ds.getIndex("dbl"), 2) ;
      }
    }

  }
}


SCENARIO("merging datasets")
{
  using namespace sl3 ;
   GIVEN ("a database, a table and knowen data")
   {

     Database db{ ":memory:" };
     db.execute (
         "CREATE TABLE t (int INTEGER,txt TEXT, dbl real );"
         "INSERT INTO t VALUES (1, 'eins', 1.111) ;"
         "INSERT INTO t VALUES (2, 'zwei', 2.22) ;"
         "INSERT INTO t VALUES (3, NULL, NULL) ;"
     );

    WHEN ("having a typed dataset")
    {
      Dataset ds({Type::Int,Type::Text, Type::Real}) ;
      THEN ("merging compatible types works")
      {
        CHECK_NOTHROW(ds.merge (db.select("SELECT 1,'hello', 2.2;",
                {Type::Int, Type::Text, Type::Real})));
      }

      THEN ("merging differn field count throws")
      {
        CHECK_THROWS_AS(ds.merge (
            db.select("SELECT 1,'hello';",{Type::Int, Type::Text})),
                        ErrTypeMisMatch);
      }
      THEN ("merging incompatible types throws")
      {
        CHECK_THROWS_AS(ds.merge(db.select("SELECT 'he', 'll', 'o';")),
            ErrTypeMisMatch);
      }

    }

    WHEN ("having an dataset with names fields")
    {
      Dataset ds = db.select("SELECT * FROM t;");
      CHECK_EQ(ds.getIndex("int"), 0) ;
      CHECK_EQ(ds.getIndex("txt"), 1) ;
      CHECK_EQ(ds.getIndex("dbl"), 2) ;

      THEN ("merging a dataset with different fieldnames throws")
      {

        auto ds1 = db.select("SELECT 1 as foo, 'eins' as bar, 2.2 as bas;");
        CHECK_THROWS_AS(ds.merge(ds1), ErrTypeMisMatch) ;
      }

    }

  }
}
