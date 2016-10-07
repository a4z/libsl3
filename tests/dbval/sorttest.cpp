
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
    //  vector<DbValue> vals{
    //      DbValue (2.2), DbValue ("foo"), DbValue (Blob{}), DbValue (1)};

  vector<DbValue> vals{
          DbValue (2.2), DbValue ("foo"), DbValue(Blob{}) , DbValue (1)};



      sort (begin (vals), end (vals));
      // for (const auto& val : vals) cout << val << endl ;

      BOOST_CHECK_NO_THROW (BOOST_CHECK (vals.at (0) == 1);
                            BOOST_CHECK (vals.at (1) == 2.2);
                            BOOST_CHECK (vals.at (2) == "foo");
                            BOOST_CHECK (vals.at (3) == Blob{}));
    }


    void
    variantSort ()
    {
      using namespace std;

      vector<DbValue> vals{  DbValue (2.2, Type::Variant), 
        DbValue ("foo", Type::Variant), 
        DbValue(Blob{}, Type::Variant) , 
        DbValue (1, Type::Variant)};



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


    //void printDS(const Dataset& ds)
    //{

      //std::cout << ".................." << std::endl ;
      //for (const auto& row: ds)
      //{
        //for (const auto& field : row)
          //std::cout << field << " " ;

        //std::cout << std::endl ;
      //}

      //std::cout << ".................." << std::endl ;
    //}

    void
    sortRecord ()
    {
      auto db = testdb ();
      auto ds = db.select ("SELECT * FROM tbltest;");

      ds.sort ({0});
      auto checkSQL = "SELECT * FROM tbltest ORDER BY intFld;";
      BOOST_CHECK (sameDS (ds, db.select (checkSQL)));

      ds.sort( {1, 2, 0});
      checkSQL = "SELECT * FROM tbltest ORDER BY txtFld, dblFld, intFld";
      
      auto dbsort = db.select (checkSQL);
      //printDS (ds);
      //printDS(dbsort) ;

      BOOST_CHECK (sameDS (ds, dbsort));
    }

    a4TestAdd (a4test::suite ("sorting")
                   .addTest ("stdsort", stdSort)
                   .addTest ("variantSort", variantSort)
                   .addTest ("sortRecord", sortRecord));
  }
}
