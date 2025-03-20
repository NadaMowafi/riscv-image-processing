# riscv-image-processing

Image processing library for RISC-V processors. This library is designed to utilize the RISC-V vector extension (RVV) to accelerate image processing algorithms.

## Build Instructions for Scalar Version

1. Clone the repository
```
git clone
```

2. Build the library
```
cd riscv-image-processing
mkdir build
cd build
cmake ..
cmake --build .
```

3. Run the tests
```
ctest
```
or
```
ctest -C Debug
```

## Build Instructions for Vector Version

*not yet implemented*
