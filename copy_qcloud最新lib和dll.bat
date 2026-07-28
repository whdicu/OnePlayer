@echo off
set SRC=D:\Projects\QCloudMusicApi\build\QCloudMusicApi\Release

echo ========================================
echo   Copy QCloudMusicApi Build Output
echo ========================================
echo.

if not exist "%SRC%" (
    echo Source dir not found: %SRC%
    pause
    exit /b 1
)

echo Copying .lib to QCloudMusicApi\lib...
if not exist "QCloudMusicApi\lib" mkdir "QCloudMusicApi\lib"
copy /Y "%SRC%\*.lib" "QCloudMusicApi\lib\"

echo Copying .dll to QCloudMusicApi\bin...
if not exist "QCloudMusicApi\bin" mkdir "QCloudMusicApi\bin"
copy /Y "%SRC%\*.dll" "QCloudMusicApi\bin\"

echo Copying .dll to x64\Release...
if not exist "x64\Release" mkdir "x64\Release"
copy /Y "%SRC%\*.dll" "x64\Release\"

echo.
echo ========================================
echo   Done!
echo ========================================
pause
