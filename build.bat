@echo off

set INCLUDE=.\include
set LIB=.\lib
set SRC=.\src

echo Building GLAD...
rem gcc -shared -I%INCLUDE% %SRC%\lib\glad.c -o %LIB%\glad.dll -Wl,--out-implib,%LIB%\libglad.dll.a
gcc -c -I%INCLUDE% %SRC%\lib\glad.c -o %LIB%\glad.o
ar rcs %LIB%\libglad.a %LIB%\glad.o
del %LIB%\glad.o

echo Building main...
rem g++ -static -I%INCLUDE% -L%LIB% %SRC%\main.cpp -o .\main.exe -lglad -lglfw3 -lopengl32 -lgdi32
g++ -c -I%INCLUDE% -L%LIB% %SRC%\main.cpp -o main.o
echo Linking main...
g++ -I%INCLUDE% -L%LIB% ./main.o %LIB%\libglad.a %LIB%\libglfw3.a -lopengl32 -lgdi32 -llibpng16 -o ./main.exe
del main.o

rem copy %LIB%\glfw3.dll .
rem copy %LIB%\glad.dll .
copy %LIB%\libpng16.dll .