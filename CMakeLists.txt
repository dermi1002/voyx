cmake_minimum_required(VERSION 3.12...3.18)

project(voyx)

if (MSVC)
  # add_compile_options(/W3 /WX)
else()
  # add_compile_options(-Wall -Werror)
endif()

include("${CMAKE_CURRENT_LIST_DIR}/lib/cpm.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/lib/qt.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/lib/qcustomplot.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/lib/cxxopts.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/lib/dr.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/lib/easyloggingpp.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/lib/fmt.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/lib/mlinterp.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/lib/pocketfft.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/lib/qdft.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/lib/readerwriterqueue.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/lib/rtaudio.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/lib/rtmidi.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/lib/sdft.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/lib/stftpitchshift.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/lib/xtensor.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/lib/xtl.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/src/voyx/voyx.cmake")
