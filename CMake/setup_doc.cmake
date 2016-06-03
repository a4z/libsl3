

if (NOT have_myDocSetup)

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
    
    
    set(have_myDocSetup ON BOOL)    

endif (NOT have_myDocSetup)
