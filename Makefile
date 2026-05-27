all: build run

build: arkham_map.c | bin
	gcc "./arkham_map.c" -o "./bin/arkham_map.exe"

run:
	"./bin/arkham_map.exe"

run-win:
	".\bin\arkham_map.exe"

bin:
	mkdir bin