include_guard(GLOBAL)

include(FetchContent)
FetchContent_Declare(
    doxygen-awesome-css
    URL https://github.com/jothepro/doxygen-awesome-css/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(doxygen-awesome-css)

FetchContent_GetProperties(doxygen-awesome-css SOURCE_DIR AWESOME_CSS_DIR)

option(BUILD_DOCUMENTATION "make Doxygen documentation" ON)
#das sollt da raus weil ja nur die option gefragt, die action jedoch includiert sein soll
if(BUILD_DOCUMENTATION)

    find_package( Doxygen )

    if(DOXYGEN_FOUND)

        if(DOT)
            set(HAVE_DOT YES)
        else(DOT)
            set(HAVE_DOT NO)
        endif(DOT)

# Generate the Doxyfile
        # set(DOXYFILE_IN ${CMAKE_SOURCE_DIR}/doc/Doxyfile.in)
        # set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
        # configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)


        configure_file(${PROJECT_SOURCE_DIR}/doc/doxyfile.cmake
            ${PROJECT_BINARY_DIR}/doc/${PROJECT_NAME}.doxyfile

        )

        #todo, naming, would like to have doxydoc in all projects
        #add_custom_target(doc${PROJECT_NAME} ${DOXYGEN}
        add_custom_target(doc ${DOXYGEN}
            ${PROJECT_BINARY_DIR}/doc/${PROJECT_NAME}.doxyfile
        )

    endif(DOXYGEN_FOUND)

endif(BUILD_DOCUMENTATION)
