#include "../testing.hpp"

#include <sl3/database.hpp>
#include <sl3/error.hpp>

#include <string>

SCENARIO ("for coverage")
{
  GIVEN ("a record with known data and known types")
  {
    sl3::Database db{":memory:"};
    auto          sql = "SELECT 'b' as byte, NULL as noval; ";
    REQUIRE_NOTHROW (db.execute (sql));
    WHEN ("asking for the used storegae space")
    {
      THEN ("some int will use some space, and null non space")
      {
        db.execute (sql, [] (sl3::Columns cols) {
          CHECK (cols.getSize (0) == 1);
          CHECK (cols.getSize (1) == 0);
          return true;
        });
      }
    }
    WHEN ("geting an int from a null value")
    {
      THEN ("sqlite will convert it to 0")
      {
        db.execute (sql, [] (sl3::Columns cols) {
          CHECK (cols.getInt (1) == 0);
          return true;
        });
      }
    }

    WHEN ("want accecc to shoot myslef into the foot")
    {
      THEN ("this is possible")
      {
        db.execute (sql, [] (sl3::Columns cols) {
          CHECK (cols.get_stmt () != nullptr);
          return true;
        });
      }
    }
  }
}
