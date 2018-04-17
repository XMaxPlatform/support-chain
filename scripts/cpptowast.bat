
set xmax_install_dir=%1
set cpp_file=%2
set wast_file=%3

set workdir=mktemp


mkdir %workdir%\built

set workfulldir=%cd%\%workdir%

for /f "delims=" %%a in ("%cpp_file%")do (

rem file %%a
rem filePath %%~dpa
rem fileName %%~nxa

%WASM_BIN%\clang.exe -emit-llvm -O3 --std=c++14 --target=wasm32 -ffreestanding -nostdlib -fno-threadsafe-statics -fno-rtti -fno-exceptions -I "%xmax_install_dir%\include" -I %%~dpa -c %%a -o %workfulldir%\built\%%~nxa

)

for /f "delims=" %%i in ('"dir /a/s/b/on %workfulldir%\built\*.*"') do (  
echo %%~fi >> %workfulldir%/listfiles.txt
)
set /p listfiles=<%workfulldir%/listfiles.txt

%WASM_BIN%\llvm-link.exe -o %workfulldir%\linked.bc %listfiles%
%WASM_BIN%\llc.exe --asm-verbose=false -o %workfulldir%\assembly.s %workfulldir%\linked.bc
%BINARYEN_BIN%\s2wasm.exe -o %wast_file% -s 16384 %workfulldir%\assembly.s



rd /s /Q %workfulldir%