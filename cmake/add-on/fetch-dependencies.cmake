
include(FetchContent)
FetchContent_Declare(
  commonCompilerWarnings
  GIT_REPOSITORY https://github.com/a4z/commonCompilerWarnings.git
  GIT_TAG main
  OVERRIDE_FIND_PACKAGE
)

FetchContent_Declare(
  doctest
  SYSTEM
  GIT_REPOSITORY "https://github.com/onqtam/doctest"
  GIT_TAG "v2.4.12"
  OVERRIDE_FIND_PACKAGE
)

set(DOCTEST_NO_INSTALL TRUE)

set(commonCompilerWarnings_NO_INSTALL TRUE)
# now with this problem solved, we probably should always fetch that internally
# so for users, this just works
