#!/bin/bash
# Buildroot only runs shell scripts

#####################################
# Creates the version.py file used used to set the package version by setup.py
#
# ./set-version.sh <path to version.py> <fallback version>
#   1. Try to get version by 'git describe' from current working directory
#   2. If that fails, it uses the 'fallback version'
#####################################


VERSION=$(git describe)
if [ $? -ne 0 ]; then
    echo "'$(pwd)' not a git directory?"
    if [ ! -z "$2" ]; then
        VERSION=$2
        echo "Using argument '$VERSION' as version."
    else
        echo "Could not determine the current version from git or argument '$2'"
        exit 1
    fi
else
    echo "Using git describe '$VERSION' as version."
fi

VERSION=$(echo "$VERSION" | grep -Eo '[0-9]+\.[0-9]+\.[0-9]+')
if [[ "$VERSION" == "" ]]; then
    echo "Illegal version format detected, using VERSION=0.0.0."
    VERSION="0.0.0"
fi

if [ ! -d "$1" ]; then
    echo "Directory '$1' does not exist. Cannot create version file!"
    exit 1
fi

echo "CURRENT_VERSION = \"$VERSION\"" > "$1/version.py"
