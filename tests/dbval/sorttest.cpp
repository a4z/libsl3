
#include <a4testing.hpp>

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





    a4TestAdd (a4test::suite ("sorting").addTest ("stdsort", stdSort));
  }
}
