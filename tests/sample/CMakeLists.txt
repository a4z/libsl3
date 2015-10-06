


SET( sample_SRC
main.cpp

)

ADD_EXECUTABLE( sl3_sample ${sample_SRC} )
TARGET_LINK_LIBRARIES( sl3_sample sl3 ${mysqlt3_LIBS} ${OPTION_GCOVLIB})




