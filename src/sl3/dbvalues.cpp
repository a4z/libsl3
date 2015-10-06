/******************************************************************************
 -------------- Copyright (c) 2009-2015 H a r a l d  A c h i t z --------------
 ---------- < h a r a l d dot a c h i t z at g m a i l dot c o m > ------------
 ---- This Source Code Form is subject to the terms of the Mozilla Public -----
 ---- License, v. 2.0. If a copy of the MPL was not distributed with this -----
 ---------- file, You can obtain one at http://mozilla.org/MPL/2.0/. ----------
 ******************************************************************************/

#include <sl3/dbvalues.hpp>

#include <sl3/error.hpp>


namespace sl3
{


#ifdef _MSC_VER
  using std::initializer_list ;

  DbValues::DbValues(conatiner_type c)
   noexcept(std::is_nothrow_move_constructible<conatiner_type>::value)
   :Container(std::move(c))
  {
  }


  DbValues::DbValues(initializer_list<typename conatiner_type::value_type> l)
   :Container(std::move(l))
  {
  }
#endif


  DbValues::DbValues () noexcept
  : Container ()
  {

  }


  DbValues::DbValues (const DbValues& row)
  :Container (row)
  {

  }


  DbValues::DbValues (DbValues&& row)
  :Container (std::move (row))
  {

  }


  DbValues&
  DbValues::operator= (const DbValues& row)
  {
    // in case of exception , both needs to stay unchanged
    // first all checks, than assign
    if (size () != row.size ())
      throw ErrTypeMisMatch ();

    for (size_t i = 0; i < size (); ++i)
      {
        if (!_cont[i].canAssign (row[i]))
          throw ErrTypeMisMatch ();
      }

    for (size_t i = 0; i < size (); ++i)
      {
        _cont [i].assign (row [i]) ;
      }

    return *this;
  }


  DbValues&
  DbValues::operator= (DbValues&& row)
  {
    // in case of exception , both needs to stay unchanged
    // first all checks, than assign

    if (size () != row.size ())
      throw ErrTypeMisMatch ();

    for (size_t i = 0; i < size (); ++i)
      {
        if (!_cont[i].canAssign (row[i]))
          throw ErrTypeMisMatch ();
      }

    _cont = std::move (row._cont);
    return *this;
  }


  void
  DbValues::swap(DbValues& other) noexcept
  {
    using std::swap;
    swap(_cont, other._cont);
  }


}

