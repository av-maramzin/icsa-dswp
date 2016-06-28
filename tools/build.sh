#! /bin/bash

SELF_DIR=$(dirname $0)
PROJECT_ROOT=${PWD}/${SELF_DIR}/../
PROJECT_BUILD_REL_DIR="build"
PROJECT_INSTALL_REL_DIR="install"

#

echo "project root dir: ${PROJECT_ROOT}"
echo "project relative build dir: ${PROJECT_BUILD_REL_DIR}"
echo "project relative install dir: ${PROJECT_INSTALL_DIR}"

if [ ! -d "${PROJECT_BUILD_REL_DIR}" ]; then
  echo "creating build directory"
  mkdir -p "${PROJECT_BUILD_REL_DIR}"

  echo "entering build directory"
  pushd "${PROJECT_BUILD_REL_DIR}"

  echo "configuring build"

# notes on cmake call

#- CC/CXX env variables dictate the compiler cmake should use
#- LLVM_DIR provides a hint for cmake's find_package for LLVM
#- CMAKE_EXPORT_COMPILE_COMMANDS exports the project's compilation db
#  (e.g. use with YCM)

LINKER_FLAGS="-Wl,-rpath-link=$(llvm-config --libdir) -lc++ -lc++abi" 

  CC=clang CXX=clang++ \
  cmake \
    -DCMAKE_POLICY_DEFAULT_CMP0056=NEW \
    -DLLVM_DIR=$(llvm-config --prefix)/share/llvm/cmake/ \
    -DCMAKE_BUILD_TYPE=DEBUG \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DCMAKE_CXX_FLAGS="-stdlib=libc++" \
    -DCMAKE_EXE_LINKER_FLAGS="${LINKER_FLAGS}" \
    -DCMAKE_SHARED_LINKER_FLAGS="${LINKER_FLAGS}" \
    -DCMAKE_MODULE_LINKER_FLAGS="${LINKER_FLAGS}" \
    -DCMAKE_INSTALL_RPATH="$(llvm-config --libdir)" \
    -DCMAKE_INSTALL_PREFIX="${PROJECT_INSTALL_DIR}" \
    "${PROJECT_ROOT}"

  popd # ${PROJECT_BUILD_REL_DIR}
fi


echo "entering build directory"
pushd "${PROJECT_BUILD_REL_DIR}"

echo "building"
make
BUILD_RESULT="$?"

popd # ${PROJECT_BUILD_REL_DIR}


exit "${BUILD_RESULT}"

