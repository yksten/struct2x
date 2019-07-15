PROJECT(tester)

INCLUDE(CheckIncludeFileCXX)

CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

INCLUDE_DIRECTORIES(../ ../include/)
AUX_SOURCE_DIRECTORY(. DIR_SRCS)

SET(THIRDPARTY "../thirdParty/cJSON.h" "../thirdParty/cJSON.c")
SOURCE_GROUP(thirdParty FILES ${THIRDPARTY})

SET(SRC "../include/srtuctMacro.h" "../include/JSONReader.h" "../src/JSONReader.cpp" "../include/JSONWriter.h" "../src/JSONWriter.cpp")
SOURCE_GROUP(src FILES ${SRC})

SET(TESTER "./main.cpp")
SOURCE_GROUP(tester FILES ${TESTER})

ADD_EXECUTABLE(tester ${THIRDPARTY} ${SRC} ${TESTER} ${DIR_SRCS})