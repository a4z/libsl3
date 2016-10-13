
#include <a4testing.hpp>

#include <sl3/value.hpp>
#include <sl3/error.hpp>
#include <sstream>
#include <string>
#include <cmath>

namespace sl3
{
  namespace
  {

    void regular ()
    {
   
// regular a == b, b != a 
      
  
    }


 

    a4TestAdd (a4test::suite ("intval")
          .addTest ("regular", regular)
          .addTest ("equal", compareEqual)
          .addTest ("less", compareLess)
          .addTest ("totalOrder", strictTotalOrdered)

                   );
  }
}

