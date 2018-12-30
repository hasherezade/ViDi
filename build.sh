echo "Trying to build ViDi..."

#QT check
QT_VER=`qmake -v`
QTV="version "
if echo "$QT_VER" | grep -q "$QTV"; then
    echo "[+] Qt5 found!"
else
    echo "[-] Qt5 NOT found!"
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
echo $CMAKE_VER
cd ..
mkdir build
echo "[+] build directory created"
cd build
cmake -G "Unix Makefiles" ../
make
cd ..
cp build/vidi/vidi ./build/ViDi
ls ViDi
echo "Done!"
pwd

