@echo off
rem Run tests on Windows

setlocal

set "builddir=none"

if exist ..\Windows\x86 set "builddir=..\Windows\x86\Debug\Storm.exe"
if exist ..\Windows\x64 set "builddir=..\Windows\x64\Debug\Storm.exe"

if %builddir% == "none" (
	echo Executable folder not found, please make sure that storm.exe has been built.
	exit /B 0
)

cd ../tests
echo "%1"


setlocal EnableDelayedExpansion
rem total number of tests compiled
set compilednum=0
rem total number of tests executed
set exec=0
rem total number of tests run
set total=0

for %%A in ("*.*") do (
	set /a total=!total! + 1
	rem Gives the name of each file in tests but not in tests\compiled
	set orig=%%~nA.storm
	rem compiled file name
	set compiled=compiled\!orig!c

	cmd /c !builddir! "-c" %%A !compiled!
	if %errorLevel% == 1 (
		rem failure

		echo Test !orig! failed to compile
		if "%1" == "--nofail" (
			exit 1
		)
	)
	if %errorLevel% == 0 set /a compilednum=!compilednum! + 1

	cmd /c !builddir! !compiled!
	if %errorLevel% == 1 (
		rem failure
		echo Test !orig! failed to execute
		if "%1" == "--nofail" (
			exit 1
		)
	)
	if %errorLevel% == 0 (
		echo Test !orig! executed successfully
		set /a exec=!exec! + 1
	)
)

echo !compilednum! tests compiled and !exec! tests run
echo !total! tests in total

rem Return to original dir
endlocal