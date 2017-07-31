/******************************************************************************
 ------------- Copyright (c) 2009-2017 H a r a l d  A c h i t z ---------------
 ---------- < h a r a l d dot a c h i t z at g m a i l dot c o m > ------------
 ---- This Source Code Form is subject to the terms of the Mozilla Public -----
 ---- License, v. 2.0. If a copy of the MPL was not distributed with this -----
 ---------- file, You can obtain one at http://mozilla.org/MPL/2.0/. ----------
 ******************************************************************************/

#include <sl3/dbvalue.hpp>
#include <sl3/error.hpp>

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <limits>
#include <ostream>
#include <type_traits>

#include <iostream>

namespace sl3
{



  namespace
  {
    template <typename T>
    constexpr bool
    oneof (const T&)
    {
      return false;
    }

    template <typename T, typename T1, typename... REST>
    constexpr bool
    oneof (const T& a, const T1& b, REST... rest)
    {
      return (a == b ? true : oneof (a, rest...));
    }

    struct check
    {
      check (Type t)
      : type (t)
      {
      } //----------------------------------------------------------------------

      bool
      notNull ()
      {
        return type != Type::Null;
      } //----------------------------------------------------------------------

      template <typename... ARGS> bool oneOf (ARGS... args)
      {
        return oneof (type, args...);

      } //--------------------------------------------------------------------

      bool
      sameAs (Type t)
      {
        return type == t;
      } //----------------------------------------------------------------------

    private:
      Type type;
    };

    struct ensure
    {
      ensure (Type t)
      : type (t)
      {
      } //----------------------------------------------------------------------

      ensure&
      notNull ()
      {
        if (type == Type::Null)
          throw ErrNullValueAccess ();

        return *this;
      } //----------------------------------------------------------------------

      template <typename... ARGS> ensure& oneOf (ARGS... args)
      {
        if (!oneof (type, args...))
          throw ErrTypeMisMatch (typeName (type)
                                 + " not one of required types");

        return *this;
      } //--------------------------------------------------------------------

      ensure&
      sameAs (Type t)
      {
        if (type != t)
          throw ErrTypeMisMatch (typeName (type) + "!=" + typeName (t));

        return *this;
      } //----------------------------------------------------------------------

    private:
      Type type;
    };


  } //--------------------------------------------------------------------------





  bool
  dbval_type_eq (const DbValue& a, const DbValue& b) noexcept
  {
    if (a.dbtype()  == b.dbtype())
      {
        return value_type_eq(a.getValue(), b.getValue()) ;
      }

    return false;
  }

  bool
  dbval_type_lt (const DbValue& a, const DbValue& b) noexcept
  {
    if (value_type_lt(a.getValue (), b.getValue ()))
      {
        return true;
      }

    if (value_type_eq(a.getValue(), b.getValue()))
      { // a variant is bigger
        return a.dbtype () < b.dbtype ();
      }

    return false;
  }

  // how, and does this makesense ?

  bool
  dbval_eq (const DbValue& a, const DbValue& b) noexcept
  {
    return value_eq (a.getValue (), b.getValue ());
  }

  bool
  dbval_lt (const DbValue& a, const DbValue& b) noexcept
  {
    return value_lt (a.getValue (), b.getValue ());
  }

  DbValue::DbValue (Type type) noexcept
      : _type (type == Type::Null ? Type::Variant : type)
  {
  }

  DbValue::DbValue (int val, Type type)
  : DbValue (type)
  {
    ensure (type).oneOf (Type::Int, Type::Variant);
    _value = val;
  }

  DbValue::DbValue (int64_t val, Type type)
  : DbValue (type)
  {
    ensure (type).oneOf (Type::Int, Type::Variant);
    _value = val;
  }

  DbValue::DbValue (std::string val, Type type)
  : DbValue (type)
  {
    ensure (type).oneOf (Type::Text, Type::Variant);
    _value = val;
  }

  DbValue::DbValue (double val, Type type)
  : DbValue (type)
  {
    ensure (type).oneOf (Type::Real, Type::Variant);
    _value = val;
  }

  DbValue::DbValue (Blob val, Type type)
  : DbValue (type)
  {
    ensure (type).oneOf (Type::Blob, Type::Variant);
    _value = val;
  }

  DbValue&
  DbValue::operator= (const DbValue& other)
  {
    if (!canAssign (other))
      {
        throw ErrTypeMisMatch (
            typeName (_type) + "="
            + (other._type == Type::Variant
                   ? typeName (other._type) + " with storage type"
                         + typeName (other.type ())
                   : typeName (other._type)));
      }

    assign (other);
    return *this;
  }

