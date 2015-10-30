cmake_minimum_required(VERSION 2.8)
project(OpenGLVM)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../dist)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../dist)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../dist)

include("Integrate.cmake")
rcf_addlocation(RADONFRAMEWORK "git" "https://github.com/tak2004/RadonFramework.git")
rcf_addlocation(RFECS "git" "https://github.com/tak2004/RadonFastEntityComponentSystem.git")

set(RADONFRAMEWORK_BUILD_UNITTEST OFF CACHE BOOL "" FORCE)
set(RADONFRAMEWORK_BUILD_SYSTEMTEST OFF CACHE BOOL "" FORCE)
set(RADONFRAMEWORK_BUILD_RESOURCETEST OFF CACHE BOOL "" FORCE)
Integrate(RADONFRAMEWORK "Radon framework" "RADONFRAMEWORK_HOME")

#setup RFECS
Integrate(RFECS "RadonFastEntityComponentSystem" "")

#setup test demo
AddSourceDirectoryRecursive(techsrclist "technology" "Source")
AddHeaderDirectoryRecursive(techhdrlist "technology" "Include")
set(TECHNOLOGY_FILES ${techsrclist} ${techhdrlist})
Generate(EXECUTABLE TECHNOLOGY technology "Frontend")
AddDependency(TECHNOLOGY RadonFramework)
AddDependency(TECHNOLOGY RadonFastEntityComponentSystem)
AddPublicInclude(TECHNOLOGY "technology")
Finalize(TECHNOLOGY)

add_subdirectory("libSpec")
add_subdirectory("libOGLIF")
add_subdirectory("CodeGenerator")
add_subdirectory("OGLIFBuilder")