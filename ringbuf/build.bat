:: -D is like the line in source code: #define BUILD_FOR_WINDOWS 1 
:: -Wall turns on all warnings
:: -s strips symbols (reduces size of executable)
:: -o2 Nearly all supported optimizations that do not involve a space-speed tradeoff.
:: -o specifies the output file. 


:: Get folder name. This is onny done for the purposes of giving the executable
:: a nicer name, removing it and having the single gcc line (and removing %sDirName%)
:: is fine. 


@echo off
set "sPath=."
for %%d in ("%sPath%") do set "sDirName=%%~nxd"
echo %sDirName%
@echo on

gcc ./main.c -DBUILD_FOR_WINDOWS -Wall -s -o2 -o main_%sDirName%_test.exe  

