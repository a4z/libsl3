


ADD_EXECUTABLE( sl3_sample main.cpp )
TARGET_LINK_LIBRARIES( sl3_sample PRIVATE sl3
${LIBWARNINGS}
#"$<BUILD_INTERFACE:a4z:commonCompilerWarnings>"
)

ADD_EXECUTABLE( sl3_sample1 main1.cpp )
TARGET_LINK_LIBRARIES( sl3_sample1 PRIVATE sl3
${LIBWARNINGS}
#"$<BUILD_INTERFACE:a4z:commonCompilerWarnings>"
)

ADD_EXECUTABLE( sl3_sample2 main2.cpp )
TARGET_LINK_LIBRARIES( sl3_sample2 PRIVATE sl3
${LIBWARNINGS}
#"$<BUILD_INTERFACE:a4z:commonCompilerWarnings>"
)

ADD_EXECUTABLE( sl3_sample3 main3.cpp )
TARGET_LINK_LIBRARIES( sl3_sample3 PRIVATE sl3
  ${LIBWARNINGS}
  #"$<BUILD_INTERFACE:a4z:commonCompilerWarnings>"
)


ADD_EXECUTABLE( sl3_sample4 main4.cpp )
TARGET_LINK_LIBRARIES( sl3_sample4 PRIVATE sl3
${LIBWARNINGS}
#"$<BUILD_INTERFACE:a4z:commonCompilerWarnings>"
)

ADD_EXECUTABLE( sl3_sample5 main5.cpp )
TARGET_LINK_LIBRARIES( sl3_sample5 PRIVATE sl3
${LIBWARNINGS}
#"$<BUILD_INTERFACE:a4z:commonCompilerWarnings>"
)

ADD_EXECUTABLE( sl3_sample6 main6.cpp )
TARGET_LINK_LIBRARIES( sl3_sample6 PRIVATE sl3
${LIBWARNINGS}
#"$<BUILD_INTERFACE:a4z:commonCompilerWarnings>"
)
