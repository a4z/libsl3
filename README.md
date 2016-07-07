libsl3, the convenient modern C++ interface for SQLite 3.x

libsl3 is designed to enable a comfortable and efficient communication with
SQLite 3.x database based based on its natural language, which is SQL.





A short usage example:
This is
```c
#include <cassert>doc
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
  auto cmd = db.command("INSERT INTO tbl (f1, f2, f3) VALUES (?,?,?);");
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
  // check some tpye info
  assert (row[0].getStorageType() == Type::Int) ;
  assert (row[1].getStorageType() == Type::Text) ;
  assert (row[2].getStorageType() == Type::Real) ;

  // of course there is also iterator access
  for(auto& row  :ds) {
      for (auto& field : row) {
          std::cout << field << " " ;
      }
      std::cout << std::endl;
  }
}

```
this will output as expected

```
1 one 1.1 
2 two 2.1 
```

   
Build requirements:
CMake, libsl3 builds uses cmake.   
Optional:   
required for building the tests:  boost,  www.boost.org.    
Building the test can be disabled by passing `-Dsl3_BUILD_TESTS=OFF` to cmake.  

libsl3 inlcudes sqlite, but it can also build and link with an existing sqlite
installation.   
This is controlled via the `USE_INTERNAL_SQLITE3` varialbe, 
which is by default true.

The CMake build contains a target for generating doxygen documentation.   
Some samples can be found in the test subfolder.  

Tested with:
gcc 4.8.2 and clang 3.3 on Slackware.    
mingw-gcc 4.9.2 and Visual Studio 2015 on Windows 10

The documentation can be found here:
http://a4z.bitbucket.org/docs/libsl3/html/index.html

License: https://www.mozilla.org/en-US/MPL/2.0/
