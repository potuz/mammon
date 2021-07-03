# Mammon
Will slowly be an ETH consensus client for research purposes.

Written in C++, currently only has basic types for phase0 and their serialization. This is very skeletal and is only here to test a CI environment, things will move drastically from commit to commit. The only functionality currently is in `test_ssz` which tests the serialization and deserialization as well as the yaml deconding/encoding of the basic types. Relies on [Acutest](https://github.com/mity/acutest) for basic testing and on [yaml-cpp](https://github.com/jbeder/yaml-cpp) for yaml  decoding/encoding.

I'm testing on Arch linux and compiling both with gcc and clang, using `C++20`. This repo is under `GPLv3` with the caveat that acutest is released under MIT license.

- No document strings.
- No error checking.
- Not portable. 
