

@echo off

if "%~1"=="--clean" (
  if exist build (
    rmdir /S /Q build
    echo Build directory removed.
  ) else (
    echo Nothing to clean.
  )
  goto :eof
)


mkdir build 2>nul
pushd build
cd
cmake -DCMAKE_BUILD_TYPE=Debug -G "Ninja" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON .. 
popd 
cmake --build build


