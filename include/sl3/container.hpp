/******************************************************************************
 ------------- Copyright (c) 2009-2016 H a r a l d  A c h i t z ---------------
 ---------- < h a r a l d dot a c h i t z at g m a i l dot c o m > ------------
 ---- This Source Code Form is subject to the terms of the Mozilla Public -----
 ---- License, v. 2.0. If a copy of the MPL was not distributed with this -----
 ---------- file, You can obtain one at http://mozilla.org/MPL/2.0/. ----------
 ******************************************************************************/

#ifndef SL3_CONTAINER_HPP_
#define SL3_CONTAINER_HPP_


#include <sl3/error.hpp>


namespace sl3
{

/**
 * \brief Wrapper to provide begin, end and random access of a container
 *
 * \tparam ContainerType a container like for example std::vector
 *
 * A ContainerType has begin, end, random access and size.
 *
 * This class makes these methods accessible and hides the others, like a
 * push_back, erase, ...
 * A derived class can decide which other methods shall become visible.
 *
 */
template<typename ContainerType>
class Container
{
 public:


  /**
   * \brief Constructor.
   *
   * Create an empty container.
   *
   */
  Container() noexcept{}


  /**
   * \brief Constructor
   *
   * \param container values
   */
  Container(ContainerType container)
   noexcept(std::is_nothrow_move_constructible<ContainerType>::value)
   :_cont(std::move(container))
  {
  }


  /**
   * \brief Constructor
   *
   * \param container values
   */
  Container(std::initializer_list<typename ContainerType::value_type> container)
   :_cont(std::move(container))
  {
  }


  /**
   * \brief Copy Constructor
   */
  Container(const Container&) = default;


  /**
   * \brief Assignment
   * \return reference to this
   */
  Container& operator=(const Container&) = default;


  /**
   * \brief Move constructor
   */
  Container (Container&&)
    noexcept(std::is_nothrow_move_constructible<ContainerType>::value)=default;


  /**
   * \brief Move assignment
   * \return reference to this
   */
  Container& operator=(Container&&) = default;


  /**
   * \brief Destructor
   */
  virtual ~Container()
     noexcept(std::is_nothrow_destructible<ContainerType>::value) = default;

  //@{
  using conatiner_type =  ContainerType ;
  using iterator = typename conatiner_type::iterator;
  using const_iterator = typename conatiner_type::const_iterator;
  using value_type = typename conatiner_type::value_type;
  using reference = typename conatiner_type::reference;
  using const_reference = typename conatiner_type::const_reference;
  using size_type = typename conatiner_type::size_type;
  using container_type = ContainerType;
  //@}


  /**
   * \brief Iterator access
   * \return requested iterator
   */
  iterator begin() { return std::begin(_cont);}


  /**
   * \brief Iterator access
   * \return requested iterator
   */
  const_iterator begin() const { return _cont.cbegin();}


  /**
   * \brief Iterator access
   * \return requested iterator
   */
  iterator end() { return std::end(_cont);}


  /**
   * \brief Iterator access
   * \return requested iterator
   */
  const_iterator end() const { return _cont.cend();}


  /**
   * \brief Iterator access
   * \return requested iterator
   */
  const_iterator cbegin() const { return _cont.cbegin();}


  /**
   * \brief Iterator access
   * \return requested iterator
   */
  const_iterator cend() const { return _cont.cend();};


  ///

  /**
   * \brief Container size
   * \return number of elements
   */
  size_type size() const { return _cont.size();}


  /**\brief checked random access
   * \param i index
   * \throw sl3::ErrOutOfRange if index is invalid
   * \return reference to element at requested index
   */
  reference at(size_t i){
    try{
        return _cont.at(i);
    } catch(...) {
        throw ErrOutOfRange("no data at: " + std::to_string(i)) ;
    }
  }


  /** \brief checked random access
   *
   * \param i index
   * \throw sl3::ErrOutOfRange if index is invalid
   * \return const reference to element at requested index
   */
  const_reference at(size_t i) const {
    try{
        return _cont.at(i);
    } catch(...) {
        throw ErrOutOfRange("no data at: " + std::to_string(i)) ;
    }
  }


  /**\brief unchecked random access
   * \param i index
   * behaves undefined if given index is invalid
   * \return reference to element at requested index
   */
  reference operator[](size_t i) {  return _cont[i] ;}


  /**\brief unchecked random access
   * \param i index
   * behaves undefined if given index is invalid
   * \return reference to element at requested index
   */
  const_reference operator[](size_t i) const {  return _cont[i] ;}


 protected:
  /// Container  T
  ContainerType _cont;

};



}

#endif /* SL3_CONTAINER_HPP_ */
