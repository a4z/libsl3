
#include <a4testing.hpp>

#include <sl3/database.hpp>
#include <sl3/dbvalue.hpp>
#include <sl3/dbvalues.hpp>
#include <sl3/error.hpp>

#include <algorithm>
#include <iostream>
#include <vector>

using namespace sl3;

namespace sl3
{
  namespace
  {
    void
    stdSort ()
    {
      using namespace std;
      vector<DbValue> vals{
          DbValue (2.2), DbValue ("foo"), DbValue (Blob{}), DbValue (1)};

      sort (begin (vals), end (vals));
      // for (const auto& val : vals) cout << val << endl ;

      BOOST_CHECK_NO_THROW (BOOST_CHECK (vals.at (0) == 1);
                            BOOST_CHECK (vals.at (1) == 2.2);
                            BOOST_CHECK (vals.at (2) == "foo");
                            BOOST_CHECK (vals.at (3) == Blob{}));
    }

    Database
    testdb ()
    {
      Database db{":memory:"};
      db.execute (
          "CREATE TABLE tbltest (intFld INTEGER,txtFld TEXT, dblFld real );"
          "INSERT INTO tbltest VALUES (2, 'aa', NULL) ;"
          "INSERT INTO tbltest VALUES (1, 'bb', 2.22) ;"
          "INSERT INTO tbltest VALUES (1, 'bb', 1.11) ;"
          "INSERT INTO tbltest VALUES (NULL, 'bb', 2.22) ;"
          "INSERT INTO tbltest VALUES (1, 'bb', 2.22) ;"
          "INSERT INTO tbltest VALUES (3, NULL, NULL) ;");

      return db;
    }

    void
    sortDS (Dataset& ds, const std::vector<size_t>& idxs)
    {
      auto lessValues = [&idxs](const DbValues& a, const DbValues& b) -> bool {
        for (auto cur : idxs)
          {
            if (a.at (cur) < b.at (cur))
              return true;
            else if (b.at (cur) < a.at (cur))
              return false;
          }
        return false;
      };

      using namespace std;

      sort (begin (ds), end (ds), lessValues);
    }

    bool
    sameDS (const Dataset& a, const Dataset& b)
    {
      if (a.size () != b.size ())
        return false;

      for (size_t i = 0; i < a.size (); ++i)
        {
          if (a[i].size () != b[i].size ())
            return false;

          for (size_t j = 0; j < a[i].size (); ++j)
            {
              if (a[i][j] != b[i][j])
                return false;
            }
        }
      return true;
    }

    void
    sortRecord ()
    {
      auto db = testdb ();
      auto ds = db.select ("SELECT * FROM tbltest;");

      sortDS (ds, {0});
      auto checkSQL = "SELECT * FROM tbltest ORDER BY intFld;";
      BOOST_CHECK (sameDS (ds, db.select (checkSQL)));

      sortDS (ds, {1, 2});
      checkSQL = "SELECT * FROM tbltest ORDER BY txtFld, dblFld";
      BOOST_CHECK (sameDS (ds, db.select (checkSQL)));
    }

    a4TestAdd (a4test::suite ("sorting")
                   .addTest ("stdsort", stdSort)
                   .addTest ("sortRecord", sortRecord));
  }
}
