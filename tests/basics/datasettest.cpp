#include <sl3/dataset.hpp>

#include <a4testing.hpp>
#include <sl3/database.hpp>
#include <sl3/error.hpp>

#include "testdb.hpp"

namespace sl3 {
namespace {


Database
testdb ()
{

  TestDb db{ ":memory:" };
  db.execute (
      "CREATE TABLE tbltest (intFld INTEGER,txtFld TEXT, dblFld real );"
      "INSERT INTO tbltest VALUES (1, 'eins', 1.111) ;"
      "INSERT INTO tbltest VALUES (2, 'zwei', 2.22) ;"
      "INSERT INTO tbltest VALUES (3, NULL, NULL) ;"
  );

  return std::move(db);
}//-----------------------------------------------------------------------------

void
BasicValidation ()
{
  BOOST_REQUIRE_NO_THROW(testdb ());

  auto db = testdb ();

  Dataset ds;

  BOOST_REQUIRE_NO_THROW(ds = db.select ("SELECT * FROM tbltest;"));
  BOOST_CHECK(ds.begin () != ds.end ()); // must be  on the first position
  BOOST_CHECK(ds.size () == 3);
  BOOST_CHECK(ds.at (0).size () == 3);

  ds.reset ();

  BOOST_CHECK(ds.begin () == ds.end ()); // must be empty
  BOOST_CHECK(ds.size () == 0);

}//-----------------------------------------------------------------------------


void
FieldTypes()
{
  BOOST_REQUIRE_NO_THROW(testdb ());
  auto db = testdb ();

  Dataset ds;
  Dataset ds1;
  Dataset ds2;

  BOOST_REQUIRE_NO_THROW(ds = db.select ("SELECT * FROM tbltest;",
                                         {Type::Int, Type::Text, Type::Real}));
  BOOST_REQUIRE_THROW(ds1 = db.select ("SELECT * FROM tbltest;" ,
                                       {Type::Real, Type::Text, Type::Int}),
                      ErrTypeMisMatch);

  BOOST_REQUIRE_NO_THROW(ds2 = db.select ("SELECT * FROM tbltest;",
                                  {Type::Variant, Type::Text, Type::Variant}));

  BOOST_REQUIRE(ds.size() > 0 );

  BOOST_REQUIRE_NO_THROW(ds.reset());
  BOOST_REQUIRE(ds.size() == 0 );

  BOOST_REQUIRE_THROW(ds = db.select ("SELECT * FROM tbltest;",
                                      {Type::Int,
                                       Type::Text ,
                                       Type::Real,
                                       Type::Variant}),
          ErrTypeMisMatch);

}//-----------------------------------------------------------------------------


void
CopyMove()
{
  BOOST_REQUIRE_NO_THROW(testdb ());
  auto db = testdb ();

  Dataset ds({Type::Int, Type::Text, Type::Real});

  BOOST_REQUIRE_NO_THROW(ds = db.select ("SELECT * FROM tbltest;"));

  BOOST_REQUIRE(ds.size() > 0 );

  Dataset ds1  ;

  BOOST_REQUIRE_NO_THROW(ds1 = ds);

  BOOST_REQUIRE(ds1.size() == ds.size());

  Dataset ds2 { std::move(ds1) };

  BOOST_REQUIRE(ds2.size() == ds.size());

  Dataset ds3({Type::Int}) ;
  // some type to hae the nowthrow now
  BOOST_REQUIRE_NO_THROW(ds3 = std::move(ds2));

  BOOST_REQUIRE(ds3.size() == ds.size());

  Dataset ds4(ds3) ;
  BOOST_REQUIRE(ds4.size() == ds.size());
// check if assignemnt op is a good idea
//  ds4 should be the same as ds

}//-----------------------------------------------------------------------------

void
Merge()
{

  BOOST_REQUIRE_NO_THROW(testdb ());
  auto db = testdb ();
// variant variant variant
  auto ds = db.select ("SELECT * FROM tbltest;");

  Dataset ds1 = db.select ("SELECT 11 as A;", {Type::Int});
  Dataset ds1a = db.select ("SELECT 'foo' as A;", {Type::Text});
  Dataset ds1aa({ Type::Int, Type::Int} );


  //db.execute ("SELECT 'foo' AS a ,11 AS b;", ds1aa);

  BOOST_REQUIRE_THROW(ds1.merge(ds), ErrTypeMisMatch);
  BOOST_REQUIRE_THROW(ds.merge(ds1), ErrTypeMisMatch);
  BOOST_REQUIRE_THROW(ds1.merge(ds1a), ErrTypeMisMatch);
  BOOST_REQUIRE_THROW(ds1a.merge(ds1aa), ErrTypeMisMatch);

  Dataset ds2({Type::Variant});
  BOOST_REQUIRE_NO_THROW(ds2.merge(ds1));
  BOOST_REQUIRE_THROW(ds2.merge(ds), ErrTypeMisMatch);

  Dataset ds3({Type::Variant, Type::Text, Type::Variant});
  BOOST_REQUIRE_NO_THROW(
      ds3.merge(DbValues {DbValue {23}, DbValue {"foo"}, DbValue {2.2}}));
  // variant text variant with variant variant variant
  BOOST_REQUIRE_THROW(ds3.merge(ds), ErrTypeMisMatch);
  BOOST_REQUIRE_NO_THROW(ds.merge(ds3));


  BOOST_REQUIRE_THROW (ds1.merge (DbValues {DbValue {23}, DbValue {"foo"}}),
                      ErrTypeMisMatch);


  BOOST_REQUIRE_THROW(ds1.merge(DbValues { DbValue {"foo"} }),
                      ErrTypeMisMatch);

  auto oldsize = ds2.size ();
  BOOST_REQUIRE_NO_THROW(ds2.merge(DbValues { DbValue {"foo"} }));

  BOOST_REQUIRE(oldsize + 1 == ds2.size ()) ;


  Dataset dsi = db.select ("SELECT 11 as A;", {Type::Int});
  Dataset dsv = db.select ("SELECT 'foo' as A;", {Type::Variant});


  BOOST_REQUIRE_THROW(dsi.merge(dsv), ErrTypeMisMatch);

  BOOST_REQUIRE_NO_THROW (dsi.reset ({Type::Variant}));
  BOOST_REQUIRE (dsi.size () == 0) ;
  BOOST_REQUIRE_NO_THROW (dsi.merge(dsv));


}//-----------------------------------------------------------------------------

void
Index()
{

  BOOST_REQUIRE_NO_THROW(testdb ());
  auto db = testdb ();

  auto ds = db.select ("SELECT * FROM tbltest;");

  BOOST_REQUIRE( ds.getIndex("intFld") == 0 );
  BOOST_REQUIRE( ds.getIndex("dblFld") == 2 );
  BOOST_REQUIRE_THROW(ds.getIndex("foo"), ErrOutOfRange);

  BOOST_REQUIRE_NO_THROW(ds.at(2));
  BOOST_REQUIRE_THROW(ds.at(3), ErrOutOfRange);


  auto ds1 = ds ;
  BOOST_REQUIRE( ds1.getIndex("intFld") == 0 );
  BOOST_REQUIRE( ds1.getIndex("dblFld") == 2 );
  BOOST_REQUIRE_THROW(ds1.getIndex("foo"), ErrOutOfRange);


  auto ds2 = std::move (ds1) ;
  BOOST_REQUIRE( ds2.getIndex("intFld") == 0 );
  BOOST_REQUIRE( ds2.getIndex("dblFld") == 2 );
  BOOST_REQUIRE_THROW(ds2.getIndex("foo"), ErrOutOfRange);


}//-----------------------------------------------------------------------------


void DbSelect ()
{
  BOOST_REQUIRE_NO_THROW(testdb ());
  auto db = testdb ();

  Types oktypes {Type::Int,
      Type::Text, Type::Real};

  BOOST_REQUIRE_NO_THROW(
        db.select ("SELECT * FROM tbltest;", oktypes)
  );

  Types falsetypes {Type::Int,
      Type::Int, Type::Int};

  BOOST_REQUIRE_THROW(
         db.select ("SELECT * FROM tbltest;", falsetypes),
         ErrTypeMisMatch
  );


}//-----------------------------------------------------------------------------

a4TestAdd(
    a4test::suite ("dataset")
    .addTest ("db selcet ds", BasicValidation)
    .addTest ("ds typed", FieldTypes)
    .addTest ("copy and move", CopyMove)
    .addTest ("merge", Merge)
    .addTest ("index", Index)
    .addTest ("dbselect", DbSelect)
);

}
}


