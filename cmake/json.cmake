set(NLOHMANN_JSON_DOWNLOAD_VERSION 3.11.3)

set(NLOHMANN_JSON_INCLUDE_DIR "${CMAKE_BINARY_DIR}/cmake/nlohamn-json-${NLOHMANN_JSON_DOWNLOAD_VERSION}/include/")
set(NLOHMANN_JSON_DOWNLOAD_LOCATION "${NLOHMANN_JSON_INCLUDE_DIR}/nlohmann/json.hpp")
if (NOT (EXISTS ${NLOHMANN_JSON_DOWNLOAD_LOCATION}))
    file(
        DOWNLOAD
        "https://github.com/nlohmann/json/releases/download/v${NLOHMANN_JSON_DOWNLOAD_VERSION}/json.hpp"
        ${NLOHMANN_JSON_DOWNLOAD_LOCATION}
    )
endif()
include_directories(${NLOHMANN_JSON_INCLUDE_DIR})