/******************************************************************************
 ------------- Copyright (c) 2009-2016 H a r a l d  A c h i t z ---------------
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

    /** \brief Constructor
     *
     *  This constructor takes a initialization value, and optionally a type
     *  which can only be Type::Variant or the default one.
     *
     *  \throw sl3::ErrTypeMisMatch if given type is incompatible
     *  \param val initial value
     *  \param type wanted type, default set to type but can
     *  be set to DbValue::Variant if waned
     */
    DbValue (int val, Type type = Type::Int);

    /**
     * \copydoc DbValue(int val, Type type = Type::Int)
     */
    DbValue (int64_t val, Type type = Type::Int);

    /**
     * \copydoc DbValue(int val, Type type = Type::Int)
     */
    DbValue (std::string val, Type type = Type::Text);

    /**
     * \copydoc DbValue(int val, Type type = Type::Int)
     */
    DbValue (double val, Type type = Type::Real);

    /**
     * \copydoc DbValue(int val, Type type = Type::Int)
     */
    DbValue (Blob val, Type type = Type::Blob);

    /**
     * \brief Destructor
     */
    ~DbValue () noexcept;

    /**
     * \brief Copy constructor
     */
    DbValue (const DbValue&) noexcept;

    /**
     * \brief Move constructor
     */
    DbValue (DbValue&&) noexcept;

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
    DbValue& operator= (const Blob& val);

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
    std::string get (const std::string& defval) const;

    /**
     * \copydoc get(int64_t defval) const;
     */
    Blob get (const Blob& defval) const;

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
     * The type might be Type::Varian, so that a DbValue can hold any value,
     * or a specific Type to guarantee the only the allow type is used.
     *
     * \return the type
     */
    Type getType () const;

    /**
     * \brief Returns the underlying type
     *
     * If getType() is a Variant this property
     * returns the actual type information, otherwise it will be the same as
     * type or null.
     *
     * \return the type the value actual holds
     */
    Type getStorageType () const;

    /**
     * \brief Check if assignment would be OK
     *
     * If this function return true, the other value can
     * be assign because it is type compatible.
     *
     * \param other value to check
     * \return true if the other value can be assigned
     */
    bool canAssign (const DbValue& other) const;

  private:
    const Type _type;
    // type never changes, even == makes just the value, not the type,

    Type _storageType;
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

    void assign (const DbValue& other);

    void clearValue ();
  };

  /**
   * \brief Stream op for a DbValue
   *
   * \param stm an outstream
   * \param v the value to stream
   * \return ostream
   */
  LIBSL3_API std::ostream& operator<< (std::ostream& stm, const sl3::DbValue& v);


  /**
   * \brief global equality 
   *   
   * Check if 2 DbValue instances are of the same type and of the same value.
   *
   * \return true if the type and the current value are equal, false otherwise
   */
  bool operator== (const DbValue& a, const DbValue& b) noexcept ;


   /**
   * \brief global unequal 
   *   
   * Check if 2 DbValue instances are not equal.
   *
   * \returns true if given DbValue instances are not equal
   */
 
  bool operator!= (const DbValue& a, const DbValue& b) noexcept ;
  
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
  get (const DbValue&)
  {
    static_assert (always_false<T>::value,
                   "Invalid type to get from DbValue!");
  }

  template <>
  inline const int64_t&
  get (const DbValue& v)
  {
    return v.getInt ();
  }

  template <>
  inline const double&
  get (const DbValue& v)
  {
    return v.getReal ();
  }

  template <>
  inline const std::string&
  get (const DbValue& v)
  {
    return v.getText ();
  }

  template <>
  inline const Blob&
  get (const DbValue& v)
  {
    return v.getBlob ();
  }
}

#endif /* DbValue_HPP_ */
