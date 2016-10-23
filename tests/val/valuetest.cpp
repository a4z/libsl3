
#include <a4testing.hpp>

#include <sl3/value.hpp>
#include <sl3/error.hpp>
#include "eqorder.hpp"

#include <sstream>

namespace sl3
{
  namespace
  {

    using VauleRelation = std::function<bool(const Value&, const Value&)>;

    VauleRelation strong_eq = std::equal_to<sl3::Value>() ;
    VauleRelation strong_lt = std::less<Value>() ;

    VauleRelation weak_eq = sl3::weak_eq;
    VauleRelation weak_lt = sl3::weak_lt ;


    void compare ()
    {
      Value ival {1} ;
      Value rval {1.0} ;
      Value txtval {"foo"} ;
      Value blobval {Blob()} ;


      BOOST_CHECK (ival == 1);
      BOOST_CHECK (ival == 1.0);
      BOOST_CHECK (ival != std::string("foo"));

      BOOST_CHECK (rval == 1);
      BOOST_CHECK (rval == 1.0);
      BOOST_CHECK (rval != std::string("foo"));


      BOOST_CHECK (txtval == std::string("foo"));
   
      BOOST_CHECK (blobval != std::string("foo"));
      BOOST_CHECK (blobval == Blob());


      Value nullval {} ;
      BOOST_CHECK (nullval.isNull());

      Value nullval2 {nullval} ;
      BOOST_CHECK (nullval2.isNull());

      BOOST_CHECK (nullval == nullval2);

      Value nullval3 {std::move (nullval2)} ;
      BOOST_CHECK (nullval3.isNull());


    }


    void
    strictTotalOrdered ()
    {
      using namespace eqo ;

      {
      Value a(100), b(100), c(100) ;
      Value d{}, e(200), f(200.0) ;

      BOOST_CHECK (eq_reflexive (a, strong_eq));
      BOOST_CHECK (eq_symmetric (a,b, strong_eq));
      BOOST_CHECK (eq_transitive (a,b,c, strong_eq));

      BOOST_CHECK (irreflexive (a,b, strong_eq, strong_lt));
      BOOST_CHECK (lt_transitive (d,e,f, strong_lt));
      BOOST_CHECK (trichotomy (d,e, strong_eq, strong_lt));
      }

      {
      Value a(100), b(100), c(100) ;
      Value d{1}, e(1.0), f(Blob{}) ;

      BOOST_CHECK (eq_reflexive (a, strong_eq));
      BOOST_CHECK (eq_symmetric (a,b, strong_eq));
      BOOST_CHECK (eq_transitive (a,b,c, strong_eq));

      BOOST_CHECK (irreflexive (a,b, strong_eq, strong_lt));
      BOOST_CHECK (lt_transitive (d,e,f, strong_lt));
      BOOST_CHECK (trichotomy (d,e, strong_eq, strong_lt));
      }


    }


    void
    weakTotalOrdered ()
    {
      using namespace eqo ;
      BOOST_CHECK (weak_eq(Value{1}, Value{1}));
      BOOST_CHECK (weak_eq(Value{1}, Value{1.0}));

      {
        Value a{}, b{}, c{} ;
        Value d{}, e(1), f("foo") ;

        BOOST_CHECK (weak_reflexive (a,a, weak_eq));
        BOOST_CHECK (eq_symmetric (a,b, weak_eq));
        BOOST_CHECK (eq_transitive (a,b,c, weak_eq));

        BOOST_CHECK (irreflexive (a,b, weak_eq, weak_lt));
        BOOST_CHECK (lt_transitive (d,e,f, weak_lt));
        BOOST_CHECK (trichotomy (d,e, weak_eq, weak_lt));
      }

      {
        Value a{1.0}, b{1}, c{1} ;
        Value d{}, e(1.1), f(Blob{}) ;

        BOOST_CHECK (weak_reflexive (a,a, weak_eq));
        BOOST_CHECK (eq_symmetric (a,b, weak_eq));
        BOOST_CHECK (eq_transitive (a,b,c, weak_eq));

        BOOST_CHECK (irreflexive (a,b, weak_eq, weak_lt));
        BOOST_CHECK (lt_transitive (d,e,f, weak_lt));
        BOOST_CHECK (trichotomy (d,e, weak_eq, weak_lt));
      }


    }


    void changeTypes ()
    {
      {
        Value v{} ;
        Value s{""} ;
        s = v ;
        BOOST_CHECK (s.getType() == Type::Null) ;
      }
      {
        Value v{""} ;
        Value s{} ;
        s = std::move (v) ;
        BOOST_CHECK (s.getType() == Type::Text) ;
        BOOST_CHECK (v.isNull()) ;
        v = std::string("foo") ;
        BOOST_CHECK (v.getType() == Type::Text) ;
        v = 1.1 ;
        BOOST_CHECK (v.getType() == Type::Real) ;

        v = std::string("foo") ;
        BOOST_CHECK (v.getType() == Type::Text) ;
        int64_t i = 1 ;
        v = i ;
        BOOST_CHECK (v.getType() == Type::Int) ;

        v = std::string("foo") ;
        BOOST_CHECK (v.getType() == Type::Text) ;
        v = Blob{} ;
        BOOST_CHECK (v.getType() == Type::Blob) ;
        BOOST_CHECK (v.blob() == Blob{}) ;
      }

      {
        Value v{""} ;
        BOOST_CHECK (v.getType() == Type::Text) ;
        v = 1 ;
        BOOST_CHECK (v.getType() == Type::Int) ;
        v = Blob{} ;
        BOOST_CHECK (v.getType() == Type::Blob) ;
        v = 1.1 ;
        BOOST_CHECK (v.getType() == Type::Real) ;
      }

      {
        Value v{Blob{} } ;
        BOOST_CHECK (v.getType() == Type::Blob) ;
        v = 1 ;
        BOOST_CHECK (v.getType() == Type::Int) ;
      }
      {
        Value v{Blob{} } ;
        BOOST_CHECK (v.getType() == Type::Blob) ;
        int64_t i = 1 ;
        v = i ;
        BOOST_CHECK (v.getType() == Type::Int) ;
      }
      {
        Value v{Blob{} } ;
        BOOST_CHECK (v.getType() == Type::Blob) ;
        v = std::string() ;
        BOOST_CHECK (v.getType() == Type::Text) ;
      }



    }


    void
    toStream ()
    {
      {
        std::stringstream ss ;
        ss << Value{};
        BOOST_CHECK_EQUAL (ss.str (), "<NULL>") ;

      }
      {
        std::stringstream ss ;
        ss << Value{1};
        BOOST_CHECK_EQUAL (ss.str (), "1") ;

      }
      {
        std::stringstream ss ;
        ss << Value{1.0};
        BOOST_CHECK_EQUAL (ss.str (), "1") ;

      }
      {
        std::stringstream ss ;
        ss << Value{"foo"};
        BOOST_CHECK_EQUAL (ss.str (), "foo") ;
      }
      {
        std::stringstream ss ;
        ss << Value{Blob{}};
        BOOST_CHECK_EQUAL (ss.str (), "<BLOB>") ;

      }
    }

    void
    swapValues ()
    {
      Value a{Blob{}};
      Value b{""};

      using sl3::swap ;

      BOOST_CHECK_NO_THROW (swap (a, b)) ;

      BOOST_CHECK (a.getType() == Type::Text) ;
      BOOST_CHECK (b.getType() == Type::Blob) ;
    }

    a4TestAdd (a4test::suite ("value")
          .addTest ("compare", compare)
          .addTest ("strictTotalOrdered", strictTotalOrdered)
          .addTest ("weakTotalOrdered", weakTotalOrdered )
          .addTest ("changeTypes", changeTypes )
          .addTest ("toStream", toStream )
          .addTest ("swap", swapValues  )
    );
  }
}






/*
 *
 * das kann jetzt jeder selber implementieren, wenn gewollt
 *
  template <typename T>
  inline const T&
  get (const Value&)
  {
    static_assert (false,
                   "Invalid type to get from DbValue!");
  }

  template <typename T>
  inline const T*
  get_if (const Value&)
  {
    static_assert (false,
                   "Invalid type to get from DbValue!");
  }


  template <>
  inline const int64_t&
  get (const Value& v)
  {
    if (v._type() == Type::Null)
      throw ErrNullValueAccess() ;
    else if (v._type() != Type::Int)
      throw ErrTypeMisMatch();

    return v._store.intval ;
  }

  template <>
  inline const int64_t*
  get_if (const Value& v)
  {
    return v._type() != Type::Int ? nullptr : v._store.intval ;
  }


  template <>
  inline const double&
  get (const Value& v)
  {
    if (v._type() == Type::Null)
      throw ErrNullValueAccess() ;
    else if (v._type() != Type::Real)
      throw ErrTypeMisMatch();

    return v._store.realval ;
  }

  template <>
  inline const double*
  get_if (const Value& v)
  {
    return v._type() != Type::Real ? nullptr : v._store.realval ;
  }



  template <>
  inline const std::string&
  get (const Value& v)
  {
    if (v._type() == Type::Null)
      throw ErrNullValueAccess() ;
    else if (v._type() != Type::Text)
      throw ErrTypeMisMatch();

    return v._store.textval ;
  }

  template <>
  inline const std::string*
  get_if (const Value& v)
  {
    return v._type() != Type::Text ? nullptr : v._store.textval ;
  }



  template <>
  inline const Blob&
  get (const Value& v)
  {
    if (v._type() == Type::Null)
      throw ErrNullValueAccess() ;
    else if (v._type() != Type::Blob)
      throw ErrTypeMisMatch();

    return v._store.blobval ;
  }

  template <>
  inline const Blob*
  get_if (const Value& v)
  {
    return v._type() != Type::Blob ? nullptr : v._store.blobval ;
  }
*/


