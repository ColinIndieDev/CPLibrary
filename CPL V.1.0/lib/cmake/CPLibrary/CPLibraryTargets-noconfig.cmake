#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "CPL::CPLibrary" for configuration ""
set_property(TARGET CPL::CPLibrary APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(CPL::CPLibrary PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libCPLibrary.a"
  )

list(APPEND _cmake_import_check_targets CPL::CPLibrary )
list(APPEND _cmake_import_check_files_for_CPL::CPLibrary "${_IMPORT_PREFIX}/lib/libCPLibrary.a" )

# Import target "CPL::glad" for configuration ""
set_property(TARGET CPL::glad APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(CPL::glad PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "C"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libglad.a"
  )

list(APPEND _cmake_import_check_targets CPL::glad )
list(APPEND _cmake_import_check_files_for_CPL::glad "${_IMPORT_PREFIX}/lib/libglad.a" )

# Import target "CPL::glfw" for configuration ""
set_property(TARGET CPL::glfw APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(CPL::glfw PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "C"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libglfw3.a"
  )

list(APPEND _cmake_import_check_targets CPL::glfw )
list(APPEND _cmake_import_check_files_for_CPL::glfw "${_IMPORT_PREFIX}/lib/libglfw3.a" )

# Import target "CPL::freetype" for configuration ""
set_property(TARGET CPL::freetype APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(CPL::freetype PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "C"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libfreetype.a"
  )

list(APPEND _cmake_import_check_targets CPL::freetype )
list(APPEND _cmake_import_check_files_for_CPL::freetype "${_IMPORT_PREFIX}/lib/libfreetype.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
