## About Modules

The project applies a modular development method where both files, CMake configurations, and branches are seperated into
their respective modules.

For something to be considered as module, following questions should be considered:

- Is exposing a public API?
- Should be a part of the MBASE SDK?
- Is solving a complicated task?

If the answer is yes for all those questions, your idea is most possibly a candidate as a module.

When the idea is considered as a module, the following parameters must be determined:

- **Full Name:** Module's full name.
- **Acronym:** An acronym for the module to be used by the CMake configurations, branch setup etc.
- **Library Name:** Module's library name which will be used by CMake find package names.
- **Description:** A brief description about the module.

After the parameters above are specified, the maintainers will create the corresponding:

- CMake configuration files.
- Include and source directories.
- Git branches.

**Let's use the `inference` module as a case study for the explanations presented in the subsequent chapters.**

### Inference Module Parameters

- **Full Name:** MBASE Inference
- **Acronym:** inf
- **Library Name:** inference
- **Description:** Non-blocking TextToText, Embedder inference module.

### Module CMake Configuration Files

After we know those parameters, we will create two cmake configuration given as

```
+ cmake
    - MBASE<Capitalized_Library_Name>Sys.cmake
+ cmake_config_in
    - mbase-<Library_Name>-config.cmake.in
```

Where the `<Capitalized_Library_Name>` is `Inference` and the `<Library_Name>` is `inference` in our case.
So the, real representation of those cmake config files for the inference module is as follows:

```
+ cmake
    - MBASEInferenceSys.cmake
+ cmake_config_in
    - mbase-inference-config.cmake.in
```

Their content will be documented in the future but for now, you can take look at the `cmake` and `cmake_config_in` directories in the MBASE repo for further examination.

After they are set, user will be able to find this module using cmake by writing the following:

```cmake
find_package(mbase.libs REQUIRED COMPONENTS inference)

target_compile_features(<your_target> PUBLIC cxx_std_17)
target_link_libraries(<your_target> PRIVATE mbase-inference)
target_include_directories(<your_target> PUBLIC mbase-inference)
```

The CMake configuration files will be written by active project maintainers.

### Include and Source directories

Based-off of the parameters, we will create and include and source directories for the proposed module given as:

```
+ include
    + mbase
        + framework
        + json
        + pc
        + <Library_Name>
            - foo.h
            - bar.h
            ...
        + ...
+ mbase
    + json
    + pc
    + <Library_Name>
        - foo.cpp
        - bar.cpp
        - ...
    + ...
```

In our case, those values will be:

```
+ include
    + mbase
        + framework
        + json
        + pc
        + inference
            - ...
        + ...
+ mbase
    + json
    + pc
    + inference
        - ...
    + ...
```

### Git branches

For the proposed module, a git branch will be created with the name `<Acronym>/devel` where the development of that module will only happen in that branch.
For example, the modules such as `pc`, `json`, `inference` have their respective branches given as `pc/devel`, `json/devel`, `inference/devel`.

Those branches will be created by the maintainers and any PR to a module in MBASE library should be made to its respective module branch.

If the module becomes too stale or deprecated or too stable that the further development is not possible, that branch may be deleted.

## One Exception

There is one module in MBASE SDK which may be considered as an exception to those rules which is the MBASE standard library module with acronym `std`.

It is a header-only module and it doesn't have its own `std` subdirectory but instead, it is directly under the `include/mbase` directory.
Its naming of the header files is not constrained by the module name or it doesn't have a strict structure in its part.

MBASE standard library is a library to be used by all MBASE modules. The standard library documentation will be written in the future.

## MBASE Module Table

| Name                   | Acronym     | Library Name | Description                                                             | State            | 
| ---------------------- | ----------- | ------------ | ----------------------------------------------------------------------- | ---------------- |
| MBASE Standard Library | std         | std          | Common standard utility objects and procedures module                   | Active, Stable   |
| MBASE Json Library     | json        | json         | JSON parser module                                                      | Inactive, Stable |
| MBASE Inference Library| inf         | inference    | Non-blocking inference module                                           | Active, Unstable |
| MBASE MCP Library      | mcp         | mcp          | An MCP Client/Server module                                             | Active, Stable   | 
