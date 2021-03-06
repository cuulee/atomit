cmake_minimum_required(VERSION 2.8)


#####################################################################
## Build parameters of the Atom-IT server
#####################################################################

add_definitions(
  -DATOMIT_VERSION="mainline"
  )

set(USE_SYSTEM_PAHO ON CACHE BOOL "Use the system version of Paho MQTT (C library)")
set(ENABLE_IMST_GATEWAY ON CACHE BOOL "Enable support for Raspberry Pi and IMST iC880A")



#####################################################################
## Configuration of third-party dependencies
#####################################################################

include(${CMAKE_CURRENT_SOURCE_DIR}/Resources/CMake/OrthancFramework.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Resources/CMake/PahoMQTT.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Resources/CMake/IMSTGateway.cmake)



#####################################################################
## Embedding of resources
#####################################################################

set(ATOMIT_EMBEDDED_FILES
  PREPARE_SQLITE_DATABASE
  ${CMAKE_CURRENT_SOURCE_DIR}/Framework/TimeSeries/SQLiteBackend/PrepareDatabase.sql
  )

if (STANDALONE_BUILD)
  # We embed all the resources in the binaries for standalone builds
  add_definitions(
    -DATOMIT_STANDALONE=1
    )
  EmbedResources(
    ${ATOMIT_EMBEDDED_FILES}
    ATOMIT_WEB_INTERFACE  ${CMAKE_CURRENT_SOURCE_DIR}/WebInterface/
    )
else()
  add_definitions(
    -DATOMIT_STANDALONE=0
    -DATOMIT_ROOT=\"${CMAKE_SOURCE_DIR}\"
    )
  EmbedResources(
    ${ATOMIT_EMBEDDED_FILES}
    )
endif()



#####################################################################
## Compilation
#####################################################################

include_directories(${ORTHANC_ROOT})

set(ATOMIT_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/Framework/AtomITToolbox.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Framework/ConfigurationSection.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Framework/FileWritersPool.cpp  
  ${CMAKE_CURRENT_SOURCE_DIR}/Framework/Filters/AdapterFilter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Framework/Filters/CSVFileSinkFilter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Framework/Filters/CSVFileSourceFilter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Framework/Filters/CounterSourceFilter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Framework/Filters/DemultiplexerFilter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Framework/Filters/FileLinesSourceFilter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Framework/Filters/FileReaderFilter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Framework/Filters/HttpPostSinkFilter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Framework/Filters/IMSTSourceFilter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Framework/Filters/LoRaPacketFilter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Framework/Filters/LuaFilter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Framework/Filters/MQTTSinkFilter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Framework/Filters/MQTTSourceFilter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Framework/Filters/SharedFileSinkFilter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Framework/Filters/SourceFilter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Framework/LoRa/FrameEncryptionKey.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Framework/LoRa/LoRaEnumerations.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Framework/LoRa/LoRaToolbox.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Framework/LoRa/MACPayload.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Framework/LoRa/PHYPayload.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Framework/LoRa/UnsignedInteger128.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Framework/MQTT/Broker.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Framework/MQTT/MQTTClientWrapper.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Framework/MQTT/SynchronousClient.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Framework/Message.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Framework/TimeSeries/GenericTimeSeriesManager.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Framework/TimeSeries/MemoryBackend/MemoryTimeSeriesBackend.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Framework/TimeSeries/MemoryBackend/MemoryTimeSeriesContent.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Framework/TimeSeries/SQLiteBackend/SQLiteDatabase.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Framework/TimeSeries/SQLiteBackend/SQLiteTimeSeriesBackend.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Framework/TimeSeries/SQLiteBackend/SQLiteTimeSeriesTransaction.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Framework/TimeSeries/TimeSeriesReader.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Framework/TimeSeries/TimeSeriesWriter.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/Resources/ThirdParty/tiny-AES-c-master/aes.c
  ${IMST_GATEWAY_SOURCES}  
  ${PAHO_SOURCES}

  ${ORTHANC_CORE_SOURCES}
  ${ORTHANC_SOURCES_DIR}/OrthancServer/OrthancHttpHandler.cpp
  ${AUTOGENERATED_SOURCES}
  )

add_library(AtomITFramework STATIC
  ${ATOMIT_SOURCES}
  )

add_executable(AtomIT
  Applications/AtomITRestApi.cpp
  Applications/FilterFactory.cpp
  Applications/MainTimeSeriesFactory.cpp
  Applications/ServerContext.cpp
  Applications/main.cpp
  )

add_executable(UnitTests
  UnitTestsSources/LoRaTests.cpp
  UnitTestsSources/TimeSeriesTests.cpp
  UnitTestsSources/UnitTests.cpp
  ${GOOGLE_TEST_SOURCES}
  )

target_link_libraries(AtomIT AtomITFramework)
target_link_libraries(UnitTests ${GOOGLE_TEST_LIBRARIES} AtomITFramework)

install(
  TARGETS AtomIT
  RUNTIME DESTINATION sbin
  )


#####################################################################
## Generate the documentation if Doxygen is present
#####################################################################

find_package(Doxygen)

if (DOXYGEN_FOUND)
  configure_file(
    ${CMAKE_SOURCE_DIR}/Resources/AtomIT.doxygen
    ${CMAKE_CURRENT_BINARY_DIR}/AtomIT.doxygen
    @ONLY)

  add_custom_target(doc
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/AtomIT.doxygen
    COMMENT "Generating internal documentation with Doxygen" VERBATIM
    )
else()
  message("Doxygen not found. The documentation will not be built.")
endif()
