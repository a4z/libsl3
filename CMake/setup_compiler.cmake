

if (NOT have_myCompilerSetup)

  include(setup_toolchain)

  set(have_myCompilerSetup ON BOOL)

endif (NOT have_myCompilerSetup)
