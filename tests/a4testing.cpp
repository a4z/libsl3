/*
--------------Copyright (c) 2009-2015 H a r a l d  A c h i t z---------------
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


// want to extend help text, so use own main func
#define BOOST_TEST_NO_MAIN 1
#define BOOST_TEST_ALTERNATIVE_INIT_API 1


//let user decide what to use (lib/single)
#ifdef BOOST_UNIT_TEST_FRAMEWORK_NO_LIB 
//single header
#include <boost/test/included/unit_test.hpp>

#else
// libversion
#include <boost/test/unit_test.hpp>

#endif

#include <boost/version.hpp>


#include "a4testing.hpp"

#include <set>
#include <map>
#include <tuple>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>



// for the extensions I need some 'global' states 
// on_suites are forthe --on switch, 
// off_suites are for the --off and the default disabled suites
// all_suites is the list of all suites
// xargs is for passing extra arguments to a list that can be accessed from tests.



namespace a4test {

std::set<std::string>& on_suites()
{
  static std::set<std::string> names; 
  return names; 
}

std::set<std::string>& off_suites()
{
  static std::set<std::string> names; 
  return names; 
}

std::vector<std::shared_ptr<SuiteBase>>& all_suites()
{
  static std::vector<std::shared_ptr<SuiteBase>> suites; 
  return suites; 
}

std::map<std::string, std::string>& xArgs()
{
  static std::map<std::string, std::string> xargs; 
  return xargs;
}

std::string xarg(const std::string& key)
{
  auto iter = xArgs().find(key);
  if( iter == std::end(xArgs()) )
    return "";
  
  return iter->second ; 
}

using  FreeTest = std::pair<std::string, std::function<void()>>;
using FreeTests = std::vector<FreeTest>;

FreeTests& free_tests()
{
  static FreeTests fts;
  return fts;
}

// testing implementation start


SuiteBase::SuiteBase ( const std::string name, bool onoff )
  : _suite ( nullptr )
  , _name ( name )
{
  if( onoff == false )
     off_suites().insert(name) ;
}

auto
SuiteBase::createSuite() -> boostTestSuite*
{  
  if ( _suite==nullptr )
    {
#if BOOST_VERSION >= 105900
       _suite = new boost::unit_test::test_suite(_name, __FILE__ ,__LINE__);
#else
       _suite = new boost::unit_test::test_suite(_name);
#endif

       AddCaseCall acc = [=](const std::string name, 
                            TestFunction tc, 
                            unsigned expected_failures, 
                            unsigned timeout){
#if BOOST_VERSION >= 105900
          (void)timeout;      
          _suite->add(boost::unit_test::make_test_case(tc, name, __FILE__, __LINE__) , expected_failures, 0 );
#else
          _suite->add(boost::unit_test::make_test_case(tc, name), expected_failures, timeout);
#endif
       };
      
       AddSubSuiteCall ass = [=]( boostTestSuite* bts, unsigned timeout)
       {
         _suite->add(bts, timeout);
       };
       
       assambleSuite( acc, ass ); // will also add child suit if there are some so change name ? 
      
    }

  return _suite;
}



RegisterSuite::RegisterSuite( const std::string& name, std::function<void()> test )
{
  
  free_tests().push_back(std::make_pair(name,test));
}

void RegisterSuite::add_global ( std::shared_ptr<SuiteBase> s )
{
   all_suites().push_back(s);
}


} // ns a4test
// testing implementation end


// helper for processing addtional cmd parameter extension
std::tuple<bool, std::set<std::string>>
check_optarg(const std::string& optname, const std::string& arg)
{   
  std::tuple<bool, std::set<std::string>> retval {false, std::set<std::string>()};

  if( arg.substr( 0, optname.size() ).compare( optname ) == 0 )
    {
      std::get<0>( retval ) = true;

      //only if not just --print or --help but --on="what ever"
      if( arg.find("=", optname.size() ) !=  std::string::npos ) 
        {
          using namespace boost::algorithm;
          std::string argval = trim_copy( arg.substr( optname.size() + 1, std::string::npos ) );
          std::vector<std::string> strvals; 
          split( strvals, argval, is_any_of( ",;" ), token_compress_on );
          ///split( std::get<1>( retval ), argval, is_any_of( ",;" ), token_compress_on );
          for(auto& s : strvals ) trim(s);
          std::get<1>( retval ).insert(strvals.begin(), strvals.end()) ;
        }
    }

  return retval;
}


void printHelp()
{
    std::cout << "\n" ;
    std::cout << "\nIn addition to the boost flags following options exist:" ;
    std::cout << "\n";
    std::cout << "\n  --on=\"list,of,suites\"" ;
    std::cout << "\n\tonly create and run the given suites in passed order" ;
    std::cout << "\n";
    std::cout << "\n  --off=\"list,of,suites\"" ;
    std::cout << "\n\tcreate and run all suites but not passed ones";
    std::cout << "\n";
//    std::cout << "\n  --deflagoff=\"list,of,suites\"" ;
//    std::cout <<  "\n\tcreate and run testsuites that are disabled by default" ;
//    std::cout << "\n";
    std::cout << "\n --xarg_name=value";
    std::cout << "\n pass argument 'name' with given value to the test suites";
    std::cout << "\n  --xarg_foo=\"bar\" will pass the variable foo with value bar";
    std::cout << "\n";
    std::cout << "\n  --print";
    std::cout << "\n\tprint available testsuite names" ; 
    std::cout << "\n";
    std::cout << std::endl;  
}




// main ...

#ifdef BOOST_TEST_ALTERNATIVE_INIT_API
bool init_unit_test()
{
  return true;
}
#else
::boost::unit_test::test_suite* 
init_unit_test( int, char* [] )
{
  return 0;
}
#endif


void buildSuites(boost::unit_test::master_test_suite_t& master)
{
 
  using namespace a4test ; 
  
  if( on_suites().find("*") != on_suites().end() ){
    on_suites().clear();
    off_suites().clear();
  }
  
  for( auto& s : all_suites() )
    {
      if( on_suites().empty() )
        {
          if( off_suites().find( s->getName() ) == std::end( off_suites() ) ){
           // std::cerr << "create suite " <<  s->getName() << "\n";
            master.add( s->createSuite() );            
          }
        }
      else
        {
          if( on_suites().find( s->getName() ) != std::end( on_suites() ) ) {
           // std::cerr << "create suite " <<  s->getName() << "\n";
            master.add( s->createSuite() );
          }
        }

    }
  
  for(auto& ft : free_tests())
  {
#if BOOST_VERSION >= 105900
    master.add(boost::unit_test::make_test_case( ft.second, ft.first , __FILE__, __LINE__));
#else
    master.add(boost::unit_test::make_test_case( ft.second, ft.first));
#endif
  }
}



int
main ( int argc, char* argv[] )
{

  using namespace a4test ; 

  bool print = false;
  bool show_help_long =  false ;
  bool show_help_short =  false ;

  for( int i = 1; i < argc; ++i ) // move this to a setup function thtat includes all required stuff...
    {
      bool haveopt ;
      std::set<std::string> optvals;

      std::tie( haveopt, optvals ) = check_optarg( "--on", argv[i] );
      if( haveopt )
        {
          on_suites().insert(optvals.begin(), optvals.end()) ;
          continue ;
        }

      std::tie( haveopt, optvals ) = check_optarg( "--off", argv[i] );
      if( haveopt )
        {
          off_suites().insert(optvals.begin(), optvals.end());
          continue ;
        }
      std::tie( haveopt, optvals ) = check_optarg( "--print", argv[i] );
      if( haveopt )
        {
          print = true ;
          break ;
        }

      std::tie( haveopt, optvals ) = check_optarg( "--help", argv[i] );
      if( haveopt )
        {
          show_help_long = true ;
          break ;
        }

      std::tie( haveopt, optvals ) = check_optarg( "-h", argv[i] );
      if( haveopt )
        {
          show_help_short = true ;
          break ;
        }

      std::tie( haveopt, optvals ) = check_optarg( "--xarg_", argv[i] );
      if( haveopt )
        {
          std::string name_val =  
          boost::algorithm::erase_head_copy(std::string(argv[i]), std::string("--xarg_").size()) ;
          
          std::size_t pos = name_val.find_first_of( '=' ) ;
          if( pos==std::string::npos || pos < 1 )
            {
              std::cerr << "ignore " << name_val << "\n";
              std::cerr << "use: --xarg_name=value\n";
              continue;
            }

          std::string argname = name_val.substr(0, pos) ;
          std::string argval =  name_val.substr( pos+1, std::string::npos );
          
          auto r = xArgs().insert( std::make_pair( std::move( argname ), std::move( argval ) ) ) ;
          if( !r.second )
            std::cerr << "ignore " << argname << ", name " << argname <<" already in use\n";

          continue ; 
        }

    }

  if( print )
    {
      std::set<std::string> names;
      for( auto& s : all_suites() )
        {
          if( off_suites().find( s->getName() ) != off_suites().end() )
            names.insert( s->getName() + " (default off)" ) ;
          else
            names.insert( s->getName() ) ;
        }
      
      std::cout << "available suites:\n\n" ;
      for( auto& n : names )
        std::cout << "  " << n << "\n" ;
        
      std::cout << std::endl; 
      return 0 ;
    }

  if( show_help_short )
    {
      printHelp();
      return 0 ;
    }

  boost::unit_test::master_test_suite_t& master =
    boost::unit_test::framework::master_test_suite();

  master.p_name.value = "test root";

  buildSuites(master);
  
  
  //for( auto& s : xArgs() ) std::cerr << "xa " << s.first << " = " << s.second << "\n" ;

  int result = ::boost::unit_test::unit_test_main( &init_unit_test, argc, argv );;

  if( show_help_long ) //append my help to boost help ..
    printHelp();


  return result;
}




