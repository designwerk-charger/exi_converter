#!/bin/bash

if [ ! -f scripts/all.sh ];then  echo "Wrong Path, execute from correct path!";exit 127; fi
source scripts/all.sh

###################################
# Global variables
BASE_DIR=$(pwd)
BUILD_DIR=$BASE_DIR"/build"
UNITTEST_REPORTS_DIR="$BASE_DIR/test-reports/"
UNITTEST_COVERAGE_DIR="$BASE_DIR/test-coverage/"


function printHelp {
    logLine "Designwerk EXI converter library run script help."
    logLine "Usage ./run.sh <command>"
    logLine ""
    logLine "<command>                  Description"
    logLine "unittests                  Runs all unittests and code coverage (runs clean before)"
    logLine "gtest                      Runs Google Test unittests (c++)"
    logLine "gtest <foo.*|*.bar>        Runs the unittests matching the filter criteria"
    logLine "clean                      Removes the test-reports, test-coverage and build directory"
}


function clean {
    rm -rf "$UNITTEST_REPORTS_DIR"
    rm -rf "$UNITTEST_COVERAGE_DIR"
    rm -rf "$BUILD_DIR"
}

function setupUnittestOutputDirectory {
    mkdir -p "$UNITTEST_REPORTS_DIR" "$UNITTEST_COVERAGE_DIR"
    checkSuccess $? "Create Unittest output directories"
}

case "$1" in
    unittests)
        clean
        setupUnittestOutputDirectory
        runCMakeUnittestsAndCoverage "$BASE_DIR/" exi_converter_test
        ;;
    gtest)
        setupUnittestOutputDirectory
        runCMakeUnittestsAndCoverage "$BASE_DIR/" exi_converter_test "$2"
        ;;
    clean)
        clean
        ;;
    help)
	    printHelp
	    ;;
    *)
	    logTip "Command $1 not recognized"
	    printHelp
	;;
esac


