/******************************************************************************
 ------------- Copyright (c) 2009-2017 H a r a l d  A c h i t z ---------------
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
   * \brief Mixin creating order operations based on defined ones
   *
   * \tparam T a type with defined equality and less operator.
   *
   * Creates !=, > , <= and >= operators if == and < are defined on T.
   *
   * \see Value or DbValue for usage
   *
   */
  template<typename T>
  struct totally_ordered
  {

    /**
     * \brief derived operator
     * \param a first element to compare
     * \param b second element to compare
     * \return comparison result
     */
    friend bool
    operator!= (const T& a, const T& b) noexcept
    {
      return !(a==b);
    }

    /// \copydoc operator!=
    friend bool
    operator> (const T& a, const T& b) noexcept
    {
      return b < a ;
    }

    /// \copydoc operator!=
    friend bool
    operator<= (const T& a, const T& b) noexcept
    {
      return !(a > b) ;
    }

    /// \copydoc operator!=
    friend bool
    operator>= (const T& a, const T& b) noexcept
    {
      return !(a < b) ;
    }

  };


  /**
   *  \brief
   *
   * This class models the duck typing sqlite uses.
   * It supports int, real, text, blob and null values.
   *
   * The class has the current type info availalbe.
   *
   *
   */
  class LIBSL3_API Value : totally_ordered<Value>
  {
  public:
    /**
     * \brief Constructor
     *
     * Creates a Null Value
     */
    Value () noexcept;

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
    explicit Value (const char* val);

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
     *  \throw sl3::ErrTypeMisMatch if getType is incompatible
     *  \throw sl3::ErrOutOfRange is the stored value is an int64_t
     *  \return  the value
     */
    explicit operator int () const;

    /** \brief Implicit conversion operator
     *  \throw sl3::ErrNullValueAccess if value is null.
     *  \throw sl3::ErrTypeMisMatch if getType is incompatible
     *  \return  the value
     */
    explicit operator int64_t () const;

    /** \brief Implicit conversion operator
     *  \throw sl3::ErrNullValueAccess if value is null.
     *  \throw sl3::ErrTypeMisMatch if getType is incompatible
     *  \throw sl3::ErrOutOfRange is the stored value is an int64_t and out of
     *    the min or max double range.
     *  \return  the value
     */
    explicit operator double () const;

    /** \brief Implicit conversion operator
     *  \throw sl3::ErrNullValueAccess if value is null.
     *  \throw sl3::ErrTypeMisMatch if getType is incompatible
     *  \return  the value
     */
    explicit operator const std::string& () const; // TODO ref or val

    /** \brief Implicit conversion operator
     *  \throw sl3::ErrNullValueAccess if value is null.
     *  \throw sl3::ErrTypeMisMatch if getType is incompatible
     *  \return  the value
     */
    explicit operator const Blob& () const; // TODO ref or val

    /** \brief Access the value
     *  \throw sl3::ErrNullValueAccess if value is null.
     *  \throw sl3::ErrTypeMisMatch if the current value has a different type.
     *  \return  reference to the value
     */
    const int64_t& int64 () const;

    /** \brief Access the value
     *  \throw sl3::ErrNullValueAccess if value is null.
     *  \throw sl3::ErrTypeMisMatch if the current value has a different type.
     *  \return  reference to the value
     */
    const double& real () const;

    /** \brief Access the value
     *  \throw sl3::ErrNullValueAccess if value is null.
     *  \throw sl3::ErrTypeMisMatch if the current value has a different type.
     *  \return  reference to the value
     */
    const std::string& text () const;

    /** \brief Access the value
     *  \throw sl3::ErrNullValueAccess if value is null.
     *  \throw sl3::ErrTypeMisMatch if the current value has a different type.
     *  \return  reference to the value
     */
    const Blob& blob () const;

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
    void setNull () noexcept;

    /**
     * \brief Check Null
     * \return if the value is null
     */
    bool isNull () const noexcept;

    /**
     * \brief The Type of the value.
     *
     * \return the type
     */
    Type getType () const noexcept;

    friend bool operator== (const Value& a, const Value& b) noexcept;
    friend bool operator< (const Value& a, const Value& b) noexcept;
    friend std::ostream& operator<< (std::ostream& stm, const sl3::Value& v);

    friend bool weak_eq (const Value& a, const Value& b) noexcept;
    friend bool weak_lt (const Value& a, const Value& b) noexcept;

  private:
    Type _type;

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
   * \brief total order equality Value
   *
   * Check if 2 Value instances are of the same type and of the same value.
   *
   * \param a first element to compare
   * \param b second element to compare
   *
   * \return true if the type and the current value are equal, false otherwise
   */
  bool operator== (const Value& a, const Value& b) noexcept;



  /**
   * \brief total order less than Value
   *
   * Applies following rules which are equal to the sorting rules of sqlite.
   *
   * - Type::Null is alwasy less than any other storage type.
   * - Type::Interger or Type::Real
   *     -- if the type is different, but the value equal,
   *     than Type::Int < Type::Real .
   *     if this is unwanted, use the weak_lt function.
   *     -- always less than Type::Text or Type::Blob
   * - Type::Text is less than Type::Blob
   *
   *  The comparison of the value itself is implemented via std::less.
   *
   *  \param a first element to compare
   *  \param b second element to compare
   *
   * \returns true if given Value a is less than given Value b
   */
  bool operator< (const Value& a, const Value& b) noexcept;


  /**
   * \brief weak order equality
   *
   * Compares only the stored value and ignores type information.
   *
   * \param a first value to compare
   * \param b second value to compare
   * \return the comparison result
   */
  bool weak_eq (const Value& a, const Value& b) noexcept;

  /**
   * \brief weak order less than
   *
   * Compares only the stored value and ignores type information.
   *
   * \param a first value to compare
   * \param b second value to compare
   * \return the comparison result
   */
  bool weak_lt (const Value& a, const Value& b) noexcept;

  /**
   * \brief Value specialized swap function
   *
   *  Independent of the type, a Value is always swapable.
   *  This can be theoretical be abused to bypass the type checking,
   *  but is up to the user to do so or not.
   *
   *  \param a first value to swap with second value
   *  \param b second value to swap with first value
   */
  void swap (Value& a, Value& b) noexcept;

  /// Define a constant for a Value that is null
  static const Value NullValue{};
}

#endif
