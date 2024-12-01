
include_guard(GLOBAL)

message(STATUS "-- Activate toolchain linux-clang.cmake")

set(CMAKE_C_COMPILER /usr/bin/clang)
set(CMAKE_CXX_COMPILER /usr/bin/clang++)

# Specify the path to the Clang linker
set(CMAKE_LINKER /usr/bin/clang++)
#set(CMAKE_LINKER /usr/bin/lld)
set(CMAKE_AR /usr/bin/llvm-ar)
set(CMAKE_RANLIB /usr/bin/llvm-ranlib)
set(CMAKE_NM /usr/bin/llvm-nm)
set(CMAKE_OBJCOPY /usr/bin/llvm-objcopy)
set(CMAKE_OBJDUMP /usr/bin/llvm-objdump)
set(CMAKE_STRIP /usr/bin/llvm-strip)
