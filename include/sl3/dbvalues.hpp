/******************************************************************************
 ------------- Copyright (c) 2009-2023 H a r a l d  A c h i t z ---------------
 ---------- < h a r a l d dot a c h i t z at g m a i l dot c o m > ------------
 ---- This Source Code Form is subject to the terms of the Mozilla Public -----
 ---- License, v. 2.0. If a copy of the MPL was not distributed with this -----
 ---------- file, You can obtain one at http://mozilla.org/MPL/2.0/. ----------
 ******************************************************************************/

#ifndef SL3_DbVALUES_HPP_
#define SL3_DbVALUES_HPP_

#include <initializer_list>

#include <sl3/config.hpp>
#include <sl3/container.hpp>
#include <sl3/dbvalue.hpp>

namespace sl3
{
  /**
   * \brief A row of DbValues
   *
   * Fixed size vector of DbValue instances.
   * DbValues models a row in a database, therefore assignment works only if
   * fields are compatible.
   *
   *
   */
  class LIBSL3_API DbValues final : public Container<std::vector<DbValue>>
  {
  public:
    //@{
    /// usual typedefs
    using conatiner_type  = Container::conatiner_type;
    using iterator        = conatiner_type::iterator;
    using const_iterator  = conatiner_type::const_iterator;
    using value_type      = conatiner_type::value_type;
    using reference       = conatiner_type::reference;
    using const_reference = conatiner_type::const_reference;
    using size_type       = conatiner_type::size_type;
    //@}

    /// Constructor
    DbValues () noexcept;

#ifndef _MSC_VER
    using Container::Container;

#else

    DbValues (conatiner_type) noexcept (
        std::is_nothrow_move_constructible<DbValue>::value);

    DbValues (std::initializer_list<typename conatiner_type::value_type>);

#endif

    /** \brief Copy constructor
     */
    DbValues (const DbValues&);

    /** \brief Move constructor
     */
    DbValues (DbValues&&) noexcept (
        std::is_nothrow_move_constructible<DbValue>::value);

    /** \brief Assigment
     *  \throw sl3::ErrTypeMisMatch if size() is different
     *  \see assignment of DbValue
     *  \return reference ot this
     */
    DbValues& operator= (const DbValues&);

    /** \brief Assigment
     *  \throw sl3::ErrTypeMisMatch if size() is different
     *  \see assignment of DbValue
     *  \return reference ot this
     */
    DbValues& operator= (DbValues&&);

    /**
     * \brief swap function
     *
     * Swaps  content of 2 DbValues.
     *
     * \param other DbValues to swap wit
     */
    void swap (DbValues& other) noexcept;
  };

  /**
   * \brief DbValue specialized swap function
   *
   *  \param a first value to swap with second value
   *  \param b second value to swap with first value
   *
   *  This function call a.swap(b). \see DbValues::swap
   */
  void swap (DbValues& a, DbValues& b) noexcept;

}

#endif
