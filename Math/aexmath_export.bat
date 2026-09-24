::call aexmath_build.bat
::call aexmath_pbe.bat
ECHO "----------EXPORTING AEXMATH... -----------"

SET SRC_INC_DIR=.\src\Maths
SET SRC_LIB_DIR=.\bin
SET DEST_DIR=.\aexmath_export
SET DEST_INC_DIR=%DEST_DIR%\aexmath
SET DEST_LIB_DIR=%DEST_DIR%\lib
SET CONFIG=Release_Lib
SET PLATFORM=x64

if not exist "%DEST_DIR%" (mkdir "%DEST_DIR%")
if not exist "%DEST_INC_DIR%" (mkdir "%DEST_INC_DIR%")
if not exist "%DEST_LIB_DIR%" (mkdir "%DEST_LIB_DIR%")

:: copy include files
copy "%SRC_INC_DIR%\*.h" "%DEST_INC_DIR%\*.h"
copy "%SRC_INC_DIR%\*.inl" "%DEST_INC_DIR%\*.inl"

:copy 
:: copy lib directory
echo f | xcopy /s "%SRC_LIB_DIR%\%PLATFORM%\%CONFIG%\*.dll" "%DEST_LIB_DIR%\%PLATFORM%\*.dll"
echo f | xcopy /s "%SRC_LIB_DIR%\%PLATFORM%\%CONFIG%\*.lib" "%DEST_LIB_DIR%\%PLATFORM%\*.lib"

:: copy lib files
SET TARGET_NAME=aexmath
SET CONFIG=Release
SET PLATFORM=x64

:copy

if not exist "%DEST_LIB_DIR%\%PLATFORM%\" (mkdir "%DEST_LIB_DIR%\%PLATFORM%\")
if not exist "%DEST_LIB_DIR%\%PLATFORM%\%CONFIG%\" (mkdir "%DEST_LIB_DIR%\%PLATFORM%\%CONFIG%\")

if exist "%SRC_LIB_DIR%\%PLATFORM%\%CONFIG%\%TARGET_NAME%.dll" echo f | xcopy /f /y "%SRC_LIB_DIR%\%PLATFORM%\%CONFIG%\%TARGET_NAME%.dll" "%DEST_LIB_DIR%\%PLATFORM%\%CONFIG%\%TARGET_NAME%.dll"
if exist "%SRC_LIB_DIR%\%PLATFORM%\%CONFIG%\%TARGET_NAME%.lib" echo f | xcopy /f /y "%SRC_LIB_DIR%\%PLATFORM%\%CONFIG%\%TARGET_NAME%.lib" "%DEST_LIB_DIR%\%PLATFORM%\%CONFIG%\%TARGET_NAME%.lib"

:: Repeat for release, if already done -> end.
if %CONFIG% == Debug_Lib goto :end
SET CONFIG=Debug_Lib
::SET TARGET_NAME=%TARGET_NAME%
goto copy
)

if %CONFIG% == Debug (
SET CONFIG=Release_Lib
SET TARGET_NAME=aexmath
goto copy
)

if %CONFIG% == Release_Lib (
SET CONFIG=Debug_Lib
SET TARGET_NAME=%TARGET_NAME%
goto copy
)

:next_platform
if %PLATFORM%== x64 goto : end
SET PLATFORM=x64
SET CONFIG=Release
set TARGET_NAME=aexmath
goto copy

:end
