#include <cassert>
#include <iostream>

#include <sl3/columns.hpp>
#include <sl3/database.hpp>

/*
  this is the only example that currently works with memory sanitizer
  I keep that here, not sure about how to adjust the other examples

  also, the problem with std::endl seems odd, maybe an alpine arm issue
*/


int
main ()
{
  using namespace sl3;
  // for memory sanitizer, this is needed at least on alpine arm
  std::cout << std::endl ;
  std::cout.flush ();

  Database db (":memory:");


  db.execute ("CREATE TABLE tbl(f1 INTEGER, f2 TEXT, f3 REAL);"
              "INSERT INTO tbl (f1, f2, f3) VALUES (1, 'one', 1.1);"
              "INSERT INTO tbl (f1, f2, f3) VALUES (2, 'two', 2.2)");

  db.execute ("SELECT f1, f2  FROM tbl;", [] (Columns cols) {
    assert (cols.count () == 2);
    assert (cols.getType (0) == Type::Int);
    assert (cols.getType (1) == Type::Text);

    std::cout << std::to_string (cols.getInt (0)) << "_" << cols.getText (1)
              << std::endl;
    return true;
  });


}
