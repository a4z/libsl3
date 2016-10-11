/******************************************************************************
 ------------- Copyright (c) 2009-2016 H a r a l d  A c h i t z ---------------
 ---------- < h a r a l d dot a c h i t z at g m a i l dot c o m > ------------
 ---- This Source Code Form is subject to the terms of the Mozilla Public -----
 ---- License, v. 2.0. If a copy of the MPL was not distributed with this -----
 ---------- file, You can obtain one at http://mozilla.org/MPL/2.0/. ----------
 ******************************************************************************/

#include <sl3/value.hpp>
#include <sl3/error.hpp>

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <limits>
#include <ostream>
#include <type_traits>

namespace sl3
{
  namespace 
  {
    // not in coverage, only used in never reachable case/if branches
    void raiseErrUnexpected (const std::string& msg) // LCOV_EXCL_LINE
    {
      throw ErrUnexpected (msg); // LCOV_EXCL_LINE
    }


    template <class T>
    typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
    almost_equal (T x, T y, int ulp)
    {
      using std::numeric_limits;
      // the machine epsilon has to be scaled to the magnitude of the values used
      // and multiplied by the desired precision in ULPs (units in the last
      // place)
      return std::abs (x - y)
                 < numeric_limits<T>::epsilon () * std::abs (x + y) * ulp
             // unless the result is subnormal
             || std::abs (x - y) < numeric_limits<T>::min ();
    }
   
  } //--------------------------------------------------------------------------



  Value::Value () noexcept 
  : _type (Type::Null)
  { 
  }

  Value::Value (int val) noexcept
  : _type (Type::Int)
  {
    _store.intval = val;
  }

  Value::Value (int64_t val) noexcept 
  : _type (Type::Int)
  {
    _store.intval = val;
  }

  Value::Value (std::string val) noexcept
  : _store (Type::Text)
  {
    new (&_store.textval) std::string (std::move (val));
  }

  Value::Value (double val) noexcept
  : _type (Type::Real)
  {
    _store.realval = val;
  }

  Value::Value (Blob val) noexcept
  : _type (Type::Blob)
  {
    new (&_store.blobval) Blob (std::move (val));
  }

  Value::~Value () noexcept 
  { 
    clearValue (); 
  }
  
  void
  Value::clearValue ()
  {
    if (_type == Type::Text)
      {
        _store.textval.~basic_string<std::string::value_type> ();
      }
    else if (_type == Type::Blob)
      {
        _store.blobval.~vector<Blob::value_type> ();
      }
    _type = Type::Null;
  }

  Value::Value (const Value& other) noexcept
  : _type (other._type)
  {
    switch (_type)
      {
      case Type::Null:
        break;

      case Type::Int:
        _store.intval = other._store.intval;
        break;

      case Type::Real:
        _store.realval = other._store.realval;
        break;

      case Type::Text:
        new (&_store.textval) std::string (other._store.textval);
        break;

      case Type::Blob:
        new (&_store.blobval) Blob (other._store.blobval);
        break;

      default:
        raiseErrUnexpected ("never reach"); // LCOV_EXCL_LINE
      }
  }

  Value::Value (Value&& other) noexcept
  : _type (other._type)
  {
    switch (_type)
      {
      case Type::Null:
        break;

      case Type::Int:
        _store.intval = std::move (other._store.intval);
        break;

      case Type::Real:
        _store.realval = std::move (other._store.realval);
        break;

      case Type::Text:
        new (&_store.textval) std::string (std::move (other._store.textval));
        other._store.textval.~basic_string ();
        break;

      case Type::Blob:
        new (&_store.blobval) Blob (std::move (other._store.blobval));
        other._store.blobval.~vector ();
        break;

      case Type::Variant:
        raiseErrUnexpected ("never reach"); // LCOV_EXCL_LINE
        break;                              // LCOV_EXCL_LINE
      }

    // important, set other to null so that clear does not trial to clear
    other._type = Type::Null;
  }

  Value&
  Value::operator= (const Value& other)
  {
    clearValue ();
    _type = other._type ;

    switch (_storageType)
      {
      case Type::Null:
        break;

      case Type::Int:
        _store.intval = other._store.intval;
        break;

      case Type::Real:
        _store.realval =  other._store.realval;
        break;

      case Type::Text:
        new (&_store.textval) std::string (other._store.textval);
        break;

      case Type::Blob:
        new (&_store.blobval) Blob (other._store.blobval);
        break;

      case Type::Variant:
        raiseErrUnexpected ("never reach"); // LCOV_EXCL_LINE
        break;                              // LCOV_EXCL_LINE
      }

    return *this;
  }

