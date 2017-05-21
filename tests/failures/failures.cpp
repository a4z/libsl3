#include <sl3/dbvalues.hpp>

#include <a4testing.hpp>

#include <sl3/error.hpp>


using namespace sl3;

namespace sl3 {
namespace {
  
void failure1()
{
  double d1 = 0.2;
  double d2 = 1 / std::sqrt(5) / std::sqrt(5);
  BOOST_CHECK (d1 != d2);
  auto realval = DbValue{d1};
  BOOST_CHECK (realval == d2);
  BOOST_CHECK (realval == DbValue(d2));
}


a4TestAdd(
  a4test::suite("failures")
  .addTest("failure1"  , failure1)

) ;

}
}
