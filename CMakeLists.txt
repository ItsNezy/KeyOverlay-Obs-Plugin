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



# Windows-specific
if(WIN32)
  add_definitions(-DNOMINMAX -DWIN32_LEAN_AND_MEAN)
  target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ws2_32 user32 crypt32 advapi32)
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

# Copy web UI to data/ui during CMake configure
# This ensures data/ui/ has the web files before install/package steps
file(GLOB_RECURSE WEB_UI_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/web/*.html"
  "${CMAKE_CURRENT_SOURCE_DIR}/web/*.css"
  "${CMAKE_CURRENT_SOURCE_DIR}/web/*.js"
  "${CMAKE_CURRENT_SOURCE_DIR}/web/*.json"
  "${CMAKE_CURRENT_SOURCE_DIR}/web/*.png"
  "${CMAKE_CURRENT_SOURCE_DIR}/web/*.jpg"
  "${CMAKE_CURRENT_SOURCE_DIR}/web/*.svg"
  "${CMAKE_CURRENT_SOURCE_DIR}/web/*.woff2"
)
foreach(_file ${WEB_UI_FILES})
  file(RELATIVE_PATH _rel "${CMAKE_CURRENT_SOURCE_DIR}/web" "${_file}")
  configure_file("${_file}" "${CMAKE_CURRENT_SOURCE_DIR}/data/ui/${_rel}" COPYONLY)
endforeach()

# Also copy at build time so changes are picked up
add_custom_target(copy_web_ui ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/web"
    "${CMAKE_CURRENT_SOURCE_DIR}/data/ui"
  COMMENT "Copying web UI to data/ui/"
)
add_dependencies(${CMAKE_PROJECT_NAME} copy_web_ui)

set_target_properties_plugin(${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME ${_name})
