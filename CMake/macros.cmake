

if (NOT have_myMacros)

    # this is a debug function
    macro(printvars)
    set(list_var "${ARGN}")
        foreach(arg IN LISTS list_var)
            message(STATUS "*var ${arg}:   ${${arg}}")
        endforeach()
    endmacro(printvars)
                
    

    MACRO(PREFIX_WITH pre what)
        #message(STATUS ${what})
        SET(tmp "")
        FOREACH(element ${${what}})
        	SET(tmp ${pre}${element} ${tmp})
        ENDFOREACH(element)
        SET(${what} ${tmp})
    ENDMACRO(PREFIX_WITH)
    
    
    MACRO(PREFIX_COMPILER_DEFINES deflist)

        if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
            set(perfixd "-D")    
        elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
            set(perfixd "-D")
        elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang")
            set(perfixd "-D")
        elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
            set(perfixd "/D")
        else()

        endif()
                
        PREFIX_WITH(${perfixd} ${deflist})  
        
    ENDMACRO(PREFIX_COMPILER_DEFINES)


    set( have_myMacros ON )

endif (NOT have_myMacros)




