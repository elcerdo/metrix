project(supergame)

cmake_minimum_required(VERSION 2.8)

find_package(PkgConfig REQUIRED)

pkg_search_module(OGRE REQUIRED OGRE)
message(STATUS "** OGRE_VERSION ${OGRE_VERSION}")
include_directories(${OGRE_INCLUDE_DIRS})

pkg_search_module(OIS REQUIRED OIS)
message(STATUS "** OIS_VERSION ${OIS_VERSION}")
include_directories(${OIS_INCLUDE_DIRS})
message(STATUS "prout ${OIS_LIBRARIES}")

add_executable(prout
    main.cc)

target_link_libraries(prout
    ${OIS_LIBRARIES}
    ${OGRE_LIBRARIES})

