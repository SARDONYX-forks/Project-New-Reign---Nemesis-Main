cmake_minimum_required(VERSION 3.13.0 FATAL_ERROR)

project(NemesisUnlimitedBehaviorEngine LANGUAGES CXX VERSION 0.84)
################################################################################
# VARIABLES
################################################################################


# This part is the only one that should be modified by the user wanting to compile Nemesis

# set(Qt5_DIR "C:/IT/Qt/5.14.1/msvc2017/lib/cmake/Qt5") # Comment out when use Github CI

option(UseLocalBoost "Uses local copy of Boost instead of downloading it" OFF) # ON or OFF
set(BOOST_ROOT "C:/IT/Boost_1.72.0" CACHE PATH "Boost local path" )

#You have to use a local copy of Python source, not the binaries
#Note: It MUST match the version set in Python.cmake
option(UseLocalPython "Uses local copy of Python instead of downloading it" OFF) # ON or OFF
set(PYTHON_ROOT "C:/IT/cpython" CACHE PATH "Python local path" )

################################################################################
# Use solution folders feature
################################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################################################################
# Required packages
################################################################################

include(CMake/Python.cmake)
include(CMake/Boost.cmake)
include(CMake/Qt.cmake)
add_subdirectory(external/hkxcmd)

################################################################################
# Source groups
################################################################################
# Header Files
file(GLOB Header_Files "include/*.h" "include/**/*.h")
source_group("Header Files" FILES ${Header_Files})

# Source Files
file(GLOB Source_Files "src/*.cpp" "src/**/*.cpp")
source_group("Source Files" FILES ${Source_Files})

# Resources Files
file(GLOB Resources_Files "resources/*.rc" "resources/*.qrc")
source_group("Resources Files" FILES ${Resources_Files})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
    ${Resources_Files}
    )
################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} WIN32 "${PROJECT_SOURCE_DIR}/${ALL_FILES}")

################################################################################
# Include directories
################################################################################

target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}")

#Windows API
target_include_directories(${PROJECT_NAME} PUBLIC $ENV{UniversalCRTSdkDir}/Include/ucrt)

################################################################################
# Compile definitions
################################################################################
# _WIN32_WINNT=0x0601: Windows7
# See: https://learn.microsoft.com/en-US/cpp/porting/modifying-winver-and-win32-winnt?view=msvc-170
target_compile_definitions(${PROJECT_NAME} PRIVATE
    "UNICODE;"
    "_UNICODE;"
    "WIN32;"
    "WIN64"
    "_WIN32_WINNT=0x0601"
    )

################################################################################
# Copy Python DLL
################################################################################

message(STATUS "Copying Python DLL: ${Python_DLL_PATH}")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${Python_DLL_PATH}
    ${CMAKE_BINARY_DIR}/${Python_DLL_NAME})

################################################################################
# Compile and link options
################################################################################

#C++17
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

if(MSVC)
    #Strictly follow the standard
    target_compile_options(${PROJECT_NAME} PRIVATE "/permissive-")
    #Required
    target_compile_options(${PROJECT_NAME} PRIVATE "/bigobj")
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE BoostNemesis PythonNemesis Qt5Nemesis hkxcmd#add_subdirectory(external/hkxcmd)
)
