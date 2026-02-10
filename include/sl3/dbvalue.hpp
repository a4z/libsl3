/******************************************************************************
 ------------- Copyright (c) 2009-2023 H a r a l d  A c h i t z ---------------
 ---------- < h a r a l d dot a c h i t z at g m a i l dot c o m > ------------
 ---- This Source Code Form is subject to the terms of the Mozilla Public -----
 ---- License, v. 2.0. If a copy of the MPL was not distributed with this -----
 ---------- file, You can obtain one at http://mozilla.org/MPL/2.0/. ----------
 ******************************************************************************/

#ifndef SL3_DBVALUE_HPP_
#define SL3_DBVALUE_HPP_

#include <string>

#include <sl3/config.hpp>
#include <sl3/error.hpp>
#include <sl3/types.hpp>
#include <sl3/value.hpp>

// todo after a while, type and storage type become confusing
// find better name, type should be value type,
// storage type something else, find better name!

namespace sl3
{
  /**
   *  \brief
   *
   * This class models the duck typing sqlite uses.
   * It supports int, real, text, blob and null values.
   *
   * The type can be a variant, to support all of the possible types,
   * or it can be set explicitly to a wanted type.
   *
   * Using this type for communication with the database can ensure either
   * type guarantee or give the flexibility of variant types.
   *
   * If a type is of type variant, the storage type can be any of the
   * supported types, otherwise, if the type is set to a specific type, it
   * is guaranteed that the type will match, or an exception occurs.
   *
   */
  class LIBSL3_API DbValue
  {
  public:
    /**
     * \brief Constructor
     *
     *  Constructs a type  and the value is null.
     *
     *  \param type wanted storage type
     *  If Type::Null is given, the type will be a variant.
     */
    DbValue (Type type) noexcept;

    // TODO constructor Value, Value - Type,
    // would this maybe make the other value c'tors obsolete?
    // this would be fine
    // don't like the ctors below anyway, they should take a flag,
    // variant or not, but not the type

    /** \brief Constructor
     *
     *  This constructor takes an initialization value, and optionally a type
     *  which can only be Type::Variant or the default one.
     *
     *
     *  \throw sl3::ErrTypeMisMatch if given type is incompatible
     *  \param val initial value
     *  \param type wanted type, default set to type but can
     *  be set to DbValue::Variant if wanted
     */
    explicit DbValue (int val, Type type = Type::Int);

    /**
     * \copydoc DbValue(int val, Type type = Type::Int)
     */
    explicit DbValue (int64_t val, Type type = Type::Int);

    /**
     * \copydoc DbValue(int val, Type type = Type::Int)
     */
    explicit DbValue (std::string val, Type type = Type::Text);

    /**
     * \copydoc DbValue(int val, Type type = Type::Int)
     */
    explicit DbValue (double val, Type type = Type::Real);

    /**
     * \copydoc DbValue(int val, Type type = Type::Int)
     */
    explicit DbValue (Blob val, Type type = Type::Blob);

    /**
     * \brief Destructor
     */
    ~DbValue () noexcept = default;

    /**
     * \brief Copy constructor
     */
    DbValue (const DbValue&) noexcept = default;

    /**
     * \brief Move constructor
     */
    DbValue (DbValue&&) noexcept = default;

    /** \brief Assignment
     *  \throw sl3::ErrTypeMisMatch if getType is incompatible
     *  \note , only value assignment happens here,
     *  the type does not change, the storage type might change in case of type
     *  is a variant.
     *
     * \param val new value
     * \return reference to this
     */
    DbValue& operator= (const DbValue& val);

    /**
     * \copydoc operator=(const DbValue& val)
     */
    DbValue& operator= (DbValue&& val);

    /**
     * \copydoc operator=(const DbValue& val)
     */
    DbValue& operator= (int val);

    /**
     * \copydoc operator=(const DbValue& val)
     */
    DbValue& operator= (const int64_t& val);

    /**
     * \copydoc operator=(const DbValue& val)
     */
    DbValue& operator= (const double& val);

    /**
     * \copydoc operator=(const DbValue& val)
     */
    DbValue& operator= (const std::string& val);

    /**
     * \copydoc operator=(const DbValue& val)
     */
    DbValue&
    operator= (const char* val)
    {
      return *this = std::string{val};
    }

    /**
     * \copydoc operator=(const DbValue& val)
     */
    DbValue& operator= (const Blob& val);

    /**
     * \copydoc operator=(const DbValue& val)
     */
    DbValue& operator= (const Value& val);

    /** \brief Assignment
     *  \throw sl3::ErrTypeMisMatch if getType is incompatible
     *  \note , only value assignment happens here,
     *  the type does not change, the storage type might change in case of type
     *  is a variant.
     *  \param val new value
     */
    void set (int val);

    /**
     * \copydoc set(int val)
     */
    void set (int64_t val);

    /**
     * \copydoc set(int val)
     */
    void set (const std::string& val);

    /**
     * \copydoc set(int val)
     */
    void set (double val);

    /**
     * \copydoc set(int val)
     */
    void set (const Blob& val);

    /** \brief Value access
     *  \return reference to the underlying Value
     */
    const Value& getValue () const noexcept;

    /** \brief Value access
     *  Just a shorter way than getValue.
     *  \return reference to the
     * underlying Value
     */
    const Value&
    value () const noexcept
    {
      return getValue ();
    };

