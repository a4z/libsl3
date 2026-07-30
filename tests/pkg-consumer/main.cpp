#include <sl3/columns.hpp>
#include <sl3/database.hpp>
#include <sl3/error.hpp>
#include <sl3/types.hpp>

#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>

int
main ()
{
  std::ostringstream typeStream;
  typeStream << sl3::Type::Text;
  if (typeStream.str () != "Type::Text")
    {
      std::cerr << "Expect Type::Text, got " << typeStream.str () << "\n";
      return EXIT_FAILURE;
    }

  const std::string compiledVersion = sl3::sqliteCompiledVersion ();
  const std::string runtimeVersion  = sl3::sqliteRuntimeVersion ();
  const int         compiledNumber  = sl3::sqliteCompiledVersionNumber ();
  const int         runtimeNumber   = sl3::sqliteRuntimeVersionNumber ();

  std::cout << "sl3 sqlite compiled: " << compiledVersion << " ("
            << compiledNumber << ")\n";
  std::cout << "sl3 sqlite runtime : " << runtimeVersion << " ("
            << runtimeNumber << ")\n";

  if (compiledVersion.empty () || runtimeVersion.empty ())
    {
      std::cerr << "sqlite version strings must not be empty\n";
      return EXIT_FAILURE;
    }

  if (compiledNumber <= 0 || runtimeNumber <= 0)
    {
      std::cerr << "sqlite version numbers must be positive\n";
      return EXIT_FAILURE;
    }

  if (compiledNumber != runtimeNumber)
    {
      std::cerr << "Expect compiledNumber != runtimeNumber. ";
      std::cerr << "Got" << compiledNumber << " != " << runtimeNumber << "\n";
      return EXIT_FAILURE;
    }

  sl3::Database db (":memory:");
  // run commands against the db

  db.execute ("CREATE TABLE tbl(f1 INTEGER, f2 TEXT, f3 REAL);"
              "INSERT INTO tbl (f1, f2, f3) VALUES (1, 'one', 1.1);"
              "INSERT INTO tbl (f1, f2, f3) VALUES (2, 'two', 2.2)");

  auto ds = db.select ("SELECT f1, f2  FROM tbl;");
  if (ds.size () != 2)
    {
      std::cerr << "Expect 2 records, ";
      std::cerr << "got" << ds.size () << "\n";
      return EXIT_FAILURE;
    }

  if (ds.at (0).size () != 2)
    {
      std::cerr << "Expect 2 columns, ";
      std::cerr << "got" << ds.at (0).size () << "\n";
      return EXIT_FAILURE;
      ;
    }

  auto t0 = ds.at (0).at (0).type ();
  if (t0 != sl3::Type::Int)
    {
      std::cerr << "Expect Int, got " << sl3::typeName (t0) << "\n";
      return EXIT_FAILURE;
    }

  auto t1 = ds.at (0).at (1).type ();
  if (t1 != sl3::Type::Text)
    {
      std::cerr << "Expect Text, got " << sl3::typeName (t0) << "\n";
      return EXIT_FAILURE;
    }

  auto moved = std::move (db);
  try
    {
      (void)db.prepare ("SELECT 1");
      std::cerr << "Expect ErrNoConnection from moved-from database\n";
      return EXIT_FAILURE;
    }
  catch (const sl3::ErrNoConnection& e)
    {
      std::ostringstream errorStream;
      errorStream << e;
      if (errorStream.str ().find ("sl3::NoConnection:") == std::string::npos)
        {
          std::cerr << "Unexpected ErrNoConnection text: "
                    << errorStream.str () << "\n";
          return EXIT_FAILURE;
        }
    }

  (void)moved;

  return EXIT_SUCCESS;
}
