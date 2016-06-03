

if (NOT have_myCompilerSetup)

    if ( CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX )
    
        message(STATUS "set up defaults for gcc")
    
        if (NOT CMAKE_C_FLAGS)
            SET (CMAKE_C_FLAGS "-Wall -Wextra -pedantic -std=c11 -pipe")
        endif(NOT CMAKE_C_FLAGS)
        
       SET (CMAKE_C_FLAGS ${CMAKE_C_FLAGS} CACHE STRING 
        "Flags used by the compiler during all build types."
        FORCE )
    
        if (NOT CMAKE_CXX_FLAGS)
            SET (CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -std=c++1y -pipe -pthread")
        endif(NOT CMAKE_CXX_FLAGS)
    
       SET (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} CACHE STRING 
        "Flags used by the compiler during all build types." 
        FORCE )  
        
        if (CODECOVERAGE) 
            add_definitions(--coverage -fprofile-arcs -ftest-coverage)
            set (OPTION_GCOVLIB gcov CACHE STRING INTERNAL)
        endif(CODECOVERAGE)
        
    elseif( MSVC )
    # TODO default flags ? 
    
    else ( CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX )
        # 
        # cmake ../  -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Debug
        if(${CMAKE_C_COMPILER} MATCHES "clang")
            message(STATUS "set up defaults for clang")
            #this is somehow to much , TODO find out what is usefull
            #set(MYCLANGWARNINGS "-Weverything")  
            set(MYCLANGWARNINGS "-Wall -Wextra -pedantic")
            
            if (NOT CMAKE_C_FLAGS)
                SET (CMAKE_C_FLAGS "-std=c99 ${MYCLANGWARNINGS}")
            endif(NOT CMAKE_C_FLAGS)
            
           SET (CMAKE_C_FLAGS ${CMAKE_C_FLAGS} CACHE STRING 
            "Flags used by the compiler during all build types." 
            FORCE )
        
            if (NOT CMAKE_CXX_FLAGS)
                SET (CMAKE_CXX_FLAGS "-std=c++0x -pipe -pthread ${MYCLANGWARNINGS}")
            endif(NOT CMAKE_CXX_FLAGS)
        
           SET (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} CACHE STRING 
            "Flags used by the compiler during all build types." 
            FORCE )       
    
        else (${CMAKE_C_COMPILER} MATCHES "clang")
            message(WARNING "unknown compiler")                      
        endif(${CMAKE_C_COMPILER} MATCHES "clang")    
        
    endif ( CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX )

    set(have_myCompilerSetup ON BOOL)    

endif (NOT have_myCompilerSetup)
