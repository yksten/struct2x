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

SET(THIRDPARTY "./thirdParty/cJSON.h" "./thirdParty/cJSON.c")
SOURCE_GROUP(thirdParty FILES ${THIRDPARTY})

SET(HEADER "./include/struct2x.h" "./include/json/JSONReader.h" "./include/json/JSONWriter.h" "./include/protobuf/encoder.h" "./include/protobuf/decoder.h")
SOURCE_GROUP(header FILES ${HEADER})

SET(SRC "./src/json/JSONReader.cpp" "./src/json/JSONWriter.cpp" "./src/protobuf/encoder.cpp" "./src/protobuf/decoder.cpp" "./src/struct2x.cpp")
SOURCE_GROUP(src FILES ${SRC})

SET(PARSE "./src/protobuf/parse/proto.h" "./src/protobuf/parse/proto.cpp")
SOURCE_GROUP(parse FILES ${PARSE})

LIST(REMOVE_ITEM DIR_SRCS "./tester/main.cpp")
IF (MSVC)
    ADD_LIBRARY(struct2x SHARED ${THIRDPARTY} ${PARSE} ${HEADER} ${SRC} ${DIR_SRCS})
ELSE ()
    ADD_LIBRARY(struct2x ${THIRDPARTY} ${PARSE} ${HEADER} ${SRC} ${DIR_SRCS})
ENDIF (MSVC)


SET(TESTER "./tester/main.cpp" "./tester/testStruct.h")
SOURCE_GROUP(tester FILES ${TESTER})

ADD_EXECUTABLE(tester ${TESTER})
TARGET_LINK_LIBRARIES(tester struct2x)
