/******************************************************************************
 -------------- Copyright (c) 2009-2015 H a r a l d  A c h i t z --------------
 ---------- < h a r a l d dot a c h i t z at g m a i l dot c o m > ------------
 ---- This Source Code Form is subject to the terms of the Mozilla Public -----
 ---- License, v. 2.0. If a copy of the MPL was not distributed with this -----
 ---------- file, You can obtain one at http://mozilla.org/MPL/2.0/. ----------
 ******************************************************************************/

#ifndef SL3_RowCallback_HPP
#define SL3_RowCallback_HPP


#include <functional>


namespace sl3 {

  class Columns ;

  /**
   * \brief Callback for SQL Select statements
   *
   * This interface can be used to process through the result of
   * a SQL statement.
   *
   *
   */
  class RowCallback
  {

      friend class Command;

    protected:

      /**
       * \brief Constructor
       */
      RowCallback () noexcept = default ;

      /**
       * \brief Constructor
       */
      virtual ~RowCallback () noexcept = default ;

      /**
       * \brief Process one row of the result from  a SELECT statement
       *
       * @param columns Columns object for accessing values.
       *
       * @return false if processing the query result shall stop
       *       \n true otherwise
       */
      virtual bool onRow (Columns columns) = 0;

      /**
       * \brief Called before a query result will be processed
       *
       * The default implementation does nothing.
       */
      virtual void onStart ();

      /**
      * \brief Called after a query result has been processed
      *
      * The default implementation does nothing.
      */
      virtual void onEnd ();


  };






}



#endif
