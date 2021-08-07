PROJECT(struct2x)
CMAKE_MINIMUM_REQUIRED(VERSION 3.1)
INCLUDE(CheckIncludeFileCXX)

SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_DEBUG_POSTFIX d)

INCLUDE_DIRECTORIES(./ ./include/)
AUX_SOURCE_DIRECTORY(. DIR_SRCS)

# json
SET(JSON "./include/struct2x/json/GenericReader.h" "./thirdParty/json/GenericReader.cpp" "./include/struct2x/json/GenericWriter.h" "./thirdParty/json/GenericWriter.cpp")
SOURCE_GROUP("thirdParty\\json" FILES ${JSON})

# struct2x
SET(INCLUDE "./include/struct2x/traits.h" "./include/struct2x/struct2x.h")
SOURCE_GROUP(include FILES ${INCLUDE})

# json
SET(INCLUDEJSON "./include/struct2x/json/encoder.h" "./include/struct2x/json/decoder.h")
SOURCE_GROUP("include\\json" FILES ${INCLUDEJSON})

# protobuf
SET(INCLUDEPROTOBUF "./include/struct2x/protobuf/encoder.h" "./include/struct2x/protobuf/decoder.h")
SOURCE_GROUP("include\\protobuf" FILES ${INCLUDEPROTOBUF})
SET(SRCPROTOBUF "./src/protobuf/encoder.cpp" "./src/protobuf/decoder.cpp")
SOURCE_GROUP("src\\protobuf" FILES ${SRCPROTOBUF})

LIST(REMOVE_ITEM DIR_SRCS "./tester/main.cpp")
IF (MSVC)
    ADD_LIBRARY(struct2x SHARED
    ${JSON} 
    ${INCLUDE} 
    ${INCLUDEJSON} 
    ${INCLUDEPROTOBUF} ${SRCPROTOBUF} 
    ${DIR_SRCS})
ELSE ()
    ADD_LIBRARY(struct2x 
    ${JSON} 
    ${INCLUDE} 
    ${INCLUDEJSON} 
    ${INCLUDEPROTOBUF} ${SRCPROTOBUF} 
    ${DIR_SRCS})
ENDIF (MSVC)


SET(EXAMPLE OFF CACHE BOOL "")
if (${EXAMPLE} STREQUAL ON)
    SET(EXAMPLE "./example/main.cpp" "./example/exampleData.h")
    SOURCE_GROUP(example FILES ${EXAMPLE})
    ADD_EXECUTABLE(example ${EXAMPLE})
    TARGET_LINK_LIBRARIES(example struct2x)
endif()
