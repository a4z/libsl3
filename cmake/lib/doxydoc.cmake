include_guard(GLOBAL)

find_package( Doxygen QUIET )
if(NOT DOXYGEN_FOUND)
    message(STATUS "Doxygen not found, doc target not available")
    return()
endif()

include(FetchContent)
FetchContent_Declare(
    doxygen-awesome-css
    URL https://github.com/jothepro/doxygen-awesome-css/archive/refs/tags/v2.4.0.zip
)
FetchContent_MakeAvailable(doxygen-awesome-css)

FetchContent_GetProperties(doxygen-awesome-css SOURCE_DIR AWESOME_CSS_DIR)

if(DOT)
    set(HAVE_DOT YES)
else(DOT)
    set(HAVE_DOT NO)
endif(DOT)

configure_file(${PROJECT_SOURCE_DIR}/doc/doxyfile.cmake
    ${PROJECT_BINARY_DIR}/doc/${PROJECT_NAME}.doxyfile
)

#todo, naming, would like to have doxydoc in all projects
#add_custom_target(doc${PROJECT_NAME} ${DOXYGEN}
add_custom_target(doc ${DOXYGEN}
    ${PROJECT_BINARY_DIR}/doc/${PROJECT_NAME}.doxyfile
)
