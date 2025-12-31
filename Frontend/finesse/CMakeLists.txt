cmake_minimum_required(VERSION 3.16)
project(finesse LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2)
qt_standard_project_setup()
qt_policy(SET QTP0001 NEW)
qt_policy(SET QTP0004 NEW)

qt_add_executable(finesse_app
    main.cpp
)

qt_add_qml_module(finesse_app
    URI finesse
    VERSION 1.0
    QML_FILES
        Main.qml
        pages/LandingPage.qml
        pages/Calendar.qml
        QML_FILES pages/Payments.qml
        QML_FILES pages/Summary.qml
        RESOURCES pages/finesse.qrc
RESOURCES pages/finesse.qrc
QML_FILES pages/Calendar.qml pages/LandingPage.qml pages/Payments.qml pages/Summary.qml
QML_FILES pages/components/Header.qml
QML_FILES pages/components/Header.qml
QML_FILES pages/components/Header.qml
)

target_link_libraries(finesse_app PRIVATE
    Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick Qt6::QuickControls2
)
