cmake_minimum_required(VERSION 3.10)
project(libhep VERSION 1.0.0 LANGUAGES C)

include(GNUInstallDirs)

add_library(hep SHARED
  core_hep.c
  src/hep_log.c
  src/hep_io.c
  src/hep_config.c
  src/hep_proto.c
  src/hep_transport.c
)

target_include_directories(hep
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_definitions(hep PRIVATE USE_IPV6)
find_package(ZLIB)
if(ZLIB_FOUND)
  target_compile_definitions(hep PRIVATE USE_ZLIB)
  target_link_libraries(hep PRIVATE ZLIB::ZLIB)
endif()

set_target_properties(hep PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION 1
  PUBLIC_HEADER "include/hep/hep_api.h;include/hep/hep_config.h;include/hep/hep_io.h;include/hep/hep_log.h;include/hep/hep_portability.h;include/hep/hep_proto.h"
)

install(TARGETS hep
  EXPORT hepTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/hep
)

option(HEP_BUILD_TESTS "Build libhep test programs" OFF)
if(HEP_BUILD_TESTS)
  enable_testing()
  add_executable(test_hep_auth_transport tests/test_hep_auth_transport.c)
  target_include_directories(test_hep_auth_transport
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
  target_link_libraries(test_hep_auth_transport PRIVATE hep)
  add_test(NAME test_hep_auth_transport COMMAND test_hep_auth_transport)
endif()
