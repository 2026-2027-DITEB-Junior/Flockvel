goto :end

ECHO "----------export.bat-----------"

SET TARGET_NAME=aexmath
SET CONFIG=Release
SET PLATFORM=Win32
SET SRC_DIR=.\bin
SET DEST_DIR=.\lib

:copy
if exist "%SRC_DIR%\%PLATFORM%\%CONFIG%\%TARGET_NAME%.dll" echo f | xcopy /f /y "%SRC_DIR%\%PLATFORM%\%CONFIG%\%TARGET_NAME%.dll" "%DEST_DIR%\%PLATFORM%\%CONFIG%\%TARGET_NAME%.dll"
if exist "%SRC_DIR%\%PLATFORM%\%CONFIG%\%TARGET_NAME%.lib" echo f | xcopy /f /y "%SRC_DIR%\%PLATFORM%\%CONFIG%\%TARGET_NAME%.lib" "%DEST_DIR%\%PLATFORM%\%CONFIG%\%TARGET_NAME%.lib"
::if exist "%SRC_DIR%\%CONFIG%\%TARGET_NAME%.pdb" echo f | xcopy /f /y "%SRC_DIR%\%CONFIG%\%TARGET_NAME%.pdb" "%DEST_DIR%\%CONFIG%\%TARGET_NAME%.pdb"

:: Repeat for release, if already done -> end.
if %CONFIG% == Debug_Lib goto :next_platform

if %CONFIG% == Release (
SET CONFIG=Debug
SET TARGET_NAME=%TARGET_NAME%_d
goto copy
)

if %CONFIG% == Debug (
SET CONFIG=Release_Lib
SET TARGET_NAME=aexmath
goto copy
)

if %CONFIG% == Release_Lib (
SET CONFIG=Debug_Lib
SET TARGET_NAME=%TARGET_NAME%_d
goto copy
)

:next_platform
if %PLATFORM%== x64 goto : end
SET PLATFORM=x64
SET CONFIG=Release
set TARGET_NAME=aexmath
goto copy

:end

