@echo off

::�ж��ڵ�ǰ�����¸ýű��Ƿ��ǵ�һ��ִ��
if "%PlatformInited%"=="" (
    echo Platform is initing!
) else (
    goto :eof
)

::��ʼ����Ϊ����Release�汾
call %~dp0\set_build_type.bat Release

set PlatformIniFile=%1

::�Ӹ�����ini�����ļ��м���ϵͳ����
if "%PlatformIniFile%"=="" (
    echo Default configuration will be used!
) else (
    for /f "delims=" %%a in ('call %~dp0\initool.cmd /s general /i Architecture %PlatformIniFile%') do (
        set Architecture=%%a
    )

    for /f "delims=" %%a in ('call %~dp0\initool.cmd /s general /i Platform %PlatformIniFile%') do (
        set Platform=%%a
    )

    for /f "delims=" %%a in ('call %~dp0\initool.cmd /s general /i ProcessorNum %PlatformIniFile%') do (
        set ProcessorNum=%%a
    )

    for /f "delims=" %%a in ('call %~dp0\initool.cmd /s general /i AdditionCMakeArg %PlatformIniFile%') do (
        set AdditionCMakeArg=%%a
    )

    for /f "delims=" %%a in ('call %~dp0\initool.cmd /s windows /i Toolchain %PlatformIniFile%') do (
        set Toolchain=%%a
    )

    for /f "delims=" %%a in ('call %~dp0\initool.cmd /s windows /i CompilePrefix %PlatformIniFile%') do (
        set CompilePrefix=%%a
    )
)

if "%Architecture%"=="" set Architecture=x86_64
if "%Platform%"=="" (
if "%PROCESSOR_ARCHITECTURE%"=="x86" set Platform=x86
if "%PROCESSOR_ARCHITECTURE%"=="AMD64" set Platform=x64
)
if "%Toolchain%"=="" set Toolchain=v141
if "%ProcessorNum%"=="" set ProcessorNum=%NUMBER_OF_PROCESSORS%
if not "%NUMBER_OF_COMPILE%"=="" (
    set ProcessorNum=%NUMBER_OF_COMPILE%
)
::����ϵͳ���ó�ʼ��VS����
if "%VISUAL_STUDIO_ROOT%"=="" (
  echo "VISUAL_STUDIO_ROOT��������δ���壬��Ĭ��ʹ��Visual Studio 2017��ҵ��"
  set "VISUAL_STUDIO_ROOT=C:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise"
)
echo "��ǰʹ�õ�Visual StudioΪ%VISUAL_STUDIO_ROOT%"

if "%Platform%"=="x64" (
call "%VISUAL_STUDIO_ROOT%\VC\Auxiliary\Build\vcvars64.bat"
) else (
call "%VISUAL_STUDIO_ROOT%\VC\Auxiliary\Build\vcvars32.bat"
)

::��ӹ���Ŀ¼
set PATH=%~dp0tool\;%PATH%

::����VS��˱���
if "%NOT_USE_JOM%"=="y" (
  set MAKE_TOOL=nmake.exe
  echo "��ǰmake����MAKE_TOOL��ָ��Ϊnmake.exe"
) else (
  if exist %~dp0tool\jom.exe (
    set MAKE_TOOL=jom -j%ProcessorNum%
    echo "��ǰmake����MAKE_TOOL��ָ��Ϊjom.exe"
  ) else (
    set MAKE_TOOL=nmake.exe
    echo "��ǰmake����MAKE_TOOL��ָ��Ϊnmake.exe"
  )
)

::��Ǹýű��ѱ�ִ��
set PlatformInited=1

goto :eof
