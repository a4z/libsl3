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
      THEN ("assinging new dataset is possible without problems")
      {
        CHECK_NOTHROW(ds = db.select("SELECT * FROM t;"));
        CHECK_EQ(ds.getIndex("int"), 0) ;
        CHECK_EQ(ds.getIndex("txt"), 1) ;
        CHECK_EQ(ds.getIndex("dbl"), 2) ;
        CHECK_THROWS_AS(ds.getIndex("abc"), ErrOutOfRange);

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
            REQUIRE(row.at(0).dbtype() == types[0]) ;
            REQUIRE(row.at(1).dbtype() == types[1]) ;
            REQUIRE(row.at(2).dbtype() == types[2]) ;
          }
      }
    }


    WHEN ("creating a dataset via a rvalue")
    {
      auto dsm = db.select("SELECT * FROM t;") ;
      CHECK_NOTHROW(Dataset ds{std::move(dsm)});

      THEN ("the ds include all data and names")
      {
        dsm = db.select("SELECT * FROM t;");
        Dataset ds{std::move(dsm)} ;

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
   GIVEN ("a database and a typed empty dataset")
   {

     Database db{ ":memory:" };
     Dataset ds({Type::Int,Type::Text, Type::Real}) ;
    WHEN ("having a type compatible dataset")
    {
      const Types types = {Type::Int, Type::Text, Type::Real} ;
      auto ds1 =db.select("SELECT 1,'hello', 2.2;", types );

      THEN ("merging  works")
      {
        CHECK_NOTHROW(ds.merge (ds1));
      }
      AND_THEN ("merging compatible DbValues works")
      {

        CHECK_NOTHROW(ds.merge(ds1.at(0)));
      }
    }

    WHEN ("merge a dataset with different field count")
    {
      const auto dsdifferent = db.select("SELECT 1,'hello';",
                                         {Type::Int, Type::Text});
      THEN ("merging this dataset throws")
      {
        CHECK_THROWS_AS(ds.merge (dsdifferent), ErrTypeMisMatch);
      }
      AND_THEN ("merging just a row of this dataset also throws")
      {
        CHECK_THROWS_AS(ds.merge (dsdifferent.at(0)), ErrTypeMisMatch);
      }
    }

    WHEN ("having a dataset with incompatible types")
    {
      const auto dsdifferent = db.select("SELECT 'he', 'll', 'o';") ;

      THEN ("merging this ds throws")
      {
        CHECK_THROWS_AS(ds.merge(dsdifferent), ErrTypeMisMatch);
      }
      THEN ("merging a row of this ds throws")
      {
        CHECK_THROWS_AS(ds.merge(dsdifferent.at(0)), ErrTypeMisMatch);
      }

      AND_WHEN ("reset the fields  but not the types")
      {
        THEN ("merging this ds still thorws")
        {
          ds.reset() ;
          CHECK_THROWS_AS(ds.merge(dsdifferent), ErrTypeMisMatch);
        }
      }

      AND_WHEN ("reset the fields to compatible types")
      {
        THEN ("merging works")
        {
          ds.reset({Type::Variant,Type::Variant, Type::Variant}) ;
          CHECK_NOTHROW(ds.merge(dsdifferent));
          CHECK(ds.size () == 1);
        }
      }
    }

    WHEN ("having an dataset with names fields")
    {
      Dataset ds = db.select("SELECT 1 as int, 'eins' as txt, 2.2 as dbl;");
      CHECK_EQ(ds.getIndex("int"), 0) ;
      CHECK_EQ(ds.getIndex("txt"), 1) ;
      CHECK_EQ(ds.getIndex("dbl"), 2) ;

      THEN ("merging a dataset with the same fieldnames work")
      {
        const auto orig_cout = ds.size() ;
        auto ds1 = db.select("SELECT 1 as int, 'eins' as txt, 2.2 as dbl;");
        CHECK_NOTHROW(ds.merge(ds1)) ;
        CHECK (ds.size() == orig_cout+1);
      }

      AND_WHEN ("merging a dataset with different fieldnames throws")
      {
        auto ds1 = db.select("SELECT 1 as foo, 'eins' as bar, 2.2 as bas;");
        THEN ("merging throws")
        {
          CHECK_THROWS_AS(ds.merge(ds1), ErrTypeMisMatch) ;
        }
      }

    }

  }
}




SCENARIO("sorting a dataset")
{
  using namespace sl3 ;
  GIVEN ("a database, a table and known data")
  {

    Database db{ ":memory:" };
    db.execute (
        "CREATE TABLE t (int INTEGER,txt TEXT, dbl real );"
        "INSERT INTO t VALUES (1,  1.1, 'b') ;"
        "INSERT INTO t VALUES (2,  0.11, 'c') ;"
        "INSERT INTO t VALUES (1,   0.2, 'a') ;"
    );


    WHEN ("having dataset with known order")
    {
      Dataset ds = db.select("SELECT * FROM t;");
      CHECK_EQ(ds[0][2].getText(), "b") ;
      CHECK_EQ(ds[1][2].getText(), "c") ;
      CHECK_EQ(ds[2][2].getText(), "a") ;

      THEN ("sorting with one index sorts as expected")
      {
        ds.sort({0}) ;
        CHECK_EQ(ds[0][2].getText(), "b") ;
        CHECK_EQ(ds[1][2].getText(), "a") ;
        CHECK_EQ(ds[2][2].getText(), "c") ;
      }
      THEN ("sorting with 2 indexes sorts as expected")
      {
        ds.sort({0, 1}) ;
        CHECK_EQ(ds[0][2].getText(), "a") ;
        CHECK_EQ(ds[1][2].getText(), "b") ;
        CHECK_EQ(ds[2][2].getText(), "c") ;
      }

    }
  }
}




SCENARIO("doing some things via dbvalues on rows of datasets")
{
   using namespace sl3 ;
   GIVEN ("a database and a typed dataset with knowen content")
   {
     Database db{ ":memory:" };
     Types types = {Type::Int,Type::Text, Type::Real} ;
     Dataset ds = db.select("SELECT 1,'hello', 1.1 as dbl;", types);
     WHEN ("having a dataset with different field count")
     {
       auto ds1 = db.select("SELECT 1,'hello';",
                                          {Type::Int, Type::Text});
       THEN ("swapping rows will work !")
       { // this might be wanted or not
         CHECK_NOTHROW(ds.at(0).swap(ds1.at(0)));
         CHECK_NOTHROW(std::swap(ds.at(0),ds1.at(0)));
         CHECK_NOTHROW(sl3::swap(ds.at(0),ds1.at(0)));
       }
     }

     WHEN ("having a dataset with different field count")
     {
       auto ds1 = db.select("SELECT 1,'hello';",
                                         {Type::Int, Type::Text});
       THEN ("assign a different rows will not work !")
       {
         CHECK_THROWS_AS(ds.at(0) = ds1.at(0), ErrTypeMisMatch);
         CHECK_THROWS_AS(ds.at(0) = std::move(ds1.at(0)), ErrTypeMisMatch);
       }
     }

     WHEN ("having a dataset with same field count but incampatible types")
     {
       auto ds1 = db.select("SELECT 1,'hello', 'hello';",
                                          {Type::Int, Type::Text, Type::Text});

       THEN ("assign a row will not work !")
       {
         CHECK_THROWS_AS(ds.at(0) = ds1.at(0), ErrTypeMisMatch);
         CHECK_THROWS_AS(ds.at(0) = std::move(ds1.at(0)), ErrTypeMisMatch);
       }

     }

     WHEN ("having an ohter , compatible dataset")
     {
       auto ds1 = db.select("SELECT 2,'world', 3.3;",
                                          {Type::Int, Type::Text, Type::Real});
       auto ds2 = db.select("SELECT 3,'world', 3.3;",
                                          {Type::Int, Type::Text, Type::Real});
       THEN ("assign a row will work !")
       {
         CHECK_NOTHROW(ds.at(0) = ds1.at(0));
         CHECK (ds[0][0].getInt() == 2) ;
         CHECK_NOTHROW(ds.at(0) = std::move(ds2.at(0)));
         CHECK (ds[0][0].getInt() == 3) ;
       }

     }
   }
}



