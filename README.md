# Project-KAPE
Knowing Ants Pattern Evolution

## Compilation and execution:
To compile, from the directory "Project-KAPE":
```shell
$ cmake -S ./sources/ -B release -DCMAKE_BUILD_TYPE=Release
$ cmake --build release
```
To run tests:
```shell
$ cmake --build release --target test
```
To execute:
```shell
$ ./release/project-kape
```
