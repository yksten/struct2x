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

# rapidjson thirdparty
SET(RAPIDJSONERROR "./thirdParty/rapidjson/error/en.h" "./thirdParty/rapidjson/error/error.h")
SOURCE_GROUP("thirdParty\\rapidjson\\error" FILES ${RAPIDJSONERROR})
SET(RAPIDJSONINTERNAL "./thirdParty/rapidjson/internal/biginteger.h" "./thirdParty/rapidjson/internal/diyfp.h" "./thirdParty/rapidjson/internal/dtoa.h" "./thirdParty/rapidjson/internal/ieee754.h" "./thirdParty/rapidjson/internal/itoa.h" "./thirdParty/rapidjson/internal/meta.h" "./thirdParty/rapidjson/internal/pow10.h" "./thirdParty/rapidjson/internal/regex.h" "./thirdParty/rapidjson/internal/stack.h" "./thirdParty/rapidjson/internal/strfunc.h" "./thirdParty/rapidjson/internal/strtod.h" "./thirdParty/rapidjson/internal/swap.h")
SOURCE_GROUP("thirdParty\\rapidjson\\internal" FILES ${RAPIDJSONINTERNAL})
SET(RAPIDJSONMSINTTYPES "./thirdParty/rapidjson/msinttypes/inttypes.h" "./thirdParty/rapidjson/msinttypes/stdint.h")
SOURCE_GROUP("thirdParty\\rapidjson\\msinttypes" FILES ${RAPIDJSONMSINTTYPES})
SET(RAPIDJSON "./thirdParty/rapidjson/allocators.h" "./thirdParty/rapidjson/document.h" "./thirdParty/rapidjson/encodedstream.h" "./thirdParty/rapidjson/encodings.h" "./thirdParty/rapidjson/filereadstream.h" "./thirdParty/rapidjson/filewritestream.h" "./thirdParty/rapidjson/fwd.h" "./thirdParty/rapidjson/istreamwrapper.h" "./thirdParty/rapidjson/memorybuffer.h" "./thirdParty/rapidjson/memorystream.h" "./thirdParty/rapidjson/ostreamwrapper.h" "./thirdParty/rapidjson/pointer.h" "./thirdParty/rapidjson/prettywriter.h" "./thirdParty/rapidjson/rapidjson.h" "./thirdParty/rapidjson/reader.h" "./thirdParty/rapidjson/schema.h" "./thirdParty/rapidjson/stream.h" "./thirdParty/rapidjson/stringbuffer.h" "./thirdParty/rapidjson/writer.h")
SOURCE_GROUP("thirdParty\\rapidjson" FILES ${RAPIDJSON})

# serialize
SET(INCLUDE "./include/serialize.h")
SOURCE_GROUP(include FILES ${INCLUDE})

# cjson
SET(INCLUDECJSON "./include/cjson/CJSONEncoder.h" "./include/cjson/CJSONDecoder.h")
SOURCE_GROUP("include\\cjson" FILES ${INCLUDECJSON})
SET(SRCCJSON "./src/cjson/CJSONEncoder.cpp" "./src/cjson/CJSONDecoder.cpp")
SOURCE_GROUP("src\\cjson" FILES ${SRCCJSON})

# rapidjson
SET(INCLUDERAPIDJSON "./include/rapidjson/encoder.h" "./include/rapidjson/decoder.h")
SOURCE_GROUP("include\\rapidjson" FILES ${INCLUDERAPIDJSON})
SET(SRCRAPIDJSON "./src/rapidjson/encoder.cpp" "./src/rapidjson/decoder.cpp")
SOURCE_GROUP("src\\rapidjson" FILES ${SRCRAPIDJSON})

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
    ${RAPIDJSONERROR} ${RAPIDJSONINTERNAL} ${RAPIDJSONMSINTTYPES} ${RAPIDJSON} 
    ${INCLUDE} 
    ${INCLUDECJSON} ${SRCCJSON} 
    ${INCLUDERAPIDJSON} ${SRCRAPIDJSON} 
    ${INCLUDEJSON} ${SRCJSON} 
    ${INCLUDEPROTOBUF} ${SRCPROTOBUF} 
    ${DIR_SRCS})
ELSE ()
    ADD_LIBRARY(serialize 
    ${CJSON} 
    ${RAPIDJSONERROR} ${RAPIDJSONINTERNAL} ${RAPIDJSONMSINTTYPES} ${RAPIDJSON} 
    ${INCLUDE} 
    ${INCLUDECJSON} ${SRCCJSON} 
    ${INCLUDERAPIDJSON} ${SRCRAPIDJSON} 
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
