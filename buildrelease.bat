mkdir build_win
cd build_win
cmake ..
cmake --build . --config Release --parallel 8
cpack -G WIX -D CPACK_PACKAGE_FILE_NAME=openxr-explorer-win-x64
cd ..
xcopy build_win\openxr-explorer-win-x64.msi openxr-explorer-win-x64.msi* /Y
powershell "Compress-Archive -Force -Path build_win\Release\openxr-explorer.exe, build_win\Release\xrsetruntime.exe -DestinationPath openxr-explorer-win-x64.zip"

mkdir build_linux
cd build_linux
wsl cmake .. -DCMAKE_BUILD_TYPE=Release
wsl cmake --build . --parallel 8
cd ..
powershell "Compress-Archive -Force -Path build_linux\openxr-explorer, build_linux\xrsetruntime -DestinationPath openxr-explorer-linux-x64.zip"