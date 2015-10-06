

if (NOT have_myBoostSetup)

    #generel to find boost 
    if ( NOT BOOSTROOT AND NOT BOOST_ROOT)
        set( BOOSTROOT  $ENV{BOOSTROOT} )
        if (NOT BOOSTROOT)
            set( BOOSTROOT  $ENV{BOOST_ROOT} )
        endif()
    endif()
    
    if(WIN32 AND MSVC)
    
        if (NOT BOOSTROOT)
        #TODO , check "C:/Program Files/boost" and C:/Boost ...
            message( STATUS "use some default value for Boost installation root" )
            set(BOOSTROOT "C:/Program Files/boost")
        endif(NOT BOOSTROOT)
    
    endif(WIN32 AND MSVC)

    
    SET(BOOSTROOT ${BOOSTROOT} CACHE PATH "where to find boost" )   
    
    #set(Boost_ADDITIONAL_VERSIONS "1.46.1" CACHE STRING "additional boost versions")
    option (Boost_NO_SYSTEM_PATHS "prevent searching system path for boost" OFF)
    option(Boost_USE_STATIC_LIBS "Can be set to ON to force the use of the static boost libraries" OFF)
    option(Boost_USE_MULTITHREAD "set to OFF to use the non-multithreaded boost libs" ON)
    

    find_package( Boost )
    include_directories( BEFORE SYSTEM ${Boost_INCLUDE_DIR} )
    #link_directories ( ${Boost_LIBRARY_DIRS} ) 
    
    set( have_myBoostSetup ON BOOL)

endif (NOT have_myBoostSetup)
