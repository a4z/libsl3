

#include <a4testing.hpp>

#include "testdb.hpp"

#include <iostream>
#include <sl3/command.hpp>
#include <sl3/database.hpp>
#include <sl3/dataset.hpp>
#include <sl3/error.hpp>

namespace sl3 {
namespace  {


TestDb
testdb ()
{

  TestDb db { ":memory:" };
  db.execute ( "CREATE TABLE t_typeplay "
      "(intFld INTEGER , dblFld REAL, txtFld TEXT, blobFld NONE );");

  db.createStoredCommand (
      "typed", "insert into t_typeplay (intFld,  dblFld , txtFld ,  blobFld)"
      " VALUES (? , ? , ? , ?  ); ",
        { Type::Int, Type::Real, Type::Text, Type::Blob });

  db.createStoredCommand (
      "variant", "insert into t_typeplay (intFld,  dblFld , txtFld ,  blobFld)"
      " VALUES (? , ? , ? , ?  ); ");

  db.createStoredCommand ("selectall", "SELECT * FROM t_typeplay;");

  db.createStoredCommand ("clearall", "DELETE   FROM  t_typeplay;");

  return db;
}




void
RunTypedCommand ()
{
  auto db = testdb ();
  BOOST_REQUIRE_NO_THROW(db.getCommand ("typed"));

  Command& typed = db.getCommand ("typed");

  BOOST_REQUIRE_NO_THROW(typed.getParameters ().at (0).set (99));
  BOOST_REQUIRE_NO_THROW(typed.getParameters ().at (1).set (8.8));
  BOOST_REQUIRE_NO_THROW(typed.getParameters ().at (2).set ("testcase"));
  Blob blob ({'a', 'b' });
  BOOST_REQUIRE_NO_THROW(typed.getParameters ().at (3).set (blob));

  Dataset ds;
  Types types ({Type::Int, Type::Real, Type::Text, Type::Blob });
  BOOST_REQUIRE_NO_THROW(ds = db.getCommand ("selectall").select (types));
  BOOST_REQUIRE(ds.size() == 0);

  BOOST_REQUIRE_NO_THROW(typed.execute ());

  BOOST_REQUIRE_NO_THROW(ds = db.getCommand ("selectall").select (types));

  BOOST_REQUIRE(ds.size() == 1);
  BOOST_REQUIRE_EQUAL(ds[0].size (), 4);

  BOOST_REQUIRE(ds[0].at(0).getStorageType()==Type::Int );
  BOOST_REQUIRE_EQUAL(ds[0].at(0).getInt() ,
                      typed.getParameters().at(0).getInt() );

  BOOST_REQUIRE(ds[0].at(1).getStorageType()==Type::Real );
  BOOST_REQUIRE_EQUAL(ds[0].at(1).getReal() ,
                      typed.getParameters().at(1).getReal() );

  BOOST_REQUIRE(ds[0].at(2).getStorageType()==Type::Text );
  BOOST_REQUIRE_EQUAL(ds[0].at(2).getText() ,
                      typed.getParameters().at(2).getText() );

  BOOST_REQUIRE(ds[0].at(3).getStorageType()==Type::Blob );
  BOOST_REQUIRE(ds[0].at(3).getBlob() ==
      typed.getParameters().at(3).getBlob());

  for (const auto& row : ds)
    {
      for (const auto& field : row)
        BOOST_REQUIRE (field.getType () == field.getStorageType ());
    }

  for (const auto& param : typed.getParameters ())
      BOOST_REQUIRE (param.getType () == param.getStorageType ());


} //----------------------------------------------------------------------------

void
RunVariantCommand ()
{
  auto db = testdb ();

  BOOST_REQUIRE_NO_THROW(db.getCommand ("variant"));
  Command& variant = db.getCommand ("variant");

  BOOST_REQUIRE_NO_THROW(variant.getParameters ().at (0).set (99));
  BOOST_REQUIRE_NO_THROW(variant.getParameters ().at (1).set (8.8));
  BOOST_REQUIRE_NO_THROW(variant.getParameters ().at (2).set ("testcase"));

  Blob blob ({'a', 'b'});
  BOOST_REQUIRE_NO_THROW(variant.getParameters ().at (3).set (blob));

  BOOST_REQUIRE_NO_THROW(variant.select ());

  Dataset ds = db.getCommand ("selectall").select ();

  for (const auto& row : ds)
    {
      for (const auto& field : row)
        BOOST_REQUIRE (field.getType () == Type::Variant);
    }

  BOOST_REQUIRE(ds.size() == 1);
  BOOST_REQUIRE_EQUAL(ds[0].size (), 4);

  BOOST_REQUIRE(ds[0].at(0).getStorageType()==Type::Int );
  BOOST_REQUIRE_EQUAL(ds[0].at(0).getInt() ,
                      variant.getParameters().at(0).getInt() );

  BOOST_REQUIRE(ds[0].at(1).getStorageType()==Type::Real );
  BOOST_REQUIRE_EQUAL(ds[0].at(1).getReal() ,
                      variant.getParameters().at(1).getReal() );

  BOOST_REQUIRE(ds[0].at(2).getStorageType()==Type::Text );
  BOOST_REQUIRE_EQUAL(ds[0].at(2).getText() ,
                      variant.getParameters().at(2).getText() );

  BOOST_REQUIRE(ds[0].at(3).getStorageType()==Type::Blob );
  BOOST_REQUIRE(ds[0].at(3).getBlob()== variant.getParameter(3).getBlob());


  BOOST_REQUIRE_NO_THROW(variant.getParameter(0).set (99));
  BOOST_REQUIRE_NO_THROW(variant.getParameter(1).setNull ());
  BOOST_REQUIRE_NO_THROW(variant.getParameter(2).set ("testcase"));
  BOOST_REQUIRE_NO_THROW(variant.getParameter(3).setNull ());

  variant.select ();
  BOOST_REQUIRE_NO_THROW(ds.reset ());
  ds = db.getCommand ("selectall").select ();
  BOOST_REQUIRE(ds.size() == 2);
  auto last = ds.size () - 1;

  BOOST_REQUIRE(ds[last].at(0).getStorageType()==Type::Int );
  BOOST_REQUIRE_EQUAL(ds[last].at(0).getInt() ,
                      variant.getParameter(0).getInt() );

  BOOST_REQUIRE(ds[last].at(1).getStorageType()==Type::Null );
  BOOST_REQUIRE(ds[last].at(1).isNull() );

  BOOST_REQUIRE(ds[last].at(2).getStorageType()==Type::Text );
  BOOST_REQUIRE_EQUAL(ds[last].at(2).getText() ,
                      variant.getParameter(2).getText() );

  BOOST_REQUIRE(ds[last].at(3).getStorageType()==Type::Null );
  BOOST_REQUIRE(ds[last].at(3).isNull() );

} //--------------------------------------------------------------------------



void
TypeNames ()
{
  BOOST_CHECK_EQUAL (typeName (Type::Int),     "Int") ;
  BOOST_CHECK_EQUAL (typeName (Type::Real),    "Real") ;
  BOOST_CHECK_EQUAL (typeName (Type::Text),    "Text") ;
  BOOST_CHECK_EQUAL (typeName (Type::Blob),    "Blob") ;
  BOOST_CHECK_EQUAL (typeName (Type::Null),    "Null") ;
  BOOST_CHECK_EQUAL (typeName (Type::Variant), "Variant") ;
}


a4TestAdd(
  a4test::suite("typetest")
  .addTest("typed", RunTypedCommand)
  .addTest("variant", RunVariantCommand)
  .addTest("typenames", TypeNames)
) ;




}
}