  Value&
  Value::operator= (Value&& other) 
  {
    _type = other._type ; 

    switch (_type)
      {
      case Type::Null:
        break;

      case Type::Int:
        _store.intval = std::move (other._store.intval);
        break;

      case Type::Real:
        _store.realval = std::move (other._store.realval);
        break;

      case Type::Text:
        new (&_store.textval) std::string (std::move (other._store.textval));
        other._store.textval.~basic_string ();
        break;

      case Type::Blob:
        new (&_store.blobval) Blob (std::move (other._store.blobval));
        other._store.blobval.~vector ();
        break;

      case Type::Variant:
        raiseErrUnexpected ("never reach"); // LCOV_EXCL_LINE
        break;                              // LCOV_EXCL_LINE
      }

    // important, set other to null so that clear does not trial to clear
    other._type = Type::Null;

    return *this;
  }

  Value&
  Value::operator= (int val)
  {
    if (_type == Type::Text)
      {
        _store.textval.~basic_string<std::string::value_type> ();
      }
    else if (_type == Type::Blob)
      {
        _store.blobval.~vector<Blob::value_type> ();
      }
    _store.intval = val ;
    _type = Type::Int ;
    return *this;
  }

  Value&
  Value::operator= (const int64_t& val)
  {
    if (_type == Type::Text)
      {
        _store.textval.~basic_string<std::string::value_type> ();
      }
    else if (_type == Type::Blob)
      {
        _store.blobval.~vector<Blob::value_type> ();
      }
    _store.intval = val ;
    _type = Type::Int ;
    return *this;
  }

  Value&
  Value::operator= (const double& val)
  {
    if (_type == Type::Text)
      {
        _store.textval.~basic_string<std::string::value_type> ();
      }
    else if (_type == Type::Blob)
      {
        _store.blobval.~vector<Blob::value_type> ();
      }
    _store.realval = val ;
    _type = Type::Real ;
    return *this;
  }

  Value&
  Value::operator= (const std::string& val)
  {
     if (_type == Type::Text)
      {
        store.textval = val ;
        return *this ;
      }
    else if (_type == Type::Blob)
      {
        _store.blobval.~vector<Blob::value_type> ();
      }

    new (&_store.textval) std::string (val);
    _type = Type::Text ;
    return *this;
  }

  Value&
  Value::operator= (const Blob& val)
  {
    if (_type == Type::Text)
      {
        _store.textval.~basic_string<std::string::value_type> ();
      }
    else if (_type == Type::Blob)
      {
        _store.blobval = val;
        return *this;
      }
    new (&_store.blobval) Blob (val);
    _type = Type::Blob ;
    return *this;
  }

  void
  Value::set (int val)
  {
    // not sure if I leaf this conversion,
    // but its better in text dbval.set(12); it type is real
    if (_type == Type::Real)
      set (static_cast<double> (val));
    else
      set (static_cast<int64_t> (val));
  }

  void
  Value::set (int64_t val)
  {
    ensure (_type).oneOf (Type::Int, Type::Variant);

    if (_storageType != Type::Int)
      {
        clearValue ();
        _storageType = Type::Int;
      }

    _store.intval = val;
  }

  void
  Value::set (double val)
  {
    ensure (_type).oneOf (Type::Real, Type::Variant);

    if (_storageType != Type::Real)
      {
        clearValue ();
        _storageType = Type::Real;
      }

    _store.realval = val;
  }

  void
  Value::set (const std::string& val)
  {
    ensure (_type).oneOf (Type::Text, Type::Variant);

    if (_storageType != Type::Text)
      {
        clearValue ();
        new (&_store.textval) std::string (val);
        _storageType = Type::Text;
      }
    else
      {
        _store.textval = val;
      }
  }

  void
  Value::set (const Blob& val)
  {
    ensure (_type).oneOf (Type::Blob, Type::Variant);

    if (_storageType != Type::Blob)
      {
        clearValue ();
        new (&_store.blobval) Blob (val);
        _storageType = Type::Blob;
      }
    else
      {
        _store.blobval = val;
      }
  }

  const int64_t&
  Value::getInt () const
  {
    ensure (_storageType).notNull ().sameAs (Type::Int);

    return _store.intval;
  }

  int64_t
  Value::getInt (int64_t defval) const
  {
    ensure (_type).oneOf (Type::Int, Type::Variant);

    if (isNull ())
      return defval;

    ensure (_storageType).sameAs (Type::Int);

    return _store.intval;
  }

  const double&
  Value::getReal () const
  {
    ensure (_storageType).notNull ().sameAs (Type::Real);

    return _store.realval;
  }

  double
  Value::getReal (double defval) const
  {
    ensure (_type).oneOf (Type::Real, Type::Variant);

    if (isNull ())
      return defval;

    ensure (_storageType).sameAs (Type::Real);

    return _store.realval;
  }

  const std::string&
  Value::getText () const
  {
    ensure (_storageType).notNull ().sameAs (Type::Text);

    return _store.textval;
  }

