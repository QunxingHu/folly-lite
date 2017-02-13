# Folly Details
[FBVector](https://github.com/facebook/folly/blob/master/folly/docs/FBVector.md) and
[FBString](https://github.com/facebook/folly/blob/master/folly/docs/FBString.md) only.


# For linux
- In this version, jemalloc is included. if you do not want to use jemalloc, you can remove jemalloc folder and change CMakeLists.txt.
- To use jemalloc, you also can install jemalloc in your computer, and in this case, jemalloc in this project can be removed.
- Boost library is required in linux, you can install boost by `sudo apt-get install libboost-all-dev`/.
- **Attention**: GCC version must be 4.8 or above.

## usage
```
make build
cmake (fooly中CMakeLists.txt 所在目录)
make
./bin/testMain
```


# For windows
 on the way

# For Macos
 on the way