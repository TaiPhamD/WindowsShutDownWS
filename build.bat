@SETLOCAL
@REM delete output folder 
if exist build rd /s /q build
mkdir build
cd build
@REM https://github.com/microsoft/vswhere/wiki/Find-VC
set VSWHERE="%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
for /f "usebackq tokens=*" %%i in (`%VSWHERE% -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
  set InstallDir=%%i
)
CALL "%InstallDir%\Common7\Tools\vsdevcmd.bat" -arch=x64 -host_arch=x64
@IF ERRORLEVEL 1 EXIT /B 1
cmake -G "NMake Makefiles" ..
@IF ERRORLEVEL 1 EXIT /B 1
cmake --build . --config Release