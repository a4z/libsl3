/******************************************************************************
 ------------- Copyright (c) 2009-2016 H a r a l d  A c h i t z ---------------
 ---------- < h a r a l d dot a c h i t z at g m a i l dot c o m > ------------
 ---- This Source Code Form is subject to the terms of the Mozilla Public -----
 ---- License, v. 2.0. If a copy of the MPL was not distributed with this -----
 ---------- file, You can obtain one at http://mozilla.org/MPL/2.0/. ----------
 ******************************************************************************/

#ifndef SL3_DATASET_HPP_
#define SL3_DATASET_HPP_

#include <vector>
#include <map>

#include <sl3/dbvalues.hpp>
#include <sl3/error.hpp>
#include <sl3/rowcallback.hpp>

namespace sl3{



/**
 * \brief A utility for processing the result queries.
 *
 * This class is a RowCallback that loads the result of a Query into a list
 *  of DbValues.
 * The loaded list can be browsed and the loaded values can be accessed.
 *
 * A Dataset can either be created:
 *  - giving a DbValuesTypeList that describes the fields \n
 *   Giving a DbValuesTypeList the types of DbValue will use the given storage
 *   type when reading
 *   the data from sqlite. \n
 *   When the Dataset becomes populated with data the field count will be
 *   validated.
 *   In case the number of fields is different sl3::ErrTypeMisMatch will
 *   be thrown.
 *
 *  - Without any specification \n
 *  In this case all fields will be DsVariantField ,
 *  using the storage type sqlite reports for the actual value.
 *
 *
 *
 */
class Dataset final: public Container<std::vector<DbValues>>
{

  friend class Command;

public:

  /**
   * \brief Constructor
   *
   * All fields will be DsVariantField , using the storage type sqlite reports for the actual value.
   * Field count will be detected and applied.
   */
  Dataset() noexcept;

  /**
   * \brief Constructor wiht DbValuesTypeList as description
   *
   * Types of DbValue will use the given description when creating the
   *  DbValue list.
   * If the given list is not empty, field count will be validated when
   * the actual instance becomes populated with data.
   * \param types Types the fields must satisfy
   */
  Dataset(Types types);

  /**
   * \brief Copy Constructor
   */
  Dataset(const Dataset&) = default;


  /**
   * \brief Move Constructor
   */
  Dataset(Dataset&& ) noexcept (
         std::is_nothrow_move_constructible<Container<DbValues>>::value &&
         std::is_nothrow_move_constructible<Types>::value &&
         std::is_nothrow_move_constructible<std::vector<std::string>>::value);
       //  = default; no mscv does not like it



  /**
   * \brief Value assignment
   * \return reference to this
   */
  Dataset& operator=(const Dataset& ) = default ;

  /**
   * \brief Rvalues assignment
   * \return reference to this
   */
  Dataset& operator=(Dataset&& ) = default;


  /**
   * \brief Clear all states.
   * Removes loaded data so that the actual instance can be reused/refilled.
   *
   */
  void reset();


  /**
   * \brief Clear all states.
   *
   * Removes loaded data and sets a new specification for the field description
   *  so that the actual instance can be reused for populate with a different
   *  select statement / sql command.
   * Passing an empty DbValuesTypeList mean that all fields will be
   * DsVariandField and field count will be detected.
   *
   * \param types new Types requirement
   */
  void reset(const Types& types);


  /**
   * \brief Merge an other Dataset.
   *
   * Appends the data of the given Dataset to the end of the actual data.
   * The field names and types of the given Dataset must match the actuals one.
   *
   * \throw sl3::ErrTypeMisMatch if field names types are not equal or
   * size differs.
   *
   * \param other Dataset which shall be added to this one.
   */
  void merge(const Dataset& other) ;


  /**
   * \brief Merge DbValues.
   *
   * Appends the DbValues to the end of the actual data.
   *
   * \throw sl3::ErrTypeMisMatch if size differs from existing row size or
   * if types are not compatible
   *
   * \param  row A row which shall be added.
   */
  void merge(const DbValues& row) ;


  /**
   * \brief Get the index of a field by namespace
   *
   * \throw sl3::OutOfRanage if name is not found
   * \param name field name
   * \return field index
   */
  std::size_t getIndex(const std::string& name) const;

private:

  Types _fieldtypes;
  std::vector<std::string>_names;


};

}

#endif
