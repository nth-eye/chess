cmake_minimum_required(VERSION 3.5)

project(Chess LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(. QML src engine)

set(SOURCES
    src/main.cpp
    src/client.cpp
    src/game.cpp
    src/model.cpp
    src/piece.cpp
    src/player.cpp
    src/spot.cpp
    engine/engine.cpp
    engine/board.cpp
)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(Qt5 COMPONENTS Core Quick Widgets Network Concurrent REQUIRED)
#find_package(Qt5QuickCompiler)

if(ANDROID)
    add_library(Chess SHARED
        src/main.cpp
        src/client.cpp
        src/game.cpp
        src/model.cpp
        src/piece.cpp
        src/player.cpp
        src/spot.cpp
        engine/engine.cpp
        engine/board.cpp
        qml.qrc
    )
else()
    add_executable(Chess
        src/main.cpp
        src/client.cpp
        src/game.cpp
        src/model.cpp
        src/piece.cpp
        src/player.cpp
        src/spot.cpp
        engine/engine.cpp
        engine/board.cpp
        qml.qrc
    )
endif()

target_compile_definitions(Chess
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(Chess
  PRIVATE Qt5::Core Qt5::Quick Qt5::Network Qt5::Widgets Qt5::Concurrent)

