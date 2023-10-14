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

# struct2x
SET(INCLUDE "./include/struct2x/traits.h" "./include/struct2x/struct2x.h")
SOURCE_GROUP(include FILES ${INCLUDE})

# thirdParty
SET(JSON "./include/struct2x/json/genericreader.h" "./src/thirdParty/json/genericreader.cpp" "./include/struct2x/json/genericwriter.h" "./src/thirdParty/json/genericwriter.cpp")
SOURCE_GROUP("src\\thirdParty\\json" FILES ${JSON})

# json
SET(INCLUDEJSON "./include/struct2x/json/encoder.h" "./include/struct2x/json/decoder.h")
SOURCE_GROUP("include\\json" FILES ${INCLUDEJSON})
SET(SRCJSON "./src/json/encoder.cpp" "./src/json/decoder.cpp")
SOURCE_GROUP("src\\json" FILES ${SRCJSON})

LIST(REMOVE_ITEM DIR_SRCS "./tester/main.cpp")
IF (MSVC)
    ADD_LIBRARY(struct2x SHARED
    ${JSON} 
    ${INCLUDE} 
    ${INCLUDEJSON} ${SRCJSON}
    ${DIR_SRCS})
ELSE ()
    ADD_LIBRARY(struct2x 
    ${JSON} 
    ${INCLUDE} 
    ${INCLUDEJSON} ${SRCJSON}
    ${DIR_SRCS})
ENDIF (MSVC)


SET(EXAMPLE OFF CACHE BOOL "")
if (${EXAMPLE} STREQUAL ON)
    SET(EXAMPLE "./example/main.cpp" "./example/exampledata.h")
    SOURCE_GROUP(example FILES ${EXAMPLE})
    ADD_EXECUTABLE(example ${EXAMPLE})
    TARGET_LINK_LIBRARIES(example struct2x)
endif()
