PROJECT(serialize)
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

# cjson thirdparty
SET(CJSON "./thirdParty/cjson/cJSON.h" "./thirdParty/cjson/cJSON.c")
SOURCE_GROUP("thirdParty\\cjson" FILES ${CJSON})

# json
SET(JSON "./thirdParty/json/GenericReader.h" "./thirdParty/json/GenericReader.cpp" "./thirdParty/json/GenericWriter.h" "./thirdParty/json/GenericWriter.cpp")
SOURCE_GROUP("thirdParty\\json" FILES ${JSON})

# serialize
SET(INCLUDE "./include/serialize.h")
SOURCE_GROUP(include FILES ${INCLUDE})

# cjson
SET(INCLUDECJSON "./include/cjson/encoder.h" "./include/cjson/decoder.h")
SOURCE_GROUP("include\\cjson" FILES ${INCLUDECJSON})
SET(SRCCJSON "./src/cjson/encoder.cpp" "./src/cjson/decoder.cpp")
SOURCE_GROUP("src\\cjson" FILES ${SRCCJSON})

# json
SET(INCLUDEJSON "./include/json/encoder.h" "./include/json/decoder.h")
SOURCE_GROUP("include\\json" FILES ${INCLUDEJSON})
SET(SRCJSON "./src/json/encoder.cpp" "./src/json/decoder.cpp")
SOURCE_GROUP("src\\json" FILES ${SRCJSON})

# protobuf
SET(INCLUDEPROTOBUF "./include/protobuf/encoder.h" "./include/protobuf/decoder.h")
SOURCE_GROUP("include\\protobuf" FILES ${INCLUDEPROTOBUF})
SET(SRCPROTOBUF "./src/protobuf/encoder.cpp" "./src/protobuf/decoder.cpp")
SOURCE_GROUP("src\\protobuf" FILES ${SRCPROTOBUF})

LIST(REMOVE_ITEM DIR_SRCS "./tester/main.cpp")
IF (MSVC)
    ADD_LIBRARY(serialize SHARED
    ${CJSON} 
    ${JSON} 
    ${INCLUDE} 
    ${INCLUDECJSON} ${SRCCJSON} 
    ${INCLUDEJSON} ${SRCJSON} 
    ${INCLUDEPROTOBUF} ${SRCPROTOBUF} 
    ${DIR_SRCS})
ELSE ()
    ADD_LIBRARY(serialize 
    ${CJSON} 
    ${JSON} 
    ${INCLUDE} 
    ${INCLUDECJSON} ${SRCCJSON} 
    ${INCLUDEJSON} ${SRCJSON} 
    ${INCLUDEPROTOBUF} ${SRCPROTOBUF} 
    ${DIR_SRCS})
ENDIF (MSVC)


SET(EXEC OFF CACHE BOOL "")
if (${EXEC} STREQUAL ON)
    SET(TESTER "./tester/main.cpp" "./tester/testStruct.h")
    SOURCE_GROUP(tester FILES ${TESTER})
    ADD_EXECUTABLE(tester ${TESTER})
    TARGET_LINK_LIBRARIES(tester serialize)
endif()
