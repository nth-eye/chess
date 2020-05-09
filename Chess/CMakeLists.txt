cmake_minimum_required(VERSION 3.5)

project(Chess LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(.)

set(SOURCES
    main.cpp
    engine.cpp
    engine_pvtable.cpp
    engine_print.cpp
    engine_test.cpp
    engine_search.cpp
    engine_generate.cpp
    engine_misc.cpp
    client.cpp
    game.cpp
    model.cpp
    piece.cpp
    player.cpp
    spot.cpp
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
      main.cpp
      engine.cpp
      engine_pvtable.cpp
      engine_print.cpp
      engine_test.cpp
      engine_search.cpp
      engine_generate.cpp
      engine_misc.cpp
      client.cpp
      game.cpp
      model.cpp
      piece.cpp
      player.cpp
      spot.cpp
      qml.qrc
    )
else()
    add_executable(Chess
      main.cpp
      engine.cpp
      engine_pvtable.cpp
      engine_print.cpp
      engine_test.cpp
      engine_search.cpp
      engine_generate.cpp
      engine_misc.cpp
      client.cpp
      game.cpp
      model.cpp
      piece.cpp
      player.cpp
      spot.cpp
      qml.qrc
    )
endif()

target_compile_definitions(Chess
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(Chess
  PRIVATE Qt5::Core Qt5::Quick Qt5::Network Qt5::Widgets Qt5::Concurrent)