  std::string
  Value::getText (const std::string& defval) const
  {
    ensure (_type).oneOf (Type::Text, Type::Variant);

    if (isNull ())
      return defval;

    ensure (_storageType).sameAs (Type::Text);

    return _store.textval;
  }

  const Blob&
  Value::getBlob () const
  {
    ensure (_storageType).notNull ().sameAs (Type::Blob);

    return _store.blobval;
  }

  Blob
  Value::getBlob (const Blob& defval) const
  {
    ensure (_type).oneOf (Type::Blob, Type::Variant);

    if (isNull ())
      return defval;

    ensure (_storageType).sameAs (Type::Blob);

    return _store.blobval;
  }

  int64_t
  Value::get (int64_t defval) const
  {
    if (_storageType != Type::Int)
      return defval;

    return _store.intval;
  }

  int64_t
  Value::get (int defval) const
  {
    if (_storageType != Type::Int)
      return defval;

    return _store.intval;
  }

  double
  Value::get (double defval) const
  {
    if (_storageType != Type::Real)
      return defval;

    return _store.realval;
  }

  std::string
  Value::get (const std::string& defval) const
  {
    if (_storageType != Type::Text)
      return defval;

    return _store.textval;
  }

  Blob
  Value::get (const Blob& defval) const
  {
    if (_storageType != Type::Blob)
      return defval;

    return _store.blobval;
  }

  bool
  Value::operator== (const int val) const
  {
    if (_storageType == Type::Int)
      return _store.intval == val;

    return false;
  }

  bool
  Value::operator== (const int64_t& val) const
  {
    if (_storageType == Type::Int)
      return _store.intval == val;

    return false;
  }

  bool
  Value::operator== (const std::string& val) const
  {
    if (_storageType == Type::Text)
      return _store.textval == val;

    return false;
  }

  bool
  Value::operator== (const double& val) const
  {
    if (_storageType == Type::Real)
      return almost_equal (_store.realval, val, 2);

    return false;
  }

  bool
  Value::operator== (const Blob& val) const
  {
    if (_storageType == Type::Blob)
      return _store.blobval == val;

    return false;
  }

  bool
  Value::operator!= (const int val) const
  {
    return !(*this == val);
  }

  bool
  Value::operator!= (const int64_t& val) const
  {
    return !(*this == val);
  }

  bool
  Value::operator!= (const std::string& val) const
  {
    return !(*this == val);
  }

  bool
  Value::operator!= (const double& val) const
  {
    return !(*this == val);
  }

  bool
  Value::operator!= (const Blob& val) const
  {
    return !(*this == val);
  }

  std::string
  Value::ejectText ()
  {
    ensure (_storageType).notNull ().sameAs (Type::Text);

    auto tmp = std::move (_store.textval);
    setNull ();
    return tmp;
  }

  Blob
  Value::ejectBlob ()
  {
    ensure (_storageType).notNull ().sameAs (Type::Blob);

    auto tmp = std::move (_store.blobval);
    setNull ();
    return tmp;
  }

  std::ostream&
  operator<< (std::ostream& stm, const sl3::Value& v)
  {
    switch (v.getStorageType ())
      {
      case Type::Null:
        stm << "<NULL>";
        break;

      case Type::Int:
        stm << v.getInt ();
        break;

      case Type::Real:
        stm << v.getReal ();
        break;

      case Type::Text:
        stm << v.getText ();
        break;

      case Type::Blob:
        stm << "<BLOB>";
        break;

      default:
        stm << "unknown storage type !!"; // LCOV_EXCL_LINE
        break;                            // LCOV_EXCL_LINE
      }

    return stm;
  }

  void
  Value::setNull ()
  {
    clearValue ();
  }

  bool
  Value::isNull () const
  {
    return _storageType == Type::Null;
  }

  Type
  Value::getType () const
  {
    return _type;
  }

  Type
  Value::getStorageType () const
  {
    return _storageType;
  }

  bool
  Value::canAssign (const Value& other) const
  {
    if (this->getType () != Type::Variant)
      {
        if (other.getType () == Type::Variant)
          {
            return check (other._storageType).oneOf (_type, Type::Null);
          }
        else
          {
            return check (_type).sameAs (other.getType ());
          }
      }

    return true;
  }

  void
  Value::assign (const Value& other)
  {
    clearValue ();
    // _type = other._type ;
    // NOTE do not change the type,
    // if type is incompatible this line should not be reached!

    _storageType = other._storageType;

    switch (_storageType)
      {
      case Type::Null:
        break;

      case Type::Int:
        _store.intval = other._store.intval;
        break;

      case Type::Real:
        _store.realval = other._store.realval;
        break;

      case Type::Text:
        new (&_store.textval) std::string (other._store.textval);
        break;

      case Type::Blob:
        new (&_store.blobval) Blob (other._store.blobval);
        break;

      default:
        throw ErrUnexpected ("should never reach"); // LCOV_EXCL_LINE
      }
  }


} // ns
