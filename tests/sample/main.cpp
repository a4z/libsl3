#include <cassert>
#include <iostream>

#include <sl3/columns.hpp>
#include <sl3/database.hpp>
int
main ()
{
  using namespace sl3;
  // define a db
  Database db (":memory:");
  // run commands against the db

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