  DbValue&
  DbValue::operator= (DbValue&& other)
  {
    if (!canAssign (other))
      {
        throw ErrTypeMisMatch (
            typeName (_type) + "="
            + (other._type == Type::Variant
                   ? typeName (other._type) + " with storage type"
                         + typeName (other.type ())
                   : typeName (other._type)));
      }
    _value = std::move (other._value);

    return *this;
  }

  DbValue&
  DbValue::operator= (int val)
  {
    set (val);
    return *this;
  }

  DbValue&
  DbValue::operator= (const int64_t& val)
  {
    set (val);
    return *this;
  }

  DbValue&
  DbValue::operator= (const double& val)
  {
    set (val);
    return *this;
  }

  DbValue&
  DbValue::operator= (const std::string& val)
  {
    set (val);
    return *this;
  }

  DbValue&
  DbValue::operator= (const Blob& val)
  {
    set (val);
    return *this;
  }

  DbValue&
  DbValue::operator= (const Value& val)
  {
    ensure (_type).oneOf (val.getType (), Type::Variant);
    _value = val;
    return *this;
  }

  void
  DbValue::set (int val)
  {
    // not sure if I leaf this conversion,
    // but its better in text dbval.set(12); it type is real
    if (_type == Type::Real)
      set (static_cast<double> (val));
    else
      set (static_cast<int64_t> (val));
  }

  void
  DbValue::set (int64_t val)
  {
    ensure (_type).oneOf (Type::Int, Type::Variant);
    _value = val;
  }

  void
  DbValue::set (double val)
  {
    ensure (_type).oneOf (Type::Real, Type::Variant);
    _value = val;
  }

  void
  DbValue::set (const std::string& val)
  {
    ensure (_type).oneOf (Type::Text, Type::Variant);
    _value = val;
  }

  const Value&
  DbValue::getValue () const noexcept
  {
    return _value;
  }

  void
  DbValue::set (const Blob& val)
  {
    ensure (_type).oneOf (Type::Blob, Type::Variant);
    _value = val;
  }

  const int64_t&
  DbValue::getInt () const
  {
    return _value.int64 ();
  }

  int64_t
  DbValue::getInt (int64_t defval) const
  {
    if (isNull ())
      return defval;

    return _value.int64 ();
  }

  const double&
  DbValue::getReal () const
  {
    return _value.real ();
  }

  double
  DbValue::getReal (double defval) const
  {
    if (isNull ())
      return defval;

    return _value.real ();
  }

  const std::string&
  DbValue::getText () const
  {
    return _value.text ();
  }

  std::string
  DbValue::getText (const std::string& defval) const
  {
    if (isNull ())
      return defval;

    return _value.text ();
  }

  const Blob&
  DbValue::getBlob () const
  {
    return _value.blob ();
  }

  Blob
  DbValue::getBlob (const Blob& defval) const
  {
    if (isNull ())
      return defval;

    return _value.blob ();
  }

  int64_t
  DbValue::get (int64_t defval) const
  {
    if (_value.getType () != Type::Int)
      return defval;

    return _value.int64 ();
  }

  int64_t
  DbValue::get (int defval) const
  {
    if (_value.getType () != Type::Int)
      return defval;

    return _value.int64 ();
  }

  double
  DbValue::get (double defval) const
  {
    if (_value.getType () != Type::Real)
      return defval;

    return _value.real ();
  }

  std::string // TODO consider change to reference, with warning in the doc
      DbValue::get (const std::string& defval) const
  {
    if (_value.getType () != Type::Text)
      return defval;

    return _value.text ();
  }

  Blob // TODO consider change to reference, with warning in the doc
      DbValue::get (const Blob& defval) const
  {
    if (_value.getType () != Type::Blob)
      return defval;

    return _value.blob ();
  }

  std::string
  DbValue::ejectText ()
  {
    return _value.ejectText ();
  }

  Blob
  DbValue::ejectBlob ()
  {
    return _value.ejectBlob ();
  }

  std::ostream&
  operator<< (std::ostream& stm, const sl3::DbValue& v)
  {
    stm << v.getValue ();
    return stm;
  }

  void
  DbValue::setNull ()
  {
    _value.setNull ();
  }

  bool
  DbValue::isNull () const
  {
    return _value.isNull ();
  }

  Type
  DbValue::dbtype () const
  {
    return _type;
  }

  Type
  DbValue::type () const
  {
    return _value.getType ();
  }

  bool
  DbValue::canAssign (const DbValue& other) const
  {
    if (this->dbtype () != Type::Variant)
      {
        if (other.dbtype () == Type::Variant)
          {
            return check (other.type ()).oneOf (_type, Type::Null);
          }
        else
          {
            return check (_type).sameAs (other.dbtype ());
          }
      }

    return true;
  }

  void
  DbValue::assign (const DbValue& other)
  {
    _value = other._value;
  }

} // ns
