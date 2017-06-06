#include <cassert>
#include <iostream>
#include <sl3/database.hpp>
int main()
{
  using namespace sl3;
  DbValue val (Type::Variant);
  assert (val.isNull ()) ;
  val = 2 ;
  std::cout << val << std::endl; // access the integer property
  try
  {   // not possible since this variant holds an integer value
      val.getText() ;
  }
  catch (const Error& e)
  {
      std::cout << e << std::endl;
  }

  val = "example";
  // now it is possible to access the text property
  std::cout << val.getText() << std::endl;
}
