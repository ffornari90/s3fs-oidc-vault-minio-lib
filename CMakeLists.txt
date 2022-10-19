cmake_minimum_required(VERSION 3.12)
project(vault_minio VERSION 0.51.0 DESCRIPTION "Software using Vault bindings for C++ to provide MinIO STS")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CURL)

#
# Set Library information
#
set(LIB_NAME   "oidc-vault-minio")
set(LIB_SRC    "src/main.cpp" "src/readSecretValue.cpp" "src/configureClient.cpp" "src/getOIDCToken.cpp" "src/s3fsFuncs.cpp" "src/Logger.cpp")
set(LIB_HEADER "src/main.hpp" "src/Logger.hpp")
set(LIB_SAMPLE "src/oidc_vault_minio_test.cpp")
set(LIB_TYPE   "SHARED")

#
# For building Library
#
add_library(${LIB_NAME} ${LIB_TYPE} ${LIB_SRC})
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} /usr/local/include)
target_link_libraries(${LIB_NAME} /usr/lib/x86_64-linux-gnu/liboidc-agent.so /usr/local/lib/libvault.so curl)

#
# For building test file.
#
link_directories(${INSTALL_DIR}/lib)
add_executable("${LIB_NAME}_test" ${LIB_SAMPLE})
target_include_directories("${LIB_NAME}_test" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${INSTALL_DIR}/include)
target_link_libraries("${LIB_NAME}_test" ${LIB_NAME})

#
# Specify Install Folder
#
install(TARGETS "${LIB_NAME}" LIBRARY DESTINATION "${INSTALL_DIR}/lib")
