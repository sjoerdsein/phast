#!/bin/sh

set -e
mkdir -p build
cd build
qmake -r ../phast.pro
make
cd ..
mkdir -p deployed

if [ "$(uname)" = "Linux" ]; then
    ./deploy_linux.sh
else
    ./deploy.sh
fi

echo
echo "Compilation and deployment complete"