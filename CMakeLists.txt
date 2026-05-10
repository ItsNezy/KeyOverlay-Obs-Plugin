cmake_minimum_required(VERSION 3.20)
project(keyoverlay VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows specific definitions
if(WIN32)
    add_definitions(-DNOMINMAX -DWIN32_LEAN_AND_MEAN)
endif()

# Find OBS
find_package(libobs REQUIRED)
find_package(obs-frontend-api REQUIRED)

# Find Qt
find_package(Qt6 COMPONENTS Widgets WebEngineWidgets REQUIRED)

# Find uWebSockets
find_package(uSockets REQUIRED)
find_package(uWebSockets REQUIRED)

# Plugin source files
set(SOURCES
    src/plugin-main.cpp
    src/keyoverlay-plugin.cpp
    src/keyhook.cpp
    src/wsserver.cpp
    src/dock-panel.cpp
)

set(HEADERS
    src/keyoverlay-plugin.hpp
    src/keyhook.hpp
    src/wsserver.hpp
    src/dock-panel.hpp
)

add_library(${PROJECT_NAME} MODULE ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE src)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    OBS::libobs
    OBS::obs-frontend-api
    Qt6::Widgets
    Qt6::WebEngineWidgets
    uWebSockets::uWebSockets
    uSockets::uSockets
)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32 user32)
endif()

# Qt MOC (Meta-Object Compiler)
set_target_properties(${PROJECT_NAME} PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)

# Install steps
if(WIN32)
    install(TARGETS ${PROJECT_NAME}
        DESTINATION "$ENV{ProgramW6432}/obs-studio/obs-plugins/64bit"
    )
    
    # Copy data/ folder
    install(DIRECTORY data/
        DESTINATION "$ENV{ProgramW6432}/obs-studio/data/obs-plugins/${PROJECT_NAME}"
    )
endif()

# Copy web UI to data/ui during build
file(GLOB_RECURSE WEB_FILES "${CMAKE_SOURCE_DIR}/web/*")
foreach(WEB_FILE ${WEB_FILES})
    file(RELATIVE_PATH REL_PATH "${CMAKE_SOURCE_DIR}/web" "${WEB_FILE}")
    configure_file("${WEB_FILE}" "${CMAKE_SOURCE_DIR}/data/ui/${REL_PATH}" COPYONLY)
endforeach()
