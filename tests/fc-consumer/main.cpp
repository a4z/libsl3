#include <sl3/database.hpp>
#include <sl3/dataset.hpp>
#include <sl3/types.hpp>

#include <cstdlib>
#include <iostream>

int
main ()
{
  sl3::Database db (":memory:");
  db.execute ("CREATE TABLE tbl(f1 INTEGER, f2 TEXT);"
              "INSERT INTO tbl (f1, f2) VALUES (1, 'one');"
              "INSERT INTO tbl (f1, f2) VALUES (2, 'two');");

  auto ds = db.select ("SELECT f1, f2 FROM tbl;");
  if (ds.size () != 2)
    {
      std::cerr << "Expect 2 records, got " << ds.size () << "\n";
      return EXIT_FAILURE;
    }

  if (ds.at (0).at (0).type () != sl3::Type::Int)
    {
      std::cerr << "Expect Int in first column\n";
      return EXIT_FAILURE;
    }

  std::cout << "fc-consumer smoke ok\n";
  return EXIT_SUCCESS;
}
