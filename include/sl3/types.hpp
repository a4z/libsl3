/******************************************************************************
 ------------- Copyright (c) 2009-2017 H a r a l d  A c h i t z ---------------
 ---------- < h a r a l d dot a c h i t z at g m a i l dot c o m > ------------
 ---- This Source Code Form is subject to the terms of the Mozilla Public -----
 ---- License, v. 2.0. If a copy of the MPL was not distributed with this -----
 ---------- file, You can obtain one at http://mozilla.org/MPL/2.0/. ----------
 ******************************************************************************/

#ifndef SL3_TYPES_HPP_
#define SL3_TYPES_HPP_

#include <cstddef>
#include <iosfwd>
#include <string>
#include <vector>

#include <sl3/config.hpp>
#include <sl3/container.hpp>

namespace sl3
{
  /**
   * Enumeration of different value types
   *
   * These are the types known by sqlite.
   * They are used when reading from or writing to a database.
   *
   */
  enum class Type
  {
    Null    = 0, //!< Null, no value
    Int     = 1, //!< Int value
    Real    = 2, //!< Real value
    Text    = 3, //!< Text value
    Blob    = 4, //!< Blob vale
    Variant = 5  //!< takes any type
  };

  /**
   * \brief Get the type name as string
   *
   * For example, in log messages a type "Real" is more verbose that a type 2.
   *
   * \return the type name as string
   */
  std::string typeName (Type);

  /**
   * \brief overloaded stream operator for sl3::Type
   * \param os ostream
   * \param t the Type
   * \return the ostream
   */
  std::ostream& LIBSL3_API operator<< (std::ostream& os, const Type& t);

  /**
   * \brief A Container holding sl3::Type values.
   *
   *  A fixed size list of sl3::Type values.
   */
  class LIBSL3_API Types : public Container<std::vector<Type>>
  {
  public:
    //@{
    using conatiner_type  = Container::conatiner_type;
    using iterator        = conatiner_type::iterator;
    using const_iterator  = conatiner_type::const_iterator;
    using value_type      = conatiner_type::value_type;
    using reference       = conatiner_type::reference;
    using const_reference = conatiner_type::const_reference;
    using size_type       = conatiner_type::size_type;
//@}

#ifndef _MSC_VER
    using Container::Container;

#else
    /**
     * \brief c'tor
     *
     * Create an empty sl3::Type Container
     */
    Types () noexcept {}
    using Base = Container<std::vector<Type>>;

    /**
     * \brief c'tor
     *
     * Create a container with given vector of sl3::Type elements.
     *
     * \param c vector of sl3::Type
     */
    Types (conatiner_type c) noexcept (
        std::is_nothrow_move_constructible<conatiner_type>::value)
    : Base (std::move (c))
    {
    }

    /**
     * \brief c'tor
     *
     * Create a container with given initializer list of sl3::Type elements.
     *
     * \param l initialer list of sl3::Type
     */
    Types (std::initializer_list<typename conatiner_type::value_type> l)
    : Base (std::move (l))
    {
    }

#endif

    /**
     * \brief Swap container
     *
     *  Uses standard swap to change the  contents.
     *
     * \param other Types to swap with
     */
    void swap (Types& other) noexcept;
  };

  /**
   * A type for binary data
   */
  using Blob = std::vector<std::byte>;
}

#endif
