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

SET(CJSON "./thirdParty/cjson/cJSON.h" "./thirdParty/cjson/cJSON.c")
SOURCE_GROUP("thirdParty\\cjson" FILES ${CJSON})

SET(INCLUDE "./include/struct2x.h")
SOURCE_GROUP(include FILES ${INCLUDE})

SET(INCLUDECJSON "./include/json/JSONEncoder.h" "./include/json/JSONDecoder.h")
SOURCE_GROUP("include\\json" FILES ${INCLUDECJSON})

SET(INCLUDEFASTJSON "./include/fastjson/fastjsonEncoder.h" "./include/fastjson/fastjsonDecoder.h")
SOURCE_GROUP("include\\fastjson" FILES ${INCLUDEFASTJSON})

SET(INCLUDEPROTOBUF "./include/protobuf/encoder.h" "./include/protobuf/decoder.h")
SOURCE_GROUP("include\\protobuf" FILES ${INCLUDEPROTOBUF})

SET(INCLUDEMSGPACK "./include/msgpack/encoder.h" "./include/msgpack/decoder.h")
SOURCE_GROUP("include\\msgpack" FILES ${INCLUDEMSGPACK})

SET(SRC "./src/struct2x.cpp")
SOURCE_GROUP(src FILES ${SRC})

SET(SRCJSON "./src/json/JSONEncoder.cpp" "./src/json/JSONDecoder.cpp")
SOURCE_GROUP("src\\json" FILES ${SRCJSON})

SET(SRCFASTJSON "./src/fastjson/fastjsonEncoder.cpp" "./src/fastjson/fastjsonDecoder.cpp")
SOURCE_GROUP("src\\fastjson" FILES ${SRCFASTJSON})

SET(SRCPROTOBUF "./src/protobuf/encoder.cpp" "./src/protobuf/decoder.cpp")
SOURCE_GROUP("src\\protobuf" FILES ${SRCPROTOBUF})

SET(SRCMSGPACK "./src/msgpack/encoder.cpp" "./src/msgpack/decoder.cpp")
SOURCE_GROUP("src\\msgpack" FILES ${SRCMSGPACK})

LIST(REMOVE_ITEM DIR_SRCS "./tester/main.cpp")
IF (MSVC)
    ADD_LIBRARY(struct2x SHARED ${CJSON} ${INCLUDE} ${INCLUDECJSON} ${INCLUDEFASTJSON} ${INCLUDEPROTOBUF} ${INCLUDEMSGPACK} ${SRC} ${SRCJSON} ${SRCFASTJSON} ${SRCPROTOBUF} ${SRCMSGPACK} ${DIR_SRCS})
ELSE ()
    ADD_LIBRARY(struct2x ${CJSON} ${INCLUDE} ${INCLUDECJSON} ${INCLUDEFASTJSON} ${INCLUDEPROTOBUF} ${INCLUDEMSGPACK} ${SRC} ${SRCJSON} ${SRCFASTJSON} ${SRCPROTOBUF} ${SRCMSGPACK} ${DIR_SRCS})
ENDIF (MSVC)


SET(TESTER "./tester/main.cpp" "./tester/testStruct.h")
SOURCE_GROUP(tester FILES ${TESTER})

ADD_EXECUTABLE(tester ${TESTER})
TARGET_LINK_LIBRARIES(tester struct2x)
