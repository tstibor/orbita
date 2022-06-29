project(orbita CXX)
cmake_minimum_required(VERSION 3.16)

string(TIMESTAMP orbita_BUILD_TS UTC)

set(orbita_VERSION_MAJOR 0)
set(orbita_VERSION_MINOR 1)
set(orbita_VERSION_PATCH 2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing(true)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/version.h.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/version.h
)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/")
include(GNUInstallDirs)

include_directories("${CMAKE_CURRENT_BINARY_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/custwidget")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/db")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/dialog")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/mpc")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/net")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/render")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/solarsystem")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/table")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/test")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/util")

if (WIN32)
  set(NOVA_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/libnova-win64/header")
  set(NOVA_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/libnova-win64/dll/libnova-0-16-0.dll")
endif (WIN32)

find_package(Nova REQUIRED)
find_package(Qt6 6.2 REQUIRED COMPONENTS Core Gui Widgets Network OpenGLWidgets Sql Test)
find_package(OpenGL REQUIRED)

find_package(OpenMP)
if (OPENMP_FOUND)
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif (OPENMP_FOUND)

if (Qt6_FOUND)
  message(STATUS "Qt6 library version ${Qt6_VERSION} found")
endif (Qt6_FOUND)

if (NOVA_FOUND)
  include_directories(${NOVA_INCLUDE_DIR})
endif(NOVA_FOUND)

if (WIN32)
  get_target_property(QMAKE_EXE Qt6::qmake IMPORTED_LOCATION)
  get_filename_component(QT_BIN_DIR "${QMAKE_EXE}" DIRECTORY)

  find_program(WINDEPLOYQT_ENV_SETUP qtenv2.bat HINTS "${QT_BIN_DIR}")
  find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${QT_BIN_DIR}")
endif (WIN32)


QT6_ADD_RESOURCES(${CMAKE_CURRENT_SOURCE_DIR}/src/orbita.qrc)

set(orbita_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/mainwindow.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/render/renderwindow.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/custwidget/statusbarwidget.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/solarsystem/celestialbody.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/solarsystem/planet.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/solarsystem/solarsystem.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/solarsystem/asteroid.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/solarsystem/comet.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/util/logger.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/db/sqlite.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/util/settings.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/mpc/mpcparser.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/mpc/mpctosql.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/dialog/progressdialog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/table/tabledialog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/table/asteroidtabledialog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/table/comettabledialog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/dialog/downloaddialog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/net/downloadmanager.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/custwidget/clickablelabel.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/solarsystem/orbit.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/orbita.qrc
)

add_executable(orbita ${orbita_SRCS})

target_link_libraries(orbita
  ${NOVA_LIBRARIES}
  ${OPENGL_LIBRARIES}
  Qt6::Core
  Qt6::Gui
  Qt6::Widgets
  Qt6::Network
  Qt6::OpenGLWidgets
  Qt6::Sql
)

set(orbitatest_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/src/mpc/mpcparser.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/db/sqlite.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/util/settings.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/test/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/test/testsqlite.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/test/testmpcparser.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/orbita.qrc
)

add_executable(orbitatest ${orbitatest_SRCS})
add_test(NAME orbitatest COMMAND orbitatest)

target_link_libraries(orbitatest PRIVATE
  ${NOVA_LIBRARIES}
  ${OPENGL_LIBRARIES}
  Qt6::Core
  Qt6::Gui
  Qt6::Widgets
  Qt6::Network
  Qt6::OpenGLWidgets
  Qt6::Sql
  Qt6::Test
)

if (WIN32)
  add_custom_command(TARGET orbita
    POST_BUILD
    COMMAND "${WINDEPLOYQT_ENV_SETUP}" && "${WINDEPLOYQT_EXECUTABLE}" \"$<TARGET_FILE:orbita>\"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${NOVA_LIBRARIES}" \"$<TARGET_FILE_DIR:orbita>\"
  )
endif (WIN32)