    /** \brief Value access
     *  \throw sl3::ErrNullValueAccess if value is null.
     *  \throw sl3::ErrTypeMisMatch if getType is incorrect
     *  \return reference to the value
     */
    const int64_t& getInt () const;

    /**
     * \copydoc getInt() const
     */
    const double& getReal () const;

    /**
     * \copydoc getInt() const
     */
    const std::string& getText () const;

    /**
     * \copydoc getInt() const
     */
    const Blob& getBlob () const;

    /** \brief Value access with default for a NULL value.
     *
     *  \throw sl3::ErrTypeMisMatch if getType is incorrect
     *  \param defval default value to return if value is NULL
     *  \return the value or the given defval in case the value is NULL
     */
    int64_t getInt (int64_t defval) const;

    /**
     * \copydoc getInt(int64_t defval) const;
     */
    double getReal (double defval) const;

    /**
     * \copydoc getInt(int64_t defval) const;
     */
    std::string getText (const std::string& defval) const;

    /**
     * \copydoc getInt(int64_t defval) const;
     */
    Blob getBlob (const Blob& defval) const;

    /** \brief Value access with default for a NULL and different type value.
     *
     *  This method will not throw. If the actual value is null or of a
     *  different type, the given default value will be returned.
     *
     *  \param defval default value to return
     *  \return the value or the given default value if the value needs to be
     *      replaced
     */
    int64_t get (int64_t defval) const;

    /**
     * \copydoc get(int64_t defval) const;
     */
    int64_t get (int defval) const;

    /**
     * \copydoc get(int64_t defval) const;
     */
    double get (double defval) const;

    /**
     * \copydoc get(int64_t defval) const;
     */
    std::string get (const std::string& defval) const;

    /**
     * \copydoc get(int64_t defval) const;
     */
    Blob get (const Blob& defval) const;

    /** \brief Moves the current value into the return value
     *
     *  After calling this function the value will be Null.
     *
     *  \throw sl3::ErrTypeMisMatch in case of wrong type.
     *  \return The value
     */
    std::string ejectText ();

    /**
     * \copydoc ejectText()
     */
    Blob ejectBlob ();

    /**
     * \brief Set to NULL
     */
    void setNull ();

    /**
     * \brief Check Null
     * \return if the value is null
     */
    bool isNull () const;

    /**
     * \brief The Type of the value.
     *
     * This is the type with which the Value has been created
     * and will not change.
     *
     * The type might be Type::Variant, so that a DbValue can hold any value,
     *
     * or a specific Type to guarantee that only the allowed type is used.
     *
     * \return the type
     */
    Type dbtype () const;

    /**
     * \brief Returns the type of the underlying Value
     *
     * If getType() is a Variant this property
     * returns the actual type information, otherwise it will be the same as
     * type or null.
     *
     * \return the type the value actually holds
     */
    Type type () const;

    /**
     * \brief Check if assignment would be OK
     *
     * If this function returns true, the other value can
     * be assigned
     * because it is type compatible.
     *
     * \param other value to check
     * \return true if the other value can be assigned
     */
    bool canAssign (const DbValue& other) const;

    /**
     * \brief Swap 2 DbValues
     * \param a first value to swap second value
     * \param b second value to swap with first
     */
    friend void swap (DbValue& a, DbValue& b) noexcept;

  private:
    Type _type;

    Value _value;

    friend class DbValues;

    void assign (const DbValue& other);
  };

  /**
   * \brief Stream op for a DbValue
   *
   * \param stm an outstream
   * \param v the value to stream
   * \return ostream
   */
  LIBSL3_API std::ostream& operator<< (std::ostream&       stm,
                                       const sl3::DbValue& v);

  /**
   * \brief equality, including type info
   *
   * Check if 2 DbValue instances are of the same type and of the same value.
   *
   * \param a first value to compare
   * \param b second value to compare
   *
   * \return true if the type and the current value are equal, false otherwise
   */
  bool dbval_type_eq (const DbValue& a, const DbValue& b) noexcept;

  /**
   * \brief less than,  including type info
   *
   * Applies following rules which are equal to the sorting rules of sqlite.
   *
   * - Type::Null is always less than any other storage type.
   * - Type::Integer or Type::Real is always less than Type::Text or
   * Type::Blob
   * - Type::Text is less than Type::Blob
   *
   *  The type used is DbValue.getStorageType.
   *
   *  The comparison of the value itself is implemented via std::less.
   *
   * \param a first value to compare
   * \param b second value to compare
   *
   * \returns true if given DbValue a is less than given DbValue b
   */
  bool dbval_type_lt (const DbValue& a, const DbValue& b) noexcept;

  /**
   * \brief equality, ignoring type info
   *
   * Compares only the stored value for equality and ignores type information.
   *
   * \param a first value to compare
   * \param b second value to compare
   * \return the comparison result
   */
  bool dbval_eq (const DbValue& a, const DbValue& b) noexcept;

  /**
   * \brief less than, ignoring type info
   *
   * Compares only the stored value and ignores type information.
   *
   * \param a first value to compare
   * \param b second value to compare
   * \return the comparison result
   */
  bool dbval_lt (const DbValue& a, const DbValue& b) noexcept;

}

#endif /* DbValue_HPP_ */
