# https://github.com/jurihock/qdft

CPMAddPackage(
  NAME qdft
  VERSION 0.5
  GIT_TAG e1a3cc32392b3902bc65432de03c27305a5cc463
  GITHUB_REPOSITORY jurihock/qdft
  DOWNLOAD_ONLY YES)

if(qdft_ADDED)

  add_library(qdft INTERFACE)

  target_include_directories(qdft
    INTERFACE "${qdft_SOURCE_DIR}/cpp/src")

endif()
