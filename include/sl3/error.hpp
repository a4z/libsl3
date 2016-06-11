/******************************************************************************
 ------------- Copyright (c) 2009-2016 H a r a l d  A c h i t z ---------------
 ---------- < h a r a l d dot a c h i t z at g m a i l dot c o m > ------------
 ---- This Source Code Form is subject to the terms of the Mozilla Public -----
 ---- License, v. 2.0. If a copy of the MPL was not distributed with this -----
 ---------- file, You can obtain one at http://mozilla.org/MPL/2.0/. ----------
 ******************************************************************************/

#ifndef SL3__ERROR_HPP_
#define SL3__ERROR_HPP_


#include <iosfwd>
#include <string>

#include <sl3/config.hpp>

namespace sl3{


  /**
   * \brief Error codes
   *
   * These enum values used to create ErrType objects.
   *
   * Each ErrCode becomes an ErrType object.
   *
   */
  enum class ErrCode
  {
    SQL3Error = 1 , ///< sqlite3 error happened
    NoConnection = 2 , ///< no database
    OutOfRange = 5 ,///< index op out of range
    TypeMisMatch = 6 ,///< type cast problem
    NullValueAccess = 7 , ///< accessing a value that is Null
    UNEXPECTED = 99 ///< for everything that happens unexpected
  };


  /**
   * \brief Exception base type
   *
   * A command base for all ErrType objects.
   * All exceptions thrown by the library wil have this base type.
   */
  class LIBSL3_API Error
  {
  protected:

    /**
     * \brief c'tor
     *
     * \param id the ErrCode
     * \param info info text
     */
    Error(ErrCode id, std::string info ) ;

  public:

#ifdef _MSC_VER
    virtual ~Error() = default ;
#else
    /// default d'tor
    virtual ~Error() noexcept = default ;
#endif

    /**
      * \brief Get ErrCode
      * \return the Errcode of the excetion
      */
      ErrCode getId () const  ;

     /**
     * \brief Get info text
     * \return reference to the info string
     */
    const std::string& info () const ;

  protected:

    friend std::ostream& operator<<(std::ostream& os, const Error& e);

    /**
     * \brief Write to stream
     *
     *  Called from the stream operator, allows to customize
     *  stream output.
     *  \param os the ostrem
     */
    virtual void toStream(std::ostream& os) const;

  private:
    ErrCode     _id;
    std::string _info;
  };

  /**
   * \brief overloaded stream operator
   * \param os ostream
   * \param e the Error
   * \return the ostream
   */
  std::ostream& LIBSL3_API operator<< (std::ostream& os, const Error& e);


  /**
   * \brief Object class representing an ErrCode
   *
   *  Allows typedef objects using an Errcode.
   *  Each sl3::ErrCode becomes an ErrType object.
   */
  template<ErrCode ec>
  class LIBSL3_API ErrType: public Error
  {

  public:
    /**
     * \brief c'tor
     * \param msg error message
     */
    ErrType (std::string msg = "" )
      : Error (ec, std::move (msg))
    {
    }

#ifdef _MSC_VER
    virtual ~ErrType () = default ;
#else
    virtual ~ErrType () noexcept = default ;
#endif

  };

  /// error that will be thrown if no open database was available but needed
  using ErrNoConnection = ErrType<ErrCode::NoConnection> ;

  ///thrown if an index op is out of range
  using ErrOutOfRange = ErrType<ErrCode::OutOfRange> ;

  /**
   * \brief thrown in case of an incorrect type usage
   */
  using ErrTypeMisMatch = ErrType<ErrCode::TypeMisMatch>;

  ///thrown in case of accessing a Null value field/parameter
  using ErrNullValueAccess = ErrType<ErrCode::NullValueAccess> ;

  ///thrown if something unexpected happened, mostly used by test tools and in debug mode
  using ErrUnexpected =  ErrType<ErrCode::UNEXPECTED>;

  /**
   *  \brief throw an ErrUnexpected exception.
   *
   * can be used on location where the comiler should not see that
   * an exception is raised.
   * \param msg optional message
   */
  void raiseErrUnexpected (const std::string& msg = "") ;

  /**
   * \brief packaging an error from sqlite3.
   * This exception will be thrown if sqlite3 reports an error.
   *
   * Holds the sqlite3 error code and the error message if it is available.
   * \see Database::getMostRecentErrMsg
   * \see Database::getMostRecentErrCode
   *
   * \n Extended error codes can be obtained through Database class.
   * \see Database::getMostRecentExtendedErrCode
   * \see Database::enableExtendedResultCodes
   *
   */
  class LIBSL3_API SQLite3Error : public ErrType<ErrCode::SQL3Error>
  {

  public:
    /**
     * \brief c'tor
     * \param sl3ec sqite error code
     * \param sl3msg sqite error code
     */
      SQLite3Error (int sl3ec, const char* sl3msg);

      /**
       * \brief c'tor
       * \param sl3ec sqite error code
       * \param sl3msg sqite error code
       * \param msg additional message
       */
      SQLite3Error (int sl3ec, const char* sl3msg, const std::string& msg);

#ifdef _MSC_VER
    virtual ~SQLite3Error () = default ;
#else
    virtual ~SQLite3Error () noexcept = default ;
#endif


      /**
       * \brief  Get the sqlite3 error code
       * \return the sqlite3 error code
       */
      int SQLiteErrorCode () const ;

      /**
       * \brief  Get the sqlite3 error message.
       *
       * If sqlite has a error message to the current error
       *
       * \return the sqlite3 error message
       */
      const std::string& SQLiteErrorMessage() const ;

  private:
      virtual void toStream(std::ostream& os)  const override ;

      int m_sqlite_ec;
      std::string m_sqlite_msg;
  };


#define  ASSERT_EXCEPT( exp , except ) if ( !( exp ) ) \
  throw  except( std::string(__FUNCTION__) + ": " + #exp )


} // ns


#endif /* ...ERROR_HPP_ */
