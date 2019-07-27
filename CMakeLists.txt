PROJECT(struct)
CMAKE_MINIMUM_REQUIRED(VERSION 3.1)
INCLUDE(CheckIncludeFileCXX)

SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)

INCLUDE_DIRECTORIES(./ ./include/)
AUX_SOURCE_DIRECTORY(. DIR_SRCS)

SET(THIRDPARTY "./thirdParty/cJSON.h" "./thirdParty/cJSON.c")
SOURCE_GROUP(thirdParty FILES ${THIRDPARTY})

SET(SRC "./include/srtuctMacro.h" "./include/SelfAdapt.h" "./include/srtuctNoInvasiveMacro.h"
 "./include/JSONReader.h" "./src/JSONReader.cpp" "./include/JSONWriter.h" "./src/JSONWriter.cpp"
 "./include/BufferWriter.h" "./src/BufferWriter.cpp" "./include/BufferReader.h" "./src/BufferReader.cpp")
SOURCE_GROUP(src FILES ${SRC})

LIST(REMOVE_ITEM DIR_SRCS "./tester/main.cpp")
ADD_LIBRARY(struct ${THIRDPARTY} ${SRC} ${DIR_SRCS})


SET(TESTER "./tester/main.cpp")
SOURCE_GROUP(tester FILES ${TESTER})

ADD_EXECUTABLE(tester ${TESTER})
TARGET_LINK_LIBRARIES(tester struct)
