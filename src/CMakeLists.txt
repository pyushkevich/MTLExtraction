cmake_minimum_required(VERSION 3.0)
project(MTLExtraction)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find ITK.
find_package(ITK REQUIRED)

IF(ITK_FOUND)
	INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
	MESSAGE(FATAL_ERROR "Please set ITK_DIR.")
ENDIF(ITK_FOUND)

file(GLOB SRC
    *.cxx
)
add_library(lib ${SRC})

add_executable(MTLExtraction Main.cxx)

target_link_libraries(MTLExtraction PUBLIC ${ITK_LIBRARIES} lib)

install (TARGETS MTLExtraction RUNTIME DESTINATION C:/Users/jadel/bin)
