cmake_minimum_required(VERSION 3.28...3.30)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/common/bootstrap.cmake" NO_POLICY_SCOPE)

project(${_name} VERSION ${_version})

option(ENABLE_FRONTEND_API "Use obs-frontend-api for UI functionality" ON)
option(ENABLE_QT "Use Qt functionality" ON)

include(compilerconfig)
include(defaults)
include(helpers)

add_library(${CMAKE_PROJECT_NAME} MODULE)

find_package(libobs REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OBS::libobs)

if(ENABLE_FRONTEND_API)
  find_package(obs-frontend-api REQUIRED)
  target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OBS::obs-frontend-api)
endif()

if(ENABLE_QT)
  find_package(Qt6 COMPONENTS Widgets REQUIRED)
  target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt6::Widgets)
  set_target_properties(
    ${CMAKE_PROJECT_NAME}
    PROPERTIES AUTOMOC ON AUTOUIC ON AUTORCC ON
  )
endif()

# uWebSockets — header-only library + compiled uSockets
if(DEFINED UWS_INCLUDE_DIR)
  target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "${UWS_INCLUDE_DIR}")
endif()
if(DEFINED USOCKETS_LIB)
  target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE "${USOCKETS_LIB}")
endif()

# Windows-specific
if(WIN32)
  add_definitions(-DNOMINMAX -DWIN32_LEAN_AND_MEAN)
  target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ws2_32 user32)
endif()

target_sources(
  ${CMAKE_PROJECT_NAME}
  PRIVATE
    src/plugin-main.cpp
    src/keyoverlay-plugin.cpp
    src/keyhook.cpp
    src/wsserver.cpp
    src/httpserver.cpp
    src/dock-panel.cpp
)

# Copy web UI to data/ui during build
file(GLOB_RECURSE WEB_FILES "${CMAKE_CURRENT_SOURCE_DIR}/web/*")
foreach(WEB_FILE ${WEB_FILES})
  file(RELATIVE_PATH REL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/web" "${WEB_FILE}")
  configure_file("${WEB_FILE}" "${CMAKE_CURRENT_SOURCE_DIR}/data/ui/${REL_PATH}" COPYONLY)
endforeach()

set_target_properties_plugin(${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME ${_name})
