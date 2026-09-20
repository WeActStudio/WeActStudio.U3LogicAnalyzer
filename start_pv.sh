SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" &>/dev/null && pwd)
export PYTHONHOME=/mingw64
export SIGROKDECODE_DIR=${SCRIPT_DIR}/install/share/libsigrokdecode/decoders
cd build_cmake/logicanalyzer_build
./LogicAnalyzer.exe
cd ../..