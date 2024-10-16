cmake_minimum_required (VERSION 3.8)

project("genetic-algorithm-sudoku")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_STANDARD_REQUIRED TRUE)

find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    option(GIT_SUBMODULE "Check submodule during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMODULE_RESULT)
        if(NOT GIT_SUBMODULE_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMODULE_RESULT}, please checkout submodules")
        endif()


        message(STATUS "Checking out specific version for SFML submodule")
        execute_process(COMMAND ${GIT_EXECUTABLE} checkout 2.6.1
                        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/external/sfml"
                        RESULT_VARIABLE GIT_CHECKOUT_RESULT)
        if(NOT GIT_CHECKOUT_RESULT EQUAL "0")
            message(FATAL_ERROR "git checkout for SFML version 2.6.1 failed with ${GIT_CHECKOUT_RESULT}.")
        endif()


        message(STATUS "Switching to specific branch of imgui-SFML submodule")
        execute_process(COMMAND ${GIT_EXECUTABLE} switch 2.6.x
                        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui-sfml"
                        RESULT_VARIABLE GIT_SWITCH_RESULT)
        if(NOT GIT_SWITCH_RESULT EQUAL "0")
            message(FATAL_ERROR "git switch for imgui-SFML branch 2.6.x failed with ${GIT_SWITCH_RESULT}.")
        endif()
    endif()
else()
    message(FATAL_ERROR
        "Git is required but not found or this is not a Git repository.\n"
        "Make sure git is installed and initialized in this directory."
    )
endif()

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

add_subdirectory(${CMAKE_SOURCE_DIR}/external/sfml)

file(GLOB SOURCES "src/*.cpp")
file(GLOB IMGUI_SOURCES "external/imgui/*.cpp")
file(GLOB IMGUI_SFML_SOURCES "external/imgui-sfml/*.cpp")
file(GLOB IMPLOT_SOURCES "external/implot/*.cpp")

add_executable (${PROJECT_NAME} ${SOURCES} ${IMGUI_SOURCES} ${IMGUI_SFML_SOURCES} ${IMPLOT_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/external/imgui"
    "${CMAKE_SOURCE_DIR}/external/imgui-sfml"
    "${CMAKE_SOURCE_DIR}/external/implot"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    sfml-system
    sfml-graphics
    sfml-window
    opengl32
    winmm
    gdi32
)

file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})

if(WIN32)
    add_custom_command(
        TARGET ${PROJECT_NAME}
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy 
        ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll 
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        VERBATIM
    )
endif()