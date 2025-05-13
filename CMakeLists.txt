cmake_minimum_required(VERSION 3.20)
project(
  PAwChO_zad1 
  VERSION 0.1.0 
  LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(fmt REQUIRED)

add_subdirectory(cpp-httplib)

add_executable(main 
  main.cpp
)

target_link_libraries(main PRIVATE 
  fmt::fmt
  httplib::httplib
  ${ZLIB_LIBRARIES}
)

target_compile_definitions(main PRIVATE CPPHTTPLIB_NO_EXCEPTIONS)

if (WIN32)
  target_link_libraries(main PRIVATE
    stdc++exp
  )
endif()

if (MSVC)
  target_compile_options(main
    PRIVATE 
      /W4 
      /WX
  )
  target_compile_options(main PUBLIC /wd4244)
else()
  target_compile_options(main
    PRIVATE
      -Wall
      -Wextra
      -Wpedantic
      -Wshadow
      -Wconversion
      -fdiagnostics-color
      $<$<CONFIG:MinSizeRel>:-Oz>
      $<$<CONFIG:MinSizeRel>:-ffunction-sections>
      $<$<CONFIG:MinSizeRel>:-fdata-sections>
      $<$<CONFIG:MinSizeRel>:-fvisibility=hidden>
      $<$<CONFIG:MinSizeRel>:-fno-exceptions>
      $<$<CONFIG:MinSizeRel>:-fno-rtti>
      $<$<CONFIG:MinSizeRel>:-fno-unwind-tables>
      $<$<CONFIG:MinSizeRel>:-fno-asynchronous-unwind-tables>
  )
  
  if(NOT APPLE)
    target_link_options(main
      PRIVATE
        $<$<CONFIG:MinSizeRel>:-Wl,--gc-sections>
        $<$<CONFIG:MinSizeRel>:-s>
        $<$<CONFIG:MinSizeRel>:-static>
    )
  endif()
endif()

target_compile_features(main PRIVATE cxx_std_26)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set_target_properties(main PROPERTIES 
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
  COMPILE_WARNING_AS_ERROR ON
)

if(IPO_SUPPORTED)
  set_property(TARGET main PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
  target_compile_options(main 
    PRIVATE $<$<CONFIG:MinSizeRel>:-flto>)
  target_link_options(main 
    PRIVATE $<$<CONFIG:MinSizeRel>:-flto>)

endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE MinSizeRel)
endif()