/*
--------------Copyright (c) 2009-2017 H a r a l d  A c h i t z---------------
-----------< h a r a l d dot a c h i t z at g m a i l dot c o m >------------
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/


#ifndef A4Z_A4TESTING_
#define A4Z_A4TESTING_

#include <vector>
#include <string>
#include <functional>
#include <memory>

#include <boost/test/test_tools.hpp>


namespace boost { namespace unit_test {
  class test_suite;
}}


namespace a4test {

  static constexpr bool defaultOn{true} ;
  static constexpr bool defaultOff{false} ;
//	 static  const bool defaultOn{ true };
//	 static  const bool defaultOff{false} ;


  // helper, base to forwared boost stuff from here to the translation unit
class SuiteBase
{
  
public:
  using boostTestSuite = boost::unit_test::test_suite ;
  using TestFunction = std::function<void()> ;
  using AddCaseCall = std::function<void(const std::string&, TestFunction, unsigned , unsigned )> ; //name, function, expected_failures, timeout
  using AddSubSuiteCall = std::function<void(boostTestSuite*, unsigned)> ; //childsuite timeout
  
  SuiteBase(const std::string name_, bool onoff = defaultOn);
  
  virtual ~SuiteBase() = default;   
  
  boostTestSuite* createSuite() ; // creates the suite, calls assambleSuite 
  
  const std::string& getName() { return _name; } 
  
  
  struct NoFixtureTag{}; 
  
protected:
  
  virtual void assambleSuite(AddCaseCall addcase, AddSubSuiteCall addsubsuite) = 0;
  
  boostTestSuite* _suite; 
  const std::string _name ;   
}; //-----------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------



// helper, keep erverything that is not public interface here
template<typename SuiteFixture>
class SuiteInternals : public SuiteBase{  

protected:
  typedef void(SuiteFixture::*InstanceFunction)();
  using TestFunction = std::function<void()> ;


  // all info that is reqired to build a suite,
  // if suite will not be created, this is not needed and anin instace of the suite fixture will never be created

  std::shared_ptr<SuiteFixture> _suite_fixture ; 
  
  struct ToBind 
  {
    std::string _name;
    TestFunction _tfn; 
    InstanceFunction _ifn ;
    unsigned _expected_failures ;
    unsigned _timeout   ; 

  // store info that is reqired to build a suite,    
    ToBind(const std::string name, TestFunction tfn, InstanceFunction ifn, unsigned expected, unsigned tm)
    : _name(name)
    , _tfn(tfn)
    , _ifn(ifn)
    , _expected_failures(expected)
    , _timeout(tm)
    {
    }
    
  };
  std::vector<ToBind> _tobind;
  
  // store info for child suite
  struct ChildSuiteInfo
  {
    std::shared_ptr<SuiteBase> suite ;
    unsigned timeout ;
  };
  std::vector<ChildSuiteInfo> _child_suites ; 
  
  
  SuiteInternals(const std::string name, bool onoff = defaultOn)
  : SuiteBase(name, onoff)
  , _suite_fixture(nullptr)
  {
  }
  
  
  void assambleSuite(AddCaseCall addcase, AddSubSuiteCall addsubsuite) override
  {
    if(_suite_fixture == nullptr) 
      _suite_fixture = std::make_shared<SuiteFixture>() ;
    
    for( auto& tb : _tobind ){
      // assert ( tb._tfn || tb._ifn );
      if( tb._ifn ){
        TestFunction func = std::bind(tb._ifn, _suite_fixture);
        addcase( tb._name, func , tb._expected_failures, tb._timeout) ;        
      }
      else if( tb._tfn  )
        addcase( tb._name, tb._tfn, tb._expected_failures, tb._timeout) ;        
    }    
    
    for( auto& csi : _child_suites ){
      addsubsuite( csi.suite->createSuite(), csi.timeout );
    }
  }
  
  
  
}; //-----------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------- 



// public interface from here ....

template<typename SuiteFixture = SuiteBase::NoFixtureTag>
class Suite : public SuiteInternals<SuiteFixture>{
  
  using internals = SuiteInternals<SuiteFixture>;

  template<typename T> friend
  Suite<T> suite(const std::string&, bool onoff );

  Suite(const std::string name, bool onoff = defaultOn)
  : SuiteInternals<SuiteFixture>(name, onoff)
  {
  }


public: 
   using TestFunction = std::function<void()> ;
   typedef void(SuiteFixture::*InstanceFunction)();
   
   using this_type = Suite<SuiteFixture>; 

  
  

  this_type& addTest(const std::string& name, TestFunction tc, unsigned expected_failures = 0, unsigned timeout= 0)
  {
    internals::_tobind.emplace_back(name, tc , nullptr , expected_failures , timeout ); 
    return *this; 
  }
 
  this_type& addTest(const std::string& name, InstanceFunction tc, unsigned expected_failures = 0, unsigned timeout= 0)
  {
    internals::_tobind.emplace_back(name, nullptr , tc , expected_failures , timeout ); 
    return *this; 
  }
  
  // use child_site, or it has to be turned off otherwise it is also in the master tree
  template<typename ChildSuiteFixture>
  this_type& addSuite( Suite<ChildSuiteFixture>& child, unsigned timeout= 0)
  {
    using ChildSuiteInfo = typename internals::ChildSuiteInfo ;
    ChildSuiteInfo csi { std::make_shared<Suite<ChildSuiteFixture>>(child), timeout } ; 
    internals::_child_suites.push_back(csi);
    return *this; 
  }
  
  
}; //-----------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------- 

template<typename SuiteFixture = SuiteBase::NoFixtureTag>
Suite<SuiteFixture> suite(const std::string& name, bool onoff)
{
  return Suite<SuiteFixture>(name, onoff); 
}
 
template<typename SuiteFixture = SuiteBase::NoFixtureTag>
Suite<SuiteFixture> suite(const std::string& name)
{
  return suite<SuiteFixture>(name, true);
}


//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
// access additional command line arguments
std::string xarg(const std::string& key) ; 

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------

struct RegisterSuite{
  
  // TODO , overload for a simple function that will be added to master ...
  
  template<typename T>
  explicit RegisterSuite( Suite<T>& ts ) {
    std::shared_ptr<Suite<T>> s = std::make_shared<Suite<T>>(ts);
    add_global(s); 
  }

  
  RegisterSuite( const std::string& name, std::function<void()> test );
  
private:
  void add_global( std::shared_ptr<SuiteBase> s );
  
};

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------

}

#define a4TestCONCAT( x, y ) x##y
#define a4TestSuiteNameCONCAT( x, y ) a4TestCONCAT( x , y )
#define a4TestAdd( CODE ) static a4test::RegisterSuite a4TestSuiteNameCONCAT(test_register_ , __COUNTER__)( CODE  ) 
#define a4TestSimple( NAME, CODE ) static a4test::RegisterSuite a4TestSuiteNameCONCAT(test_register_ , __COUNTER__)( NAME , CODE  )



#endif
