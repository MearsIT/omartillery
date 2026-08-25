cmake_minimum_required(VERSION 3.21)

project(artillery-duel VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick QuickControls2 QuickTest Multimedia Test)

qt_standard_project_setup()

qt_policy(SET QTP0004 NEW)

qt_add_library(artillery_core STATIC)
qt_add_qml_module(artillery_core
    URI ArtilleryDuel
    VERSION 1.0
    RESOURCE_PREFIX /
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/ArtilleryDuel
    SOURCES
        src/player.h src/player.cpp
        src/gameengine.h src/gameengine.cpp
        src/physicsengine.h src/physicsengine.cpp
    QML_FILES
        qml/main.qml
        qml/GameBoard.qml
        qml/Tank.qml
        qml/PlayerControls.qml
        qml/Projectile.qml
        qml/Explosion.qml
    RESOURCES
        assets/tank_left.png
        assets/tank_right.png
        assets/terrain_pattern.png
        assets/projectile.png
        assets/explosion_frames.png
)

target_include_directories(artillery_core PUBLIC src)
target_link_libraries(artillery_core PUBLIC Qt6::Quick Qt6::QuickControls2 Qt6::Multimedia)

qt_add_executable(artillery-duel src/main.cpp)
target_link_libraries(artillery-duel PRIVATE artillery_core artillery_coreplugin)

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

install(TARGETS artillery-duel
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
