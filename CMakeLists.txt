cmake_minimum_required(VERSION 3.0)
project(Flow CXX)
set(CMAKE_CXX_STANDARD 17)

project(hades_flow)

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
list(APPEND CMAKE_PREFIX_PATH $ENV{AnalysisTree_HOME})
list(APPEND CMAKE_PREFIX_PATH $ENV{Flow_HOME})
list(APPEND CMAKE_PREFIX_PATH $ENV{HadesUtils_DIR})
list(APPEND CMAKE_PREFIX_PATH ${AnalysisTree_HOME})
list(APPEND CMAKE_PREFIX_PATH ${Flow_HOME})

find_package(ROOT REQUIRED COMPONENTS RIO Tree)
find_package( AnalysisTree REQUIRED)
find_package( Flow REQUIRED)

set(CMAKE_CXX_FLAGS "-Wall")

include(${ROOT_USE_FILE})

include_directories(
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src
        ${ROOT_INCLUDE_DIRS}
        ${AnalysisTree_INCLUDE_DIR}
        ${Flow_INCLUDE_DIR}
        ${Flow_INCLUDE_DIR}/../external/include
        $ENV{HadesUtils_DIR}/include
)

link_directories(
        ${CMAKE_SOURCE_DIR}
        ${AnalysisTreeBase_LIBRARY_DIR}
        ${Flow_LIBRARY_DIR}
        ${Flow_INCLUDE_DIR}/../external/lib
        $ENV{HadesUtils_DIR}/lib
)
add_definitions(${ROOT_CXX_FLAGS})

set (SOURCES
        src/acceptance_correction.cc
        src/tools.cc
        )

add_executable(correct src/real/correct.cc )
target_link_libraries(correct QnToolsCorrection QnToolsBase FlowCorrect FlowBase AnalysisTreeBase AnalysisTreeInfra ${ROOT_LIBRARIES}
        $ENV{HadesUtils_DIR}/lib/libHadesUtils.so)

add_executable(correlate src/real/correlate.cc)
target_link_libraries(correlate QnToolsBase FlowBase FlowCorrelate AnalysisTreeBase AnalysisTreeInfra ${ROOT_LIBRARIES})

add_executable(mc_correct src/mc/correct.cc )
target_link_libraries(mc_correct QnToolsCorrection QnToolsBase FlowCorrect FlowBase AnalysisTreeBase AnalysisTreeInfra ${ROOT_LIBRARIES}
        $ENV{HadesUtils_DIR}/lib/libHadesUtils.so)

add_executable(mc_correlate src/mc/correlate.cc)
target_link_libraries(mc_correlate QnToolsBase FlowBase FlowCorrelate AnalysisTreeBase AnalysisTreeInfra ${ROOT_LIBRARIES})