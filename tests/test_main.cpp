// #define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
// #include "doctest.h"

#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest/doctest.h"

int
main (int argc, char** argv)
{
  // environment setup give me the code that should stand here

  doctest::Context context; // initialize

  // defaults
  //    context.addFilter("test-case-exclude", "*math*"); // exclude test cases
  //    with "math" in their name
  context.setOption ("no-breaks",
                     true); // don't break in the debugger when assertions fail

  context.applyCommandLine (argc, argv);

  // overrides
  context.setOption ("abort-after",
                     5); // stop test execution after 5 failed assertions
  // context.setOption("sort", "name");   // sort the test cases by their name

  int res = context.run (); // run

  if (context.shouldExit ()) // important - query flags (and --exit) rely on
                             // the user doing this
    return res;              // propagate the result of the tests

  int client_stuff_return_code = 0;
  // your program - if the testing framework is integrated in your production
  // code

  return res + client_stuff_return_code; // the result from doctest is
                                         // propagated here as well
}
