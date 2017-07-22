#include "../testing.hpp"
#include <sl3/value.hpp>

#include <vector>
#include <string>

using ValueList = std::vector<sl3::Value> ;

template <typename... VALS>
//std::initializer_list<sl3::Value> // crash on gcc 5.3 *
ValueList
typed_values (VALS&&... vals)
{
  return {sl3::Value{vals}...};
}


// * interesting, does not happen on 6.3
// on 5.3 there is a Value::Value (const Value& other) noexcept
// call where oter has some type


SCENARIO("create, assign, copy and moveing values")
{
  using namespace sl3;
  GIVEN ("some values of each supported type")
  {
    const int ival = 2 ;
    const double dval = 2.3 ;
    const std::string txt = "hello" ;
    const Blob blob{'A','B'};

    WHEN ("creating a values vector via the helper function")
    {
      ValueList vals {typed_values(ival, dval,txt, blob)} ;

      THEN ("the expected values with expected types have been created")
      { 
         CHECK(vals.at(0).getType() == Type::Int) ;
         CHECK(vals.at(0).int64() == ival) ;
         CHECK(vals.at(1).getType() == Type::Real) ;
         CHECK(vals.at(1).real() == dval) ;
         CHECK(vals.at(2).getType() == Type::Text) ;
         CHECK(vals.at(2).text() == txt) ;
         CHECK(vals.at(3).getType() == Type::Blob) ;
         CHECK(vals.at(3).blob() == blob) ;
         for (const auto& val : vals)
           CHECK_FALSE(val.isNull());

         AND_WHEN("adding a null value to the vector")
         {
           vals.emplace_back(Value{}) ;

           THEN ("the last element is a null value")
           {
             CHECK(vals.back().isNull()) ;
           }
         }
      }
    }

    WHEN ("creating values of each type")
    {
      ValueList vals {typed_values(ival, dval,txt, blob)} ;
      vals.emplace_back(Value{}) ;

      THEN ("every value can be copied to an other value")
      {

        for (auto val : vals)
          {
            CHECK_NOTHROW(val = vals.at(0));
            CHECK(val.getType() == Type::Int) ;
            CHECK(val.int64() == ival) ;
            CHECK_NOTHROW(val = vals.at(1));
            CHECK(val.getType() == Type::Real) ;
            CHECK(val.real() == dval) ;
            CHECK_NOTHROW(val = vals.at(2));
            CHECK(val.getType() == Type::Text) ;
            CHECK(val.text() == txt) ;
            CHECK_NOTHROW(val = vals.at(3));
            CHECK(val.getType() == Type::Blob) ;
            CHECK(val.blob() == blob) ;
            CHECK_NOTHROW(val = vals.at(4));
            CHECK(val.isNull()) ;
          }
      }
    }

    WHEN ("creating values of each type")
    {
      ValueList vals {typed_values(ival, dval,txt, blob)} ;
      vals.emplace_back(Value{}) ;

      THEN ("every value can be move assinged to an other value")
      {
        for (auto val : vals)
          {
            ValueList vals_m {typed_values(ival, dval,txt, blob)} ;
            vals_m.emplace_back(Value{}) ;

            CHECK_NOTHROW(val = std::move(vals_m.at(0)));
            CHECK(val.getType() == Type::Int) ;
            CHECK(val.int64() == ival) ;
            CHECK_NOTHROW(val = std::move(vals_m.at(1)));
            CHECK(val.getType() == Type::Real) ;
            CHECK(val.real() == dval) ;
            CHECK_NOTHROW(val = std::move(vals_m.at(2)));
            CHECK(val.getType() == Type::Text) ;
            CHECK(val.text() == txt) ;
            CHECK_NOTHROW(val = std::move(vals_m.at(3)));
            CHECK(val.getType() == Type::Blob) ;
            CHECK(val.blob() == blob) ;
            CHECK_NOTHROW(val = std::move(vals_m.at(4)));
            CHECK(val.isNull()) ;
          }
      }
    }

    WHEN ("creating values of each type")
    {
      ValueList vals {typed_values(int64_t{ival}, dval,txt, blob)} ;
      vals.emplace_back(Value{}) ;

      THEN ("every value can be assinged to any other value")
      {
        for (auto val : vals)
          {

            ValueList vals_m {typed_values(ival, dval,txt, blob)} ;
            vals_m.emplace_back(Value{}) ;

            const Value val_cp = val;
            for(auto val_m : vals_m)
              {
                CHECK(val_cp.getType() == val.getType()) ;
                CHECK(val_cp == val) ;
                const Value val_m_cp = val_m ;
                val = std::move(val_m) ;
                CHECK(val_m.isNull()) ;
                CHECK(val.getType() == val_m_cp.getType()) ;
                CHECK(val == val_m_cp) ;
                val = val_cp ; // reset
                CHECK(val == val_cp) ;
              }
          }
      }
    }

    WHEN ("constructing a value from const char")
    {
      Value cval{"hello"} ;
      THEN ("it is the same as constructing from std::string")
      {
        Value sval{std::string{"hello"}} ;
        CHECK (sval == cval) ;
      }
    }

  }
}
