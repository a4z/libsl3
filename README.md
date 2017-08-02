libsl3, the convenient C++11 interface for SQLite 3.x

libsl3 is designed to enable a comfortable and efficient communication with
SQLite 3.x database based based on its natural language, which is SQL.

A short usage example:

```c
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

```
this will output as expected

```
1 one 1.1 
2 two 2.1 
```


Build requirements:
* CMake, libsl3 builds uses cmake
* doxygen, optional, for building the dcumentation

libsl3 inlcudes sqlite, but it can also build and link with an existing sqlite
installation.   
This is controlled via cmake and the `USE_INTERNAL_SQLITE3` varialbe, 
which is by default true.

The CMake build contains a target for generating doxygen documentation.   
Some samples can be found in the test subfolder.

Tested with:
* travis-ci: gcc (Ubuntu 4.8.4-2ubuntu1~14.04.3) 4.8.4, clang version 3.5.0
[![Build Status](https://travis-ci.org/a4z/libsl3.svg?branch=master)](https://travis-ci.org/a4z/libsl3)
* various newer gcc and clang versions on differnt Linux distributions (RHEL, SUSE, Slackware)
* Visual Studio 2015 [![Build status](https://ci.appveyor.com/api/projects/status/vsk807oexd8592sa?svg=true)](https://ci.appveyor.com/project/a4z/libsl3)


The documentation can be found here:
http://a4z.bitbucket.io/docs/libsl3/html/index.html

License: https://www.mozilla.org/en-US/MPL/2.0/
