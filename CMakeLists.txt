cmake_minimum_required (VERSION 3.8)

project("genetic-algorithm-sudoku")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_STANDARD_REQUIRED TRUE)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

file(GLOB SOURCES "src/*.cpp")
file(GLOB IMGUI_SOURCES "imgui/*.cpp")
add_executable (${PROJECT_NAME} ${SOURCES} ${IMGUI_SOURCES})

file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/imgui")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/imgui/imgui-sfml-2.6.x")

find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-system sfml-network sfml-graphics sfml-window sfml-audio)

target_link_libraries(${PROJECT_NAME} PRIVATE
    opengl32
)