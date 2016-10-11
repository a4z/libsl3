/******************************************************************************
 ------------- Copyright (c) 2009-2016 H a r a l d  A c h i t z ---------------
 ---------- < h a r a l d dot a c h i t z at g m a i l dot c o m > ------------
 ---- This Source Code Form is subject to the terms of the Mozilla Public -----
 ---- License, v. 2.0. If a copy of the MPL was not distributed with this -----
 ---------- file, You can obtain one at http://mozilla.org/MPL/2.0/. ----------
 ******************************************************************************/

#ifndef SL3_VALUE_HPP_
#define SL3_VALUE_HPP_

#include <string>

#include <sl3/config.hpp>
#include <sl3/error.hpp>
#include <sl3/types.hpp>

namespace sl3
{
  /**
   *  \brief
   *
   * This class models the duck typing sqlite uses.
   * It supports int, real, text, blob and null values.
   *
  * The type can be a variant, to support all of the possible types,
   * or it can be set explicit to a wanted type.
   *
   * Using this type for communication with the database can ensure either
   * type guarantee or give the flexibility of variant types.
   *
   * If a type is of type variant, the storage type can be any of the
   * supported types, otherwise, if the type is set to a specific type, it
   * is guaranteed that the type will match, or an exception occurs.
   *
   */
  class LIBSL3_API Value
  {
  public:
    /**
     * \brief Constructor
     *
     * Creates a Null Value
     */
    Value() noexcept ;


    /** \brief Constructor
     *
     *  This constructor wiht an initialization value
     *  \param val initial value
     */
    explicit Value (int val) noexcept; 

    /**
     * \copydoc Value(int val)
     */
    explicit Value (int64_t val) noexcept;

    /**
     * \copydoc Value(int val)
     */
    explicit Value (std::string val) noexcept;

    /**
     * \copydoc Value(int val)
     */
    explicit Value (double val) noexcept;

    /**
     * \copydoc Value(int val)
     */
    explicit Value (Blob val) noexcept;

    /**
     * \brief Destructor
     */
    ~Value () noexcept;

    /**
     * \brief Copy constructor
     */
    Value (const Value&) noexcept;

    /**
     * \brief Move constructor
     */
    Value (Value&&) noexcept;

    /** \brief Assignment
     *  \throw sl3::ErrTypeMisMatch if getType is incompatible
     *  \note , only value assignment happens here,
     *  the type does not change, the storage type might change in case of type
     *  is a variant.
     *
     * \param val new value
     * \return reference to this
     */
    Value& operator= (const Value& val);

    /**
     * \copydoc operator=(const Value& val)
     */
    Value& operator= (Value&& val);

    /**
     * \copydoc operator=(const Value& val)
     */
    Value& operator= (int val);

    /**
     * \copydoc operator=(const Value& val)
     */
    Value& operator= (const int64_t& val);

    /**
     * \copydoc operator=(const Value& val)
     */
    Value& operator= (const double& val);

    /**
     * \copydoc operator=(const Value& val)
     */
    Value& operator= (const std::string& val);

    /**
     * \copydoc operator=(const Value& val)
     */
    Value& operator= (const Blob& val);

    /** \brief Implicit conversion operator
     *  \throw sl3::ErrNullValueAccess if value is null.
     *  \throw sl3::ErrTypeMisMatch if getType is incorrect
     *  \return reference to the value
     */
    operator int () const;

    operator int64_t () const;

    operator double () const ;

    operator std::string& () const ; // TODO ref or val

    operator Blob& () const ; // TODO ref or val


    /**
     * \brief Get the valye applying type conversion if required.
     *
     * Applies the same conversion rules as sqlite3 does.
     *
     */
    double asInt () const;

    /**
     * \copydoc getInt() const
     */
    double asReal () const;

    /**
     * \copydoc getInt() const
     */
    std::string asText () const;

    /**
     * \copydoc getInt() const
     */
    Blob asBlob () const;

    /** \brief Value access with default for a NULL value.
     *
     *  \throw sl3::ErrTypeMisMatch if getType is incorrect
     *  \param defval default value to return if value is NULL/
     *  \return the value or the give defval in case value is NULL
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
     *  different type, the fiven default value will be returned.
     *
     *  \param defval default value to return
     *  \return the value or the give default value if the value needs to be
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
    const std::string& get (const std::string& defval) const;

    /**
     * \copydoc get(int64_t defval) const;
     */
    const Blob& get (const Blob& defval) const;

    /**
     * \brief Compare value for equality
     * \param val value to compare with
     * \return if given values is equal
     */
    bool operator== (const int val) const;

    /**
     * \copydoc operator==(const int val) const
     */
    bool operator== (const int64_t& val) const;

    /**
     * \copydoc operator==(const int val) const
     */
    bool operator== (const std::string& val) const;

    /**
     * \copydoc operator==(const int val) const
     */
    bool operator== (const double& val) const;

    /**
     * \copydoc operator==(const int val) const
     */
    bool operator== (const Blob& val) const;

    /** \brief Compare value inequality.
     * \param val value to compare with
     * \return if given values are not equal
     */
    bool operator!= (const int val) const;

    /**
     * \copydoc operator!=(const int val) const
     */
    bool operator!= (const int64_t& val) const;

    /**
     * \copydoc operator!=(const int val) const
     */
    bool operator!= (const std::string& val) const;

    /**
     * \copydoc operator!=(const int val) const
     */
    bool operator!= (const double& val) const;

    /**
     * \copydoc operator!=(const int val) const
     */
    bool operator!= (const Blob& val) const;

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
     * The type might be Type::Varian, so that a Value can hold any value,
     * or a specific Type to guarantee the only the allow type is used.
     *
     * \return the type
     */
    Type getType () const;



  private:
    Type _type;
    // type never changes, even == makes just the value, not the type,
    // except when swapping .....

    // that's the type for the union and what is applied in the db
    union Store
    {
      Store () {}
      ~Store () {}
      int64_t     intval;
      double      realval;
      std::string textval;
      Blob        blobval;
    };

    Store _store;

    friend class DbValues;

    void assign (const Value& other);

    void clearValue ();
  };

  /**
   * \brief Stream op for a Value
   *
   * \param stm an outstream
   * \param v the value to stream
   * \return ostream
   */
  std::ostream& operator<< (std::ostream& stm, const sl3::Value& v);

  /**
   * \brief global equality
   *
   * Check if 2 Value instances are of the same type and of the same value.
   *
   * \return true if the type and the current value are equal, false otherwise
   */
  bool operator== (const Value& a, const Value& b) noexcept;

  /**
   * \brief global unequal
   *
   * Check if 2 Value instances are not equal.
   *
   * \return true if given Value instances are not equal
   */
  bool operator!= (const Value& a, const Value& b) noexcept;

  /**
   * \brief global less operator for Value
   *
   * Applies following rules which are equal to the sorting rules of sqlite.
   *
   * - Type::Null is alwasy less than any other storage type.
   * - Type::Interger or Type::Real is always less than Type::Text or
   * Type::Blob
   * - Type::Text is less than Type::Blob
   *
   *  The type used is Value.getStorageType.
   *
   *  The comparison of the value itself is implemented via std::less.
   *
   * \returns true if given Value a is less than given Value b
   */
  bool operator< (const Value& a, const Value& b) noexcept;

  /*  disable for now, sorting does not always call swap, ..  
   * \brief Value specialised swap function
   *
   *  Independend of the type, a Value is always swapable.
   *  This can be theroretical be abused to bypass the tye checking,
   *  but is up to the user to do so or not.
   */
  //void swap (Value& a, Value& b) noexcept;

  // variant like access
  template <typename T> struct always_false
  {
    enum
    {
      value = false
    };
  };

  template <typename T>
  inline const T&
  get (const Value&)
  {
    static_assert (always_false<T>::value,
                   "Invalid type to get from Value!");
  }

  template <>
  inline const int64_t&
  get (const Value& v)
  {
    return v.getInt ();
  }

  template <>
  inline const double&
  get (const Value& v)
  {
    return v.getReal ();
  }

  template <>
  inline const std::string&
  get (const Value& v)
  {
    return v.getText ();
  }

  template <>
  inline const Blob&
  get (const Value& v)
  {
    return v.getBlob ();
  }
}

#endif 
