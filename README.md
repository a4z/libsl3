# libsl3, the convenient C++11 interface for SQLite 3.x

libsl3 is designed to enable a comfortable and efficient communication with
SQLite 3.x database based on its natural language, which is SQL.

## A short usage example

```cpp
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
  cmd.execute(parameters(1, "one", 1.1));
  cmd.execute(parameters(2, "two", 2.2));

  // access the data
  Dataset ds = db.select("SELECT * FROM tbl;");

  // Dataset is a container
  assert(ds.size()==2);

  // A row in a dataset is a container
  auto row = ds[0] ;
  assert(row.size()==3);
  assert ( row[0].type() == Type::Int ) ;
  assert ( row[1].type() == Type::Text ) ;
  assert ( row[2].type() == Type::Real ) ;

  // of course there is also iterator access
  for(const auto& row  :ds) {

      for (const auto& field : row) {
          std::cout << field << " " ;
      }
      std::cout << std::endl;
  }
}

```

This will output as expected

```bash
1 one 1.1
2 two 2.1
```

Additional samples can be found in the tests and tests/sampes subfolder.

## Build requirements

* CMake
* doxygen, optional, for building the documentation
* Conan

libsl3 requires, no surprise, sqlite3, and uses doctest for unit testing.
The requirements will be handled by CMake via Conan, therefore, Conan needs to be installed. https://docs.conan.io/

## Addtitional information

The online documentation for libsl3 can be found here:
http://a4z.bitbucket.io/docs/libsl3/html/index.html

License: https://www.mozilla.org/en-US/MPL/2.0/
