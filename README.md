# libsl3, the convenient interface for SQLite 3.x

libsl3 is designed to enable a comfortable and efficient communication with
SQLite 3.x database based on its natural language, which is SQL.

libsl3 origins back to a time when C++11 was new.
It has been stable to support an existing user for quite a while, over a decade.
With C++23 landing, development has been restarted.
The goal is to keep the interface stable, but using newer C++ standard might justify some breaking changes.

There is a branch preserving the original C++11 state for people looking for the old interface.

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

Additional samples can be found in the tests and tests/samples subfolder.

## Build requirements

* CMake
* doxygen, optional, for building the documentation

libsl3 requires, no surprise, sqlite3, and uses doctest for unit testing.
sqlite3 is included and will be build in the library itself, if this is unwanted, use `-DUSE_INTERNAL_SQLITE3=FALSE` when doing the CMake configuration.


## Additional information

The online documentation for libsl3 can be found here:
https://a4z.github.io/libsl3/

License: https://www.mozilla.org/en-US/MPL/2.0/
