PROJECT(struct)

include(CheckIncludeFileCXX)

CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

AUX_SOURCE_DIRECTORY(./ DIR_SRCS)

SET(SRC "./cJSON.h" "./cJSON.c"  "./JSONReader.h" "./JSONReader.cpp" "./JSONWriter.h" "./JSONWriter.cpp" "./main.cpp")
SOURCE_GROUP(src FILES ${SRC})

SET(MATH ${DIR_SRCS})

ADD_EXECUTABLE(struct ${SRC} ${MATH})