#!/bin/bash
echo "Trying to autobuild ViDi..."

#QT check
QT_VER=`qmake -v`
QTV="version 4."
if echo "$QT_VER" | grep -q "$QTV"; then
    echo "[+] Qt4 found!"
else
    echo "[-] Qt4 NOT found!"
    echo "Install qt-sdk first"
    exit -1
fi

CMAKE_VER=`cmake --version`
CMAKEV="cmake version"
if echo "$CMAKE_VER" | grep -q "$CMAKEV"; then
    echo "[+] CMake found!"
else
    echo "[-] CMake NOT found!"
    echo "Install cmake first"
    exit -1
fi

mkdir ViDi
cd ViDi
git clone https://github.com/hasherezade/ViDi.git
echo "[+] ViDi cloned"
mv ViDi src
cd src
git clone https://github.com/hasherezade/bearparser.git
echo "[+] bearparser cloned"
git clone https://github.com/aquynh/capstone.git
echo "[+] capstone cloned"
echo "[+] capstone : switching to branch 'next'"
cd capstone
git checkout next
cd ..
cd ..
mkdir build
echo "[+] build directory created"
cd build
cmake -G "Unix Makefiles" ../src/
make
cd ..
cp build/vidi/vidi ./
echo "[+] Success! ViDi is here:"
pwd
