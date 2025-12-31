cmake_minimum_required(VERSION 3.16)
project(Elec376_F25_group12 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ensure Qt's meta-object code is generated (needed for Q_OBJECT)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Qt
find_package(Qt6 REQUIRED COMPONENTS Quick Qml Network)

# --- Sources ---
set(SOURCES
    Frontend/finesse/main.cpp
    Backend/Model/Expense.cpp
    Backend/Model/paymentsmanager.cpp
    Backend/Model/calendar.cpp
    Backend/Model/user.cpp
    Backend/Model/day.cpp
    Backend/Model/report.cpp
    Backend/Model/supabaseauth.cpp
    Backend/Model/supabaseclient.cpp
)

set(HEADERS
    Backend/Model/Expense.h
    Backend/Model/paymentsmanager.h
    Backend/Model/day.h
    Backend/Model/calendar.h
    Backend/Model/user.h
    Backend/Model/report.h
    Backend/Model/supabaseauth.h
    Backend/Model/supabaseclient.h
)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    Backend/Model/calendar_bridge.h Backend/Model/calendar_bridge.cpp
    Frontend/finesse/pages/components/NavBar.qml
    Frontend/finesse/pages/LoginPage.qml
    Backend/Model/dashboardbridge.h Backend/Model/dashboardbridge.cpp
    Backend/Model/pie.h Backend/Model/pie.cpp
    Frontend/finesse/pages/ProfilePage.qml

)

# Make headers visible so main.cpp can do #include "paymentsmanager.h"
target_include_directories(${PROJECT_NAME} PRIVATE Backend/Model)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Quick Qt6::Qml Qt6::Network)

# --- QML loading (from disk) ---
set(QML_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Frontend/finesse)
target_compile_definitions(${PROJECT_NAME} PRIVATE QML_DIR="${QML_DIR}")

# Collect QML (and assets)
file(GLOB_RECURSE QML_FILES CONFIGURE_DEPENDS
    ${QML_DIR}/*.qml
    ${QML_DIR}/*.js
    ${QML_DIR}/*.png
    ${QML_DIR}/*.svg
)

# Show them in Qt Creator's Projects view
target_sources(${PROJECT_NAME} PRIVATE ${QML_FILES})

# (optional) organize them nicely in the tree
source_group(TREE ${QML_DIR} PREFIX "QML" FILES ${QML_FILES})

# Keep your existing copy-to-build-dir step (so engine.load("Main.qml") works)
foreach(QML_FILE ${QML_FILES})
    file(RELATIVE_PATH REL_PATH ${QML_DIR} ${QML_FILE})
    get_filename_component(REL_DIR ${REL_PATH} DIRECTORY)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${REL_DIR})
    configure_file(${QML_FILE} ${CMAKE_CURRENT_BINARY_DIR}/${REL_PATH} COPYONLY)
endforeach()
foreach(QML_FILE ${QML_FILES})
    file(RELATIVE_PATH REL_PATH ${QML_DIR} ${QML_FILE})
    get_filename_component(REL_DIR ${REL_PATH} DIRECTORY)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${REL_DIR})
    configure_file(${QML_FILE} ${CMAKE_CURRENT_BINARY_DIR}/${REL_PATH} COPYONLY)
endforeach()

set_target_properties(${PROJECT_NAME} PROPERTIES QT_QML_ROOT_PATH ${QML_DIR})
