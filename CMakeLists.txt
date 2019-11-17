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

SET(SRC "./include/struct2x.hpp" "./include/SelfAdapt.h" "./include/structExternalSerialize.h" "./include/bufferInterface.h"
 "./include/JSONReader.h" "./src/JSONReader.cpp" "./include/JSONWriter.h" "./src/JSONWriter.cpp"
 "./include/BufferWriter.h" "./src/BufferWriter.cpp" "./include/BufferReader.h" "./src/BufferReader.cpp")
SOURCE_GROUP(src FILES ${SRC})

LIST(REMOVE_ITEM DIR_SRCS "./tester/main.cpp")
ADD_LIBRARY(struct2x ${THIRDPARTY} ${SRC} ${DIR_SRCS})


SET(TESTER "./tester/buffer.h" "./tester/main.cpp")
SOURCE_GROUP(tester FILES ${TESTER})

ADD_EXECUTABLE(tester ${TESTER})
TARGET_LINK_LIBRARIES(tester struct2x)
