
#include <a4testing.hpp>

#include <sl3/value.hpp>
#include <sl3/error.hpp>

#include "eqorder.hpp"

#include <functional>


namespace sl3
{
  namespace
  {

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

    }

    // todo throw with convert usw ..
    // trow on illegal access ....


    a4TestAdd (a4test::suite ("value")
          .addTest ("compare", compare)

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


