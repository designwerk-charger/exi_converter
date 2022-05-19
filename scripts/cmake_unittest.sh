# Runs Google Tests with the given Testdirectory and creates Coverage for the Sources
#  $1: Testdirectory (where sources and test folders are located)
#  $2: Test target
function runCMakeUnittestsAndCoverage {
    _TEST_DIR=$1
    _TEST_TARGET=$2
    _TEST_BINARY="$BUILD_DIR/test/$_TEST_TARGET"
    _TEST_RAPPORT="${UNITTEST_REPORTS_DIR}/${_TEST_TARGET}_report.xml"
    _GENHTML_BINARY="${BASE_DIR}/testlib/lcov-master-3926d58f/bin/genhtml"
    _LCOV_BINARY="${BASE_DIR}/testlib/lcov-master-3926d58f/bin/lcov"

    if [ "$3" ]; then
        logInfo "Just run the following test: $3"
        _buildGTest
        _runSingleGTest $3
    else
        logInfo "Run all unittests"
        _buildGTest
        _runGTest
        _createCppCoverageReport
    fi

    # modify date of test because bamboo rejects the test sometiemes due to a
    #   File .../test-reports/unittest_report.xml was ignored because it was modified (...) before task started (...)
    touch "$_TEST_RAPPORT"
}


function _buildGTest {
    mkdir -p $BUILD_DIR
    pushd $BUILD_DIR > /dev/null
    checkSuccess $? "Change to build dir '$BUILD_DIR'"

    cmake $_TEST_DIR -DUSE_GCOV=ON
    checkSuccess $? "Run cmake $_TEST_DIR with coverage enabled"

    cmake --build . --target $_TEST_TARGET
    checkSuccess $? "Build $_TEST_TARGET"

    popd > /dev/null
}


function _runGTest {
    __run="$_TEST_BINARY --gtest_output=xml:$_TEST_RAPPORT"
    logCmd "$__run"
    bash -c "$__run"
    checkSuccess $? "Running Unittests"
}

# $1: Filtered test 
function _runSingleGTest {
    __run="$_TEST_BINARY --gtest_filter=$1"
    logCmd "$__run"
    bash -c "$__run"
    checkSuccess $? "Running Unittest $1"
}

 
function _createCppCoverageReport {
    pushd $UNITTEST_COVERAGE_DIR > /dev/null
    __run="$_LCOV_BINARY -c -d $BUILD_DIR/lib/CMakeFiles/exi_converter.dir/ --include '*lib/*' -o exi_converter.info"
    logCmd "$__run"
    bash -c "$__run"
    checkSuccess $? "Create Coverage info file"

    __run="$_GENHTML_BINARY exi_converter.info --title 'Designwerk EXI Converter Library' -o $UNITTEST_COVERAGE_DIR/cpp_html"
    logCmd "$__run"
    bash -c "$__run"
    checkSuccess $? "Generate HTML Coverage report"
}