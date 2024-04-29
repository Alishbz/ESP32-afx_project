cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)

set(EXTRA_COMPONENT_DIRS src)

set(DEVICE_ID 1)
message("\r\n-- DEVICE_ID:" ${DEVICE_ID})
set(application_name "esp32_canrgb")
set(application_version "0.0.2")

set(PROJECT_VER ${application_version}_D${DEVICE_ID})
add_definitions(-DDEVICE_ID=${DEVICE_ID})
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# idf.py -DCMAKE_BUILD_TYPE=Debug reconfigure
# idf.py -DCMAKE_BUILD_TYPE=Release reconfigure
if(CMAKE_BUILD_TYPE MATCHES Debug)
  set(opt_level -O0)
  add_definitions(-DDEBUG_BUILD)
else()
  set(opt_level -Os)
endif()
 
project(${application_name})
