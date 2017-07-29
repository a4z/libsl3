/******************************************************************************
 ------------- Copyright (c) 2009-2017 H a r a l d  A c h i t z ---------------
 ---------- < h a r a l d dot a c h i t z at g m a i l dot c o m > ------------
 ---- This Source Code Form is subject to the terms of the Mozilla Public -----
 ---- License, v. 2.0. If a copy of the MPL was not distributed with this -----
 ---------- file, You can obtain one at http://mozilla.org/MPL/2.0/. ----------
 ******************************************************************************/

#include <sl3/dataset.hpp>

#include <sqlite3.h>

#include <algorithm>
#include <iterator>
#include <sl3/error.hpp>
#include <stdexcept>

namespace sl3
{
  Dataset::Dataset () noexcept : _fieldtypes (), _names () {}
  Dataset::Dataset (Types types)
  : _fieldtypes (std::move (types))
  , _names ()
  {
  }

  using namespace std;

  Dataset::Dataset (Dataset&& other) noexcept (
      std::is_nothrow_move_constructible<Container<DbValues>>::value&&
          std::is_nothrow_move_constructible<Types>::value&& std::
              is_nothrow_move_constructible<std::vector<std::string>>::value)
  : Container<std::vector<DbValues>> (std::move (other))
  , _fieldtypes (move (other._fieldtypes))
  , _names (move (other._names))
  {
  }

  void
  Dataset::reset ()
  {
    _names.clear ();
    _cont.clear ();
  }

  void
  Dataset::reset (const Types& types)
  {
    _fieldtypes = types;
    reset ();
  }

  void
  Dataset::merge (const Dataset& other)
  {
    if (!other._names.empty ())
      {
        if (!_names.empty () && _names != other._names)
          throw ErrTypeMisMatch ();
      }

    if (_fieldtypes.size () != other._fieldtypes.size ())
      throw ErrTypeMisMatch ();

    for (std::size_t i = 0; i < _fieldtypes.size (); ++i)
      {
        if (_fieldtypes[i] != Type::Variant)
          {
            if (_fieldtypes[i] != other._fieldtypes[i])
              throw ErrTypeMisMatch ();
          }
      }

    _cont.insert (_cont.end (), other._cont.begin (), other._cont.end ());
  }

  void
  Dataset::merge (const DbValues& row)
  {
    if (_fieldtypes.size () > 0 && _fieldtypes.size () != row.size ())
      {
        throw ErrTypeMisMatch ();
      }

    for (std::size_t i = 0; i < _fieldtypes.size (); ++i)
      {
        if (_fieldtypes[i] != Type::Variant)
          {
            if (_fieldtypes[i] != row[i].getType ())
              {
                throw ErrTypeMisMatch ();
              }
          }
      }

    _cont.push_back (DbValues (row));
  }

  size_t
  Dataset::getIndex (const string& name) const
  {
    using namespace std;
    auto pos = find (_names.begin (), _names.end (), name);
    if (pos == _names.end ())
      throw ErrOutOfRange ("Field name " + name + " not found");

    return distance (_names.begin (), pos);
  }

  void
  Dataset::sort (const std::vector<size_t>& idxs, DbValueSort cmp)
  {
    ASSERT_EXCEPT(cmp, ErrNullValueAccess) ;

    auto lessValues = [&](const DbValues& a, const DbValues& b) -> bool {
      for (auto cur : idxs)
        {
          if (cmp (a.at (cur), b.at (cur)))
            return true;
          else if (cmp (b.at (cur),a.at (cur)))
            return false;
        }
      return false;
    };

    std::sort (begin (), end (), lessValues);
  }
}
