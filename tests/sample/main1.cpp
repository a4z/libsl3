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

  //add some data
  cmd.execute({ {1}, {"one"}, {1.1} } );
  cmd.execute({ {2}, {"two"}, {2.1} } );

  // access the data
  Dataset ds = db.select("SELECT * FROM tbl;");

  // Dataset is a container
  assert(ds.size()==2);

  // Dataset row is a container
  auto row = ds[0] ;
  assert(row.size()==3);
  assert ( row[0].getStorageType() == Type::Int ) ;
  assert ( row[1].getStorageType() == Type::Text ) ;
  assert ( row[2].getStorageType() == Type::Real ) ;

  // of course there is also iterator access
  for(auto&& row  :ds) {

      for (auto&& field : row) {
          std::cout << field << " " ;
      }
      std::cout << std::endl;
  }
}
