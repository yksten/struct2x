PROJECT(serialflex)
CMAKE_MINIMUM_REQUIRED(VERSION 3.1)
INCLUDE(CheckIncludeFileCXX)

SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_DEBUG_POSTFIX d)

INCLUDE_DIRECTORIES(include)
INCLUDE_DIRECTORIES(./src)
IF (MSVC)
    ADD_COMPILE_OPTIONS(/D _CRT_SECURE_NO_WARNINGS)
ENDIF (MSVC)

# serialflex
SET(INCLUDE "include/serialflex/traits.h" "include/serialflex/serialize.h")
SOURCE_GROUP(include FILES ${INCLUDE})

# third
SET(JSON "src/json/third/reader.h" "src/json/third/reader.cpp" "src/json/third/writer.h" "src/json/third/writer.cpp")
SOURCE_GROUP("src\\json\\third" FILES ${JSON})

# json
SET(INCLUDEJSON "include/serialflex/json/encoder.h" "include/serialflex/json/decoder.h")
SOURCE_GROUP("include\\json" FILES ${INCLUDEJSON})
SET(SRCJSON "src/json/encoder.cpp" "src/json/decoder.cpp")
SOURCE_GROUP("src\\json" FILES ${SRCJSON})

IF (MSVC)
    ADD_LIBRARY(${PROJECT_NAME} SHARED
    ${JSON} 
    ${INCLUDE} 
    ${INCLUDEJSON} ${SRCJSON}
    )
ELSE ()
    ADD_LIBRARY(${PROJECT_NAME} 
    ${JSON} 
    ${INCLUDE} 
    ${INCLUDEJSON} ${SRCJSON}
    )
    TARGET_COMPILE_OPTIONS(${PROJECT_NAME} PRIVATE -Wno-deprecated-declarations)
ENDIF (MSVC)


SET(EXAMPLE OFF CACHE BOOL "")
if (${EXAMPLE} STREQUAL ON)
    SET(EXAMPLE "example/main.cpp" "example/exampledata.h")
    SOURCE_GROUP(example FILES ${EXAMPLE})
    ADD_EXECUTABLE(example ${EXAMPLE})
    TARGET_LINK_LIBRARIES(example ${PROJECT_NAME})
endif()
