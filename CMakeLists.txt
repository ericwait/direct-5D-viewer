cmake_minimum_required(VERSION 3.22)

# Setup vcpkg script with CMake (note: should be placed before project() call)
set(VCPKG_TARGET_TRIPLET "$ENV{VCPKG_DEFAULT_TRIPLET}" CACHE STRING "Vcpkg target triplet (defaults to $ENV{VCPKG_DEFAULT_TRIPLET})")
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "vcpkg toolchain file")

# Set project name and specify supported languages
project(d3dViewer LANGUAGES C CXX VERSION 0.1.0)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)  # Use 20 for C++20 or 17 for C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # Ensure no compiler-specific extensions are used

# Set the output directories for libraries and executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Define build configurations
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

set(D3d_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/c/D3dLib/D3d)
set(Global_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/c/D3dLib/Global)
set(Messages_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/c/D3dLib/Messages)
set(Shaders_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/c/D3dLib/Shaders)

add_subdirectory(src/c/D3dLib)
add_subdirectory(src/c/Standalone)

find_package(Matlab)
if(Matlab_FOUND)
    add_subdirectory(src/c/Mex)
else()
    message(WARNING "Matlab not found, skipping src/c/Mex")
endif()
