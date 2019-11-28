#!/bin/bash
echo "Trying to rebuild ViDi..."

cd build
cmake --build . --target install --config Release
cd ..


