#!/bin/sh

# Directory containing the test binaries
set -e

# Parse arguments
TEST_BIN_DIR=""
while [ "$#" -gt 0 ]; do
    case "$1" in
        --build-dir)
            TEST_BIN_DIR="$2"
            shift 2
            ;;
        --build-dir=*)
            TEST_BIN_DIR="${1#*=}"
            shift 1
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

if [ -z "${TEST_BIN_DIR}" ]; then
    echo "Please provide the build directory by using the --build-dir option"
    exit 1
fi

# Set XCRUN based on the output of uname
if [ "$(uname)" = "Darwin" ]; then
    XCRUN="xcrun"
else
    XCRUN=""
fi

# Directory to store the coverage report
# TODO, add into build dir
COVERAGE_REPORT_DIR="${TEST_BIN_DIR}/coverage_report"

# Remove any existing .profraw files
# find ${TEST_BIN_DIR} -name "*.profraw" -delete

# Find all .profraw files
ALL_PROFILES=$(find ${TEST_BIN_DIR} -name '*.profraw')
ALL_BINARIES=''
for profile in ${ALL_PROFILES}; do
    binary=$(basename -s .profraw "${profile}")
    binary_path=$(find "${TEST_BIN_DIR}" -type f -name "${binary}" -print -quit)
    if [ -z "${binary_path}" ]; then
        echo "Binary  ${binary}  for profile ${profile} not found in ${TEST_BIN_DIR}"
        # exit 1
    else
        ALL_BINARIES="${ALL_BINARIES} ${binary_path}"
    fi
done

COVERAGE_COMBINED_FILE="${TEST_BIN_DIR}/coverage-all.profraw"

# Merge all .profraw files into a single .profdata file
rm -f ${COVERAGE_COMBINED_FILE}
echo "Merging profiles into ${COVERAGE_COMBINED_FILE}"
${XCRUN} llvm-profdata merge -sparse ${ALL_PROFILES} -o ${COVERAGE_COMBINED_FILE}

IGNORE_REGEX="${TEST_BIN_DIR}/_deps/.*|tests/.*"
echo "Generating plain coverage report"
# Generate the plain text coverage report for each binary
for binary in ${ALL_BINARIES}; do
    ${XCRUN} llvm-cov show ${binary} \
        -instr-profile=${COVERAGE_COMBINED_FILE} \
        -ignore-filename-regex=${IGNORE_REGEX} \
        -output-dir=${COVERAGE_REPORT_DIR}/plain
done

# Generate an HTML coverage report for each binary
echo "Generating HTML coverage report"
for binary in ${ALL_BINARIES}; do
    ${XCRUN} llvm-cov show ${binary} \
        -instr-profile=${COVERAGE_COMBINED_FILE} \
        -ignore-filename-regex=${IGNORE_REGEX} \
        -format=html \
        -output-dir=${COVERAGE_REPORT_DIR}/html
done

# Open the HTML report
echo "Opening the HTML coverage report: ${COVERAGE_REPORT_DIR}/html/index.html"
#open ${COVERAGE_REPORT_DIR}/html/index.html

