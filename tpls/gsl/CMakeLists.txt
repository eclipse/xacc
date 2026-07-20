cmake_minimum_required(VERSION 3.0.0)
project(GSL)

set(GSL_URL "https://mirror.fcix.net/gnu/gsl/gsl-latest.tar.gz" CACHE STRING "GSL download URL")

set(DOWNLOAD_PATH "${CMAKE_SOURCE_DIR}/tpls/gsl/gsl.tar.gz")
set(EXTRACT_PATH "${CMAKE_SOURCE_DIR}/tpls/gsl/gsl")

if(NOT EXISTS "${DOWNLOAD_PATH}")
file(DOWNLOAD ${GSL_URL} "${DOWNLOAD_PATH}.tmp"
STATUS download_status
LOG download_log
SHOW_PROGRESS
)

list(GET download_status 0 download_status_code)
list(GET download_status 1 download_status_message)
if(download_status_code)
  file(REMOVE "${DOWNLOAD_PATH}.tmp")
  message(FATAL_ERROR "Download failed. Status: ${download_status_code} ${download_status_message}\nLog: ${download_log}")
endif()

file(RENAME "${DOWNLOAD_PATH}.tmp" "${DOWNLOAD_PATH}")
endif()

if(NOT IS_DIRECTORY "${EXTRACT_PATH}")
file(MAKE_DIRECTORY "${EXTRACT_PATH}.tmp")
execute_process(
  COMMAND ${CMAKE_COMMAND} -E tar xfv ${DOWNLOAD_PATH}
  WORKING_DIRECTORY "${EXTRACT_PATH}.tmp"
  OUTPUT_QUIET
)
file(RENAME "${EXTRACT_PATH}.tmp" "${EXTRACT_PATH}")
endif()

file(GLOB download_gsl_root "${EXTRACT_PATH}/gsl-*")
if(download_gsl_root)
  set(GSL_SOURCE "${download_gsl_root}" CACHE STRING "GSL location" FORCE)
endif()

file(STRINGS "${GSL_SOURCE}/gsl_version.h" gsl_version_raw
  REGEX "define GSL_VERSION "
)

string(REGEX MATCH "([0-9]+\\.[0-9]+)" GSL_VERSION "${gsl_version_raw}")