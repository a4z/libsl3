

if (NOT have_myMacros)

    
    #haengt ueberall das praefix davor
    MACRO(PREFIX_WITH pre what)
        #message(STATUS ${what})
        SET(tmp "")
        FOREACH(element ${${what}})
        	SET(tmp ${pre}${element} ${tmp})
        ENDFOREACH(element)
        SET(${what} ${tmp})
    ENDMACRO(PREFIX_WITH)
    
    
    MACRO(PREFIX_COMPILER_DEFINES deflist)
            #message(STATUS ${${deflist}}) 
            if ( CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX )
                set(perfixd "-D")
            elseif( MSVC )
                set(perfixd "/D")        
            else ( CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX )
            
                if(${CMAKE_C_COMPILER} MATCHES "clang")
                    set(perfixd "-D")   
                else (${CMAKE_C_COMPILER} MATCHES "clang")
                    message(WARNING "unhandled compiler, can not set define prefix")
                    set(perfixd "")                 
                endif(${CMAKE_C_COMPILER} MATCHES "clang")
            
                
            endif ( CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX )    
        
            PREFIX_WITH(${perfixd} ${deflist})  
        
    ENDMACRO(PREFIX_COMPILER_DEFINES)



    set( have_myMacros ON )

endif (NOT have_myMacros)




