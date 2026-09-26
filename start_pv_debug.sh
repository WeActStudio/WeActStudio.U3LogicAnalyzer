SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" &>/dev/null && pwd)
if [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]] || [[ -n "$MSYSTEM" ]]; then
    export PYTHONHOME=/mingw64
    BIN=./LogicAnalyzer.exe
else
    # Linux / macOS: use the system Python (no PYTHONHOME needed) and point
    # the loader at the freshly built sigrok libraries.
    export LD_LIBRARY_PATH="${SCRIPT_DIR}/install/lib:${LD_LIBRARY_PATH}"
    BIN=./LogicAnalyzer
fi
export SIGROKDECODE_DIR=${SCRIPT_DIR}/install/share/libsigrokdecode/decoders
cd build_cmake/logicanalyzer_build
gdb --args "$BIN" -l 5
cd ../..
