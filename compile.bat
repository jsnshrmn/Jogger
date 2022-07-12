REM Automatically generated from Makefile
mkdir -p obj
..\..\..\bin\lcc   -c -o obj\main.o src\main.c
..\..\..\bin\lcc   -c -o obj\level_map.o res\level_map.c
..\..\..\bin\lcc   -c -o obj\level_tiles.o res\level_tiles.c
..\..\..\bin\lcc   -o obj\Example.gb obj\main.o obj\level_map.o obj\level_tiles.o 
