# libsl3, a C++ interface for SQLite 3.x

libsl3 is designed to enable comfortable and efficient communication with a
SQLite database based on its natural language, which is SQL.

libsl3 originated back at a time when C++11 was new.
It has remained stable to support existing users for quite a while, over a decade.
With the arrival of C++23, development has restarted, but due to a lack of time, not much has happened so far. The minimum required C++ standard is now C++17.
The goal is to keep the interface stable, but using a newer C++ standard might justify some breaking changes.

For people seeking the old version, release v1.1.31001 preserves the original C++11 state with CMake 2.8 support.

**The full documentation for libsl3 can be found here:** <br>
https://a4z.github.io/libsl3/

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
  // A row in a dataset is also a container
  auto row = ds[0] ;
  assert(row.size()==3);
  // Type info for each field s available
  assert ( row[0].type() == Type::Int ) ;
  assert ( row[1].type() == Type::Text ) ;
  assert ( row[2].type() == Type::Real ) ;
  // there is also iterator access
  for(const auto& row  :ds) {
      for (const auto& field : row) {
          std::cout << field << " " ;
      }
      std::cout << std::endl;
  }
}

```

This will output

```bash
1 one 1.1
2 two 2.1
```

Additional samples can be found in the tests and tests/samples subfolder.

## License

https://www.mozilla.org/en-US/MPL/2.0/
