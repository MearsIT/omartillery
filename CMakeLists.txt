cmake_minimum_required(VERSION 3.21)

project(artillery-duel VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick QuickControls2 Multimedia)

qt_standard_project_setup()

qt_add_executable(artillery-duel
    src/main.cpp
)

qt_add_qml_module(artillery-duel
    URI ArtilleryDuel
    VERSION 1.0
    RESOURCE_PREFIX /
    QML_FILES
        qml/main.qml
)

target_link_libraries(artillery-duel PRIVATE
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Multimedia
)

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

install(TARGETS artillery-duel
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
