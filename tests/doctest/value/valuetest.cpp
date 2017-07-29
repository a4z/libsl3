#include "../testing.hpp"
#include <sl3/value.hpp>

#include <vector>
#include <string>
#include <limits>
#include <sstream>

using ValueList = std::vector<sl3::Value> ;

template <typename... VALS>
//std::initializer_list<sl3::Value> // crash on gcc 5.3 *
ValueList
typed_values (VALS&&... vals)
{
  return {sl3::Value{vals}...};
}


namespace sl3
{
  // if ever wanted, implement < and == operators yourself like this
  inline
  bool
  operator== (const sl3::Value& a, const sl3::Value& b) noexcept
  {
    return sl3::value_type_eq(a, b) ;
  }

  inline
  bool
  operator< (const sl3::Value& a, const sl3::Value& b) noexcept
  {
    return sl3::value_type_lt(a, b) ;
  }
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

SCENARIO("null value values access")
{
  GIVEN ("a check throw on invalid value access function")
  {
    sl3::Value val ;
    WHEN ("trying to access any value")
    {
      CHECK(val.isNull()) ;
      THEN ("err null values access will be trhows")
      {
        CHECK_THROWS_AS(val.int64(), sl3::ErrNullValueAccess) ;
        CHECK_THROWS_AS(static_cast<int64_t>(val), sl3::ErrNullValueAccess) ;
        CHECK_THROWS_AS(val.real(), sl3::ErrNullValueAccess) ;
        CHECK_THROWS_AS(static_cast<double>(val), sl3::ErrNullValueAccess) ;
        CHECK_THROWS_AS(val.text(), sl3::ErrNullValueAccess) ;
        CHECK_THROWS_AS(static_cast<std::string>(val), sl3::ErrNullValueAccess) ;
        CHECK_THROWS_AS(val.blob(), sl3::ErrNullValueAccess) ;
        CHECK_THROWS_AS(static_cast<sl3::Blob>(val), sl3::ErrNullValueAccess) ;
      }
    }
  }
}


SCENARIO("invalid type access")
{
  using namespace sl3;
  GIVEN ("a check throw on invalid value access function")
  {
    const int ival = 2 ;
    const double dval = 2.3 ;
    const std::string txt = "hello" ;
    const Blob blob{'A','B'};

    auto check_invariants = [&](const Value val)
    {
      if(val.getType() != Type::Int)
        {
          CHECK_THROWS_AS(val.int64(), ErrTypeMisMatch) ;
          CHECK_THROWS_AS(static_cast<int64_t>(val), ErrTypeMisMatch) ;
        }
      else
        {
          CHECK_EQ(val.int64(), ival) ;
          CHECK_EQ(static_cast<int64_t>(val), ival) ;
        }
      // casting int to real works , other way not
      if(val.getType() != Type::Real && val.getType() != Type::Int)
        {
          CHECK_THROWS_AS(val.real(), ErrTypeMisMatch) ;
          CHECK_THROWS_AS(static_cast<double>(val), ErrTypeMisMatch) ;
        }
      else if(val.getType() == Type::Int)
        {
          CHECK_EQ(val.int64(), ival) ;
          CHECK_EQ(static_cast<int64_t>(val), ival) ;
        }
      else if(val.getType() == Type::Real)
        {
          CHECK_EQ(val.real(),dval) ;
          CHECK_EQ(static_cast<double>(val), dval) ;
        }


      if(val.getType() != Type::Text)
        {
          CHECK_THROWS_AS(val.text(), ErrTypeMisMatch) ;
          CHECK_THROWS_AS(static_cast<std::string>(val), ErrTypeMisMatch) ;
        }
      else
        {
          CHECK_EQ(val.text(),txt) ;
          CHECK_EQ(static_cast<std::string>(val), txt) ;
        }

      if(val.getType() != Type::Blob)
        {
          CHECK_THROWS_AS(val.blob(), ErrTypeMisMatch) ;
          CHECK_THROWS_AS(static_cast<Blob>(val), ErrTypeMisMatch) ;
        }
      else
        {
          CHECK_EQ(val.blob(),blob) ;
          CHECK_EQ(static_cast<Blob>(val), blob) ;
        }
    };


    WHEN ("having values of each type")
    {
      ValueList vals {typed_values(ival, dval,txt, blob)} ;

      THEN ("running check_throw on each fo this value will work")
      {
        for (auto v : vals)
          check_invariants(v) ;
      }

      THEN("assinging the values to something otherworks")
      {
        vals.emplace_back(Value{}) ;
        for (auto v : vals)
          {
            v = ival ;
            check_invariants(v) ;
          }
        for (auto v : vals)
           {
             v = int64_t{ival};
             check_invariants(v) ;
           }

        for (auto v : vals)
           {
             v = dval ;
             check_invariants(v) ;
           }
        for (auto v : vals)
           {
             v = txt;
             check_invariants(v) ;
           }
        for (auto v : vals)
           {
             v = blob ;
             check_invariants(v) ;
           }
      }
    }
  }
}

SCENARIO ("number special")
{
  using namespace sl3 ;
  // do the int stuff extra here
  GIVEN ("a null value")
  {
    Value v ;
    WHEN ("assign an integer")
    {
      v = 100 ;
      THEN ("accessing that integer is not a problem")
      {
        CHECK_EQ(static_cast<int32_t>(v), 100) ;
      }
    }

    WHEN ("converting a null value to an int")
    {
      THEN ("a null value access happens")
      {
        CHECK_THROWS_AS((void)static_cast<int32_t>(v), ErrNullValueAccess) ;
      }
    }

    WHEN ("assign an int value bigger than max int")
    {
      v= std::numeric_limits<int64_t>::max() ;

      THEN ("converting to an integer will might throw")
      {
         CHECK_THROWS_AS((void)static_cast<int>(v), ErrOutOfRange) ;
      }
    }

    WHEN ("assign a real value that is int")
    {
      v = 1.00 ;
      THEN ("converting to an integer will work")
      {
        CHECK_EQ(static_cast<int32_t>(v), 1) ;
      }
    }

    WHEN ("assign a real value that is not an int")
    {
      v = 1.11 ;
      THEN ("converting to an integer will throw")
      {
        CHECK_THROWS_AS((void)static_cast<int32_t>(v), ErrTypeMisMatch) ;
      }
    }

    WHEN ("assign an integer")
    {
      v = 100 ;
      THEN ("converting to a real just works")
      {
        CHECK_EQ(static_cast<double>(v), 100.0) ;
      }
    }

    WHEN ("assign a real value bigger than is to big")
    {
      v = std::numeric_limits<double>::max() ;
      THEN ("converting to an integer will throw")
      {
        CHECK_THROWS_AS((void)static_cast<int32_t>(v), ErrOutOfRange) ;
        CHECK_THROWS_AS((void)static_cast<int64_t>(v), ErrOutOfRange) ;
      }
    }

    WHEN ("assign something that is not an int")
    {
      v = "hello";
      THEN ("converting to an integer will throw")
      {
        CHECK_THROWS_AS((void)static_cast<int32_t>(v), ErrTypeMisMatch) ;
      }
    }
  }
}

SCENARIO("eject values")
{
  using namespace sl3 ;
  GIVEN ("a blob an a string value with knowen value")
  {
    const std::string txt = "hello" ;
    const Blob blob{'A','B'};

    sl3::Value sval{txt} ;
    sl3::Value bval{blob} ;
    WHEN ("ejecting the blob / string value from the right type")
    {
      auto etxt = sval.ejectText();
      auto eblob = bval.ejectBlob();

      THEN ("the ejected values are expected")
      {
        CHECK_EQ(txt, etxt);
        CHECK_EQ(blob, eblob);

        AND_THEN("the values are resteded to Null")
        {
          CHECK(sval.isNull());
          CHECK(bval.isNull());
        }
      }
    }
    WHEN ("ejecting the blob / string value from the wrongtype")
    {
      THEN ("exceptions are throws")
      {
        CHECK_THROWS_AS(sval.ejectBlob(), ErrTypeMisMatch);
        CHECK_THROWS_AS(bval.ejectText(), ErrTypeMisMatch);
      }
    }
    WHEN ("ejecting from Null values")
    {
      THEN ("exceptions are throws")
      {
        Value val ;
        CHECK_THROWS_AS(val.ejectBlob(), ErrNullValueAccess);
        CHECK_THROWS_AS(val.ejectText(), ErrNullValueAccess);
      }
    }
  }
}


SCENARIO("compare  values")
{
  using namespace sl3;
  GIVEN ("a check smaller function")
  {
    auto check_smaller= [](const Value& val, const ValueList& smaller)
    {
      CHECK_FALSE(value_lt(val, val)) ;
      CHECK_FALSE(value_type_lt(val, val)) ;

      for (const auto& v : smaller)
        {
          CHECK(value_lt(v, val)) ;
          CHECK(value_type_lt(v, val)) ;
          CHECK_FALSE(value_lt(val, v)) ;
          CHECK_FALSE(value_type_lt(val, v)) ;
        }
    };

    WHEN ("checking values with expected smaller values")
    {
      Value v{0} ;
      ValueList smaller = {Value{-1}, Value{}, Value{-1.0} };
      THEN ("all check succeed")
      {
        check_smaller(Value{0}, smaller);
        check_smaller(Value{1.0}, smaller);
        smaller.push_back(Value{"aaa"});
        check_smaller(Value{"bbb"}, smaller);
        smaller.push_back(Value{Blob{'A'}});
        check_smaller(Value{Blob{'A', 'B'}}, smaller);
      }
    }
  }

  GIVEN("a function that checks equalities")
  {
    auto check_eq= [](const Value& val, const ValueList& different)
    {
      CHECK(value_eq(val, val)) ;
      CHECK(value_type_eq(val, val)) ;

      for (const auto& v : different)
        {
          CHECK_FALSE(value_eq(val, v)) ;
          CHECK_FALSE(value_type_eq(val, v)) ;
        }
    };
    WHEN ("callin check_eq with knowen values")
    {
      THEN("all tests succeed")
      {
        check_eq(Value{},
                 {Value{1}, Value{2.0}, Value{"foo"}, Value{Blob{'A'}}});

        check_eq(Value{1},
                 {Value{},Value{2.0}, Value{"foo"}, Value{Blob{'A'}}});

        check_eq(Value{2.0},
                 {Value{},Value{1}, Value{"foo"}, Value{Blob{'A'}}});

        check_eq(Value{"foo"},
                 {Value{},Value{1}, Value{2.0}, Value{Blob{'A'}}});

        check_eq(Value{Blob{'A'}},
                 {Value{},Value{1}, Value{2.0}, Value{"foo"}});

      }
    }
  }

  GIVEN("int and a real types with same values")
  {
    Value ival{1} ;
    Value rval{1.0} ;

    WHEN ("comparing with them self")
    {
      THEN("value_eq is true and value_type_eq is true")
      {
        CHECK(value_eq(ival, ival)) ;
        CHECK(value_type_eq(ival, ival)) ;
        CHECK(value_eq(rval, rval)) ;
        CHECK(value_type_eq(rval, rval)) ;
      }
    }

    WHEN ("comparing both with each other")
    {
      THEN("value_eq is true and value_type_eq is false")
      {
        CHECK(value_eq(ival, rval)) ;
        CHECK_FALSE(value_type_eq(ival, rval)) ;
      }
    }
  }
}


SCENARIO("swapping values")
{
  using namespace sl3;
  GIVEN("2 different values")
  {
    Value a{1};
    Value b{"a longer text with no small string opt"};

    WHEN("swapping those values")
    {
      std::swap(a,b);
      THEN("the values are swapped")
      {
        CHECK(a.getType() == Type::Text);
        CHECK(a.text() == "a longer text with no small string opt");

        CHECK(b.getType() == Type::Int);
        CHECK(b.int64() == 1);
      }
    }
  }
}


SCENARIO("stringing values")
{
  using namespace sl3;
  GIVEN("a string stream")
  {
    std::stringstream ss ;
    WHEN("stringing a null value")
    {
      ss << Value{} ;
      THEN("the stingstream contains <NULL>")
      {
        CHECK(ss.str() == "<NULL>");
      }
    }
    WHEN("stringing an int value")
    {
      ss << Value{1} ;
      THEN("the stingstream contains the int")
      {
        CHECK(ss.str() == "1");
      }
    }

    WHEN("stringing a real value")
    {
      ss << Value{1.1} ;
      THEN("the stingstream contains the real")
      {
        CHECK(ss.str() == "1.1");
      }
    }

    WHEN("stringing a text value")
    {
      ss << Value{"hello"} ;
      THEN("the stingstream contains the text")
      {
        CHECK(ss.str() == "hello");
      }
    }

    WHEN("stringing a blob value")
    {
      ss << Value{Blob{}} ;
      THEN("the stingstream contains <BLOB>")
      {
        CHECK(ss.str() == "<BLOB>");
      }
    }

  }
}



