cmake_minimum_required(VERSION 3.21)

project(artillery-duel VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick QuickControls2 QuickTest Multimedia Test)

qt_standard_project_setup()

qt_policy(SET QTP0004 NEW)

set_source_files_properties(qml/Theme.qml PROPERTIES
    QT_QML_SINGLETON_TYPE TRUE
)

qt_add_library(artillery_core SHARED)
qt_add_qml_module(artillery_core
    URI io.github.jandal.artillery-duel
    VERSION 1.0
    RESOURCE_PREFIX /
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/io/github/jandal/artillery-duel
    SOURCES
        src/player.h src/player.cpp
        src/gameengine.h src/gameengine.cpp
        src/physicsengine.h src/physicsengine.cpp
        src/aiopponent.h src/aiopponent.cpp
    QML_FILES
        Panel.qml
        qml/GameBoard.qml
        qml/Tank.qml
        qml/PlayerControls.qml
        qml/Projectile.qml
        qml/Explosion.qml
        qml/MainMenu.qml
        qml/HUD.qml
        qml/GameOverScreen.qml
        qml/Theme.qml
        qml/RetroButton.qml
)

target_include_directories(artillery_core PUBLIC src)
target_link_libraries(artillery_core PUBLIC Qt6::Quick Qt6::QuickControls2 Qt6::Multimedia)

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
