# Mammon
Will slowly be an ETH consensus client for research purposes.

Written in C++, currently only has basic types for phase0, their serialization and Merkleization. This is very skeletal and is only here to test a CI environment, things will move drastically from commit to commit. The only functionality currently is in `test_ssz` which tests the serialization, deserialization and Merkleization, as well as the yaml deconding/encoding of the basic types. Relies on [Acutest](https://github.com/mity/acutest) for basic testing and on [yaml-cpp](https://github.com/jbeder/yaml-cpp) for yaml  decoding/encoding.

## Build

Warning: At times (for example now) this code will only build on specific CPUs.
Currently it should only build on CPUs supporting SHA-ni extensions. I will
update with runtime checks after different extensions assembly has been
implemented. 

```
$ mkdir build
$ cd build
$ cmake .. -DMINIMAL=true
$ make
```

Place the eth2 test vectors from [the eth2 spec tests repo](https://github.com/ethereum/eth2.0-spec-tests) in the root directory of mammon (this repo has one one test-vector for each type). Run the tests with 

```
$ make test
```

## Setup

format is [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) with the only modifications that indentation is 4 characters and column width is 120. Enforced with `clang-format`. Code is checked with `clang-tidy` and `cppcheck`. 

I'm testing on Arch linux and compiling both with gcc and clang, using `C++20`. This repo is under `GPLv3` with the caveat that acutest is released under MIT license.

- No Doxygen strings.
- No error checking.
- Not portable. 
