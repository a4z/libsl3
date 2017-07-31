#include <cassert>
#include <iostream>

#include <sl3/database.hpp>

int main()
{
  using namespace sl3;
   // define a db
   Database db(":memory:");
   // run commands against the db
   db.execute("CREATE TABLE tbl(f1 INTEGER, f2 TEXT, f3 REAL);");
   // create a command with parameters
   auto cmd = db.prepare("INSERT INTO tbl (f1, f2, f3) VALUES (?,?,?);");
   // no types so we use variants
   //so this will work
   cmd.execute (parameters (1, "one", 1.1));
   //and this will also work
   cmd.execute (parameters ("some text", "two", 2.1));

   // access the data
   Dataset ds = db.select("SELECT * FROM tbl;");

   assert (ds.size () == 2);       // 2 records
   assert (ds[0].size() == 3);  // 3 fields

   // first row first field is a integer, as inserted
   assert (ds[0][0].dbtype() == Type::Variant) ;
   assert (ds[0][0].type() == Type::Int) ;
   // second row first field is text, as inserted
   assert (ds[1][0].dbtype() == Type::Variant) ;
   assert (ds[1][0].type() == Type::Text) ;

   // of course we can work wit the values
   for(auto&& row  :ds) {
       for (auto&& field : row) {
           std::cout << typeName (field.dbtype()) << "/"
               << typeName (field.type()) << ": " << field << ", ";
       }
       std::cout << std::endl;
   }
}

