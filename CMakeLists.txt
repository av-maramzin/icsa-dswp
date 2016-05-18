cmake_minimum_required(VERSION 2.8.12.2)
project(icsa-dswp)

# CMP0056 is required for any libc++abi dependency of the try_compile() command
# otherwise LD_LIBRARY_DIR needs to be set which is a less than optimal
set(PROJECT_POLICIES CMP0056)

foreach(PROJECT_POLICY ${PROJECT_POLICIES})
  if(POLICY ${PROJECT_POLICY})
    cmake_policy(GET ${PROJECT_POLICY} POLICY_VALUE)
    message("policy ${PROJECT_POLICY}: ${POLICY_VALUE}")
  endif()
endforeach()


add_subdirectory(pass)
add_subdirectory(runtime)
