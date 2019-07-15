PROJECT(struct)

include(CheckIncludeFileCXX)

CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

AUX_SOURCE_DIRECTORY(./ DIR_SRCS)

SET(THIRDPARTY "./thirdParty/cJSON.h" "./thirdParty/cJSON.c")
SOURCE_GROUP(thirdParty FILES ${THIRDPARTY})

SET(SRC "./srtuctMacro.h" "./JSONReader.h" "./JSONReader.cpp" "./JSONWriter.h" "./JSONWriter.cpp" "./main.cpp")
SOURCE_GROUP(src FILES ${SRC})

ADD_EXECUTABLE(struct ${THIRDPARTY} ${SRC} ${DIR_SRCS})