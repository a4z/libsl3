/******************************************************************************
 ------------- Copyright (c) 2009-2016 H a r a l d  A c h i t z ---------------
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

      template <typename... ARGS>
      bool
      oneOf (ARGS... args)
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

      template <typename... ARGS>
      ensure&
      oneOf (ARGS... args)
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

    // not in coverage, only used in never reachable case/if branches
    void raiseErrUnexpected (const std::string& msg) // LCOV_EXCL_LINE
    {
      throw ErrUnexpected (msg); // LCOV_EXCL_LINE
    }

  } //--------------------------------------------------------------------------

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

  bool
  sameValue (const DbValue& a, const DbValue& b)
  {
    bool retVal = false;

    switch (a.getStorageType ())
      {
      case Type::Null:
        retVal = b.getStorageType () == Type::Null;
        break;

      case Type::Int:
        if (b.getStorageType () == Type::Int)
          retVal = a.getInt () == b.getInt ();
        else if (b.getStorageType () == Type::Real)
          retVal = almost_equal (
              static_cast<double> (a.getInt ()), b.getReal (), 2);
        break;

      case Type::Real:
        if (b.getStorageType () == Type::Real)
          retVal = almost_equal (a.getReal (), b.getReal (), 2);
        else if (b.getStorageType () == Type::Int)
          retVal = almost_equal (
              a.getReal (), static_cast<double> (b.getInt ()), 2);
        break;

      case Type::Text:
        if (b.getStorageType () == Type::Text)
          retVal = a.getText () == b.getText ();
        break;

      case Type::Blob:
        if (b.getStorageType () == Type::Blob)
          retVal = a.getBlob () == b.getBlob ();
        break;

      default:
        break; // never reached?
      }

    return retVal;
  }

  bool
  operator== (const DbValue& a, const DbValue& b) noexcept
  {
    if (check (a.getType ()).sameAs (b.getType ()))
      {
        return sameValue (a, b);
      }

    return false;
  }

  bool
  operator!= (const DbValue& a, const DbValue& b) noexcept
  {
    return !(a == b);
  }

  bool
  operator< (const DbValue& a, const DbValue& b) noexcept
  {
    // todo if we have the same storage type, but different type,
    // than a variant should be NOT less a concrete type

    if (a.getStorageType () == Type::Null)
      {
        if (b.getStorageType () == Type::Null)
          return a.getType () < b.getType ();
        else
          return true;
      }

    if (b.getStorageType () == Type::Null)
      return false; // a not null handled above

    if (a.getStorageType () == Type::Blob)
      {
        if (b.getStorageType () == Type::Blob)
          {
            if (a.getBlob () < b.getBlob ())
              {
                return true;
              }
            else if (a.getBlob () == b.getBlob ())
              return a.getType () < b.getType ();
            else
              return false;
          }
        else
          return false; // blob always bigger than other types
      }

    if (b.getStorageType () == Type::Blob)
      return true;

    if (a.getStorageType () == Type::Text)
      {
        if (b.getStorageType () == Type::Text)
          {
            if (a.getText () < b.getText ())
              return true;
            else if (a.getText () == b.getText ())
              return a.getType () < b.getType ();
            else
              return false;
          }
        else
          return false; // blob already handled, text > than numbers
      }

    if (b.getStorageType () == Type::Text)
      return true; // we are a number, therefore smaller

    if (a.getStorageType () == Type::Int)
      {
        if (b.getStorageType () == Type::Int)
          {
            if (a.getInt () < b.getInt ())
              return true;
            else if (a.getInt () == b.getInt ())
              return a.getType () < b.getType ();
            else
              return false;
          }
        else // b is real
          {
            if (a.getInt () < b.getReal ())
              return true;
            else if (sameValue (a, b))
              return a.getType () < b.getType ();
            else
              return false;
          }
      }

    // we are real :-)
    if (b.getStorageType () == Type::Int)
      return a.getReal () < b.getInt ();
    else if (b.getStorageType () == Type::Real)
      return a.getReal () < b.getReal ();

    if (sameValue (a, b))
      {
        return a.getType () < b.getType ();
      }

    return false;
  }

  DbValue::DbValue (Type type) noexcept
  : _type (type == Type::Null ? Type::Variant : type)
  , _storageType (Type::Null)
  {
  }

  DbValue::DbValue (int val, Type type)
  : DbValue (type)
  {
    ensure (type).oneOf (Type::Int, Type::Variant);

    _store.intval = val;
    _storageType  = Type::Int;
  }

  DbValue::DbValue (int64_t val, Type type)
  : DbValue (type)
  {
    ensure (type).oneOf (Type::Int, Type::Variant);

    _store.intval = val;
    _storageType  = Type::Int;
  }

  DbValue::DbValue (std::string val, Type type)
  : DbValue (type)
  {
    ensure (type).oneOf (Type::Text, Type::Variant);

    new (&_store.textval) std::string (std::move (val));
    _storageType = Type::Text;
  }

  DbValue::DbValue (double val, Type type)
  : DbValue (type)
  {
    ensure (type).oneOf (Type::Real, Type::Variant);

    _store.realval = val;
    _storageType   = Type::Real;
  }

  DbValue::DbValue (Blob val, Type type)
  : DbValue (type)
  {
    ensure (type).oneOf (Type::Blob, Type::Variant);

    new (&_store.blobval) Blob (std::move (val));
    _storageType = Type::Blob;
  }

  DbValue::~DbValue () noexcept { clearValue (); }
  void
  DbValue::clearValue ()
  {
    if (_storageType == Type::Text)
      {
        _store.textval.~basic_string<std::string::value_type> ();
      }
    else if (_storageType == Type::Blob)
      {
        _store.blobval.~vector<Blob::value_type> ();
      }
    _storageType = Type::Null;
  }

  DbValue::DbValue (const DbValue& other) noexcept
  : _type (other._type)
  , _storageType (other._storageType)
  {
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
        raiseErrUnexpected ("never reach"); // LCOV_EXCL_LINE
        return;                             // LCOV_EXCL_LINE
      }
  }

  DbValue::DbValue (DbValue&& other) noexcept
  : _type (other._type)
  , _storageType (other._storageType)
  {
    switch (_storageType)
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
    other._storageType = Type::Null;
  }

  DbValue&
  DbValue::operator= (const DbValue& other)
  {
    if (!canAssign (other))
      {
        throw ErrTypeMisMatch (typeName (_type) + "="
                               + (other._type == Type::Variant
                                      ? typeName (other._type)
                                            + " with storage type"
                                            + typeName (other._storageType)
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
        throw ErrTypeMisMatch (typeName (_type) + "="
                               + (other._type == Type::Variant
                                      ? typeName (other._type)
                                            + " with storage type"
                                            + typeName (other._storageType)
                                      : typeName (other._type)));
      }

    if (_storageType != Type::Null)
      clearValue ();

    // _type = other._type ;
    // NOTE do not change the type,
    // if type is incompatible this line should not be reached!

    _storageType = std::move (other._storageType);

    switch (_storageType)
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
        break;

      case Type::Blob:
        new (&_store.blobval) Blob (std::move (other._store.blobval));
        break;

      default:
        throw ErrUnexpected ("should never reach"); // LCOV_EXCL_LINE
      }

    // important, set other to null
    other.clearValue ();

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
    set (std::move (val));
    return *this;
  }

  DbValue&
  DbValue::operator= (const Blob& val)
  {
    set (std::move (val));
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

    if (_storageType != Type::Int)
      {
        clearValue ();
        _storageType = Type::Int;
      }

    _store.intval = val;
  }

  void
  DbValue::set (double val)
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
  DbValue::set (const std::string& val)
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
  DbValue::set (const Blob& val)
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
  DbValue::getInt () const
  {
    ensure (_storageType).notNull ().sameAs (Type::Int);

    return _store.intval;
  }

  int64_t
  DbValue::getInt (int64_t defval) const
  {
    ensure (_type).oneOf (Type::Int, Type::Variant);

    if (isNull ())
      return defval;

    ensure (_storageType).sameAs (Type::Int);

    return _store.intval;
  }

  const double&
  DbValue::getReal () const
  {
    ensure (_storageType).notNull ().sameAs (Type::Real);

    return _store.realval;
  }

  double
  DbValue::getReal (double defval) const
  {
    ensure (_type).oneOf (Type::Real, Type::Variant);

    if (isNull ())
      return defval;

    ensure (_storageType).sameAs (Type::Real);

    return _store.realval;
  }

  const std::string&
  DbValue::getText () const
  {
    ensure (_storageType).notNull ().sameAs (Type::Text);

    return _store.textval;
  }

  std::string
  DbValue::getText (const std::string& defval) const
  {
    ensure (_type).oneOf (Type::Text, Type::Variant);

    if (isNull ())
      return defval;

    ensure (_storageType).sameAs (Type::Text);

    return _store.textval;
  }

  const Blob&
  DbValue::getBlob () const
  {
    ensure (_storageType).notNull ().sameAs (Type::Blob);

    return _store.blobval;
  }

  Blob
  DbValue::getBlob (const Blob& defval) const
  {
    ensure (_type).oneOf (Type::Blob, Type::Variant);

    if (isNull ())
      return defval;

    ensure (_storageType).sameAs (Type::Blob);

    return _store.blobval;
  }

  int64_t
  DbValue::get (int64_t defval) const
  {
    if (_storageType != Type::Int)
      return defval;

    return _store.intval;
  }

  int64_t
  DbValue::get (int defval) const
  {
    if (_storageType != Type::Int)
      return defval;

    return _store.intval;
  }

  double
  DbValue::get (double defval) const
  {
    if (_storageType != Type::Real)
      return defval;

    return _store.realval;
  }

  std::string
  DbValue::get (const std::string& defval) const
  {
    if (_storageType != Type::Text)
      return defval;

    return _store.textval;
  }

  Blob
  DbValue::get (const Blob& defval) const
  {
    if (_storageType != Type::Blob)
      return defval;

    return _store.blobval;
  }

  bool
  DbValue::operator== (const int val) const
  {
    if (_storageType == Type::Int)
      return _store.intval == val;

    return false;
  }

  bool
  DbValue::operator== (const int64_t& val) const
  {
    if (_storageType == Type::Int)
      return _store.intval == val;

    return false;
  }

  bool
  DbValue::operator== (const std::string& val) const
  {
    if (_storageType == Type::Text)
      return _store.textval == val;

    return false;
  }

  bool
  DbValue::operator== (const double& val) const
  {
    if (_storageType == Type::Real)
      return almost_equal (_store.realval, val, 2);

    return false;
  }

  bool
  DbValue::operator== (const Blob& val) const
  {
    if (_storageType == Type::Blob)
      return _store.blobval == val;

    return false;
  }

  bool
  DbValue::operator!= (const int val) const
  {
    return !(*this == val);
  }

  bool
  DbValue::operator!= (const int64_t& val) const
  {
    return !(*this == val);
  }

  bool
  DbValue::operator!= (const std::string& val) const
  {
    return !(*this == val);
  }

  bool
  DbValue::operator!= (const double& val) const
  {
    return !(*this == val);
  }

  bool
  DbValue::operator!= (const Blob& val) const
  {
    return !(*this == val);
  }

  std::string
  DbValue::ejectText ()
  {
    ensure (_storageType).notNull ().sameAs (Type::Text);

    auto tmp = std::move (_store.textval);
    setNull ();
    return tmp;
  }

  Blob
  DbValue::ejectBlob ()
  {
    ensure (_storageType).notNull ().sameAs (Type::Blob);

    auto tmp = std::move (_store.blobval);
    setNull ();
    return tmp;
  }

  std::ostream&
  operator<< (std::ostream& stm, const sl3::DbValue& v)
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
  DbValue::setNull ()
  {
    clearValue ();
  }

  bool
  DbValue::isNull () const
  {
    return _storageType == Type::Null;
  }

  Type
  DbValue::getType () const
  {
    return _type;
  }

  Type
  DbValue::getStorageType () const
  {
    return _storageType;
  }

  bool
  DbValue::canAssign (const DbValue& other) const
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
  DbValue::assign (const DbValue& other)
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

  void
  swap (DbValue& a, DbValue& b) noexcept
  {
    using std::swap;

    if (a.getStorageType () == b.getStorageType ())
      {
        swap (a._type, b._type);

        switch (a.getStorageType ())
          {
          case Type::Blob:
            swap (a._store.blobval, b._store.blobval);
            break;

          case Type::Text:
            swap (a._store.textval, b._store.textval);
            break;

          case Type::Real:
            swap (a._store.realval, b._store.realval);
            break;

          case Type::Int:
            swap (a._store.intval, b._store.intval);
            break;

          default:
            break; // both are null
          }
      }
    else
      {
        DbValue tmp (std::move (a));
        a.setNull ();

        auto moveAssing = [](auto& from, auto& to) -> void {
          to._type        = from._type;
          to._storageType = from._storageType;

          switch (to.getStorageType ())
            {
            case Type::Blob:
              to._store.blobval = std::move (from._store.blobval);
              break;

            case Type::Text:
              to._store.textval = std::move (from._store.textval);
              break;

            case Type::Real:
              to._store.realval = from._store.realval;
              break;

            case Type::Int:
              to._store.intval = from._store.intval;
              break;

            default:
              break;
            }
        };

        moveAssing (b, a);
        b.setNull ();
        b._type = tmp._type;
        b       = std::move (tmp);
      }
  }

} // ns
