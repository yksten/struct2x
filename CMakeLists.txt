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
SOURCE_GROUP(cjson FILES ${CJSON})

SET(RAPIDJSON "./thirdParty/rapidjson/error/en.h" "./thirdParty/rapidjson/error/error.h" "./thirdParty/rapidjson/internal/biginteger.h" "./thirdParty/rapidjson/internal/diyfp.h" "./thirdParty/rapidjson/internal/dtoa.h" "./thirdParty/rapidjson/internal/ieee754.h" "./thirdParty/rapidjson/internal/itoa.h" "./thirdParty/rapidjson/internal/meta.h" "./thirdParty/rapidjson/internal/pow10.h" "./thirdParty/rapidjson/internal/regex.h" "./thirdParty/rapidjson/internal/stack.h" "./thirdParty/rapidjson/internal/strfunc.h" "./thirdParty/rapidjson/internal/strtod.h" "./thirdParty/rapidjson/internal/swap.h" "./thirdParty/rapidjson/msinttypes/inttypes.h" "./thirdParty/rapidjson/msinttypes/stdint.h" "./thirdParty/rapidjson/allocators.h" "./thirdParty/rapidjson/document.h" "./thirdParty/rapidjson/encodedstream.h" "./thirdParty/rapidjson/encodings.h" "./thirdParty/rapidjson/filereadstream.h" "./thirdParty/rapidjson/filewritestream.h" "./thirdParty/rapidjson/fwd.h" "./thirdParty/rapidjson/istreamwrapper.h" "./thirdParty/rapidjson/memorybuffer.h" "./thirdParty/rapidjson/memorystream.h" "./thirdParty/rapidjson/ostreamwrapper.h" "./thirdParty/rapidjson/pointer.h" "./thirdParty/rapidjson/prettywriter.h" "./thirdParty/rapidjson/rapidjson.h" "./thirdParty/rapidjson/reader.h" "./thirdParty/rapidjson/schema.h" "./thirdParty/rapidjson/stream.h" "./thirdParty/rapidjson/stringbuffer.h" "./thirdParty/rapidjson/writer.h")
SOURCE_GROUP(rapidjson FILES ${RAPIDJSON})

SET(HEADER "./include/struct2x.h" "./include/json/JSONReader.h" "./include/json/JSONWriter.h" "./include/protobuf/encoder.h" "./include/protobuf/decoder.h" "./include/rapidjson/rapidjsonEncoder.h" "./include/rapidjson/rapidjsonDecoder.h")
SOURCE_GROUP(header FILES ${HEADER})

SET(SRC "./src/struct2x.cpp" "./src/json/JSONReader.cpp" "./src/json/JSONWriter.cpp" "./src/protobuf/encoder.cpp" "./src/protobuf/decoder.cpp" "./src/rapidjson/rapidjsonEncoder.cpp" "./src/rapidjson/rapidjsonDecoder.cpp")
SOURCE_GROUP(src FILES ${SRC})

SET(PARSE "./src/protobuf/parse/proto.h" "./src/protobuf/parse/proto.cpp")
SOURCE_GROUP(parse FILES ${PARSE})

LIST(REMOVE_ITEM DIR_SRCS "./tester/main.cpp")
IF (MSVC)
    ADD_LIBRARY(struct2x SHARED ${CJSON} ${RAPIDJSON} ${PARSE} ${HEADER} ${SRC} ${DIR_SRCS})
ELSE ()
    ADD_LIBRARY(struct2x ${CJSON} ${RAPIDJSON} ${PARSE} ${HEADER} ${SRC} ${DIR_SRCS})
ENDIF (MSVC)


SET(TESTER "./tester/main.cpp" "./tester/testStruct.h")
SOURCE_GROUP(tester FILES ${TESTER})

ADD_EXECUTABLE(tester ${TESTER})
TARGET_LINK_LIBRARIES(tester struct2x)
