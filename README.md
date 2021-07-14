# Golden Sun: The Lost Age Editor

## To build

### Building GSdatahandler.dll
To build the dll, you need cmake and a C++ compiler. On Windows, this is Visual Studio 2019.

```
cd GSdatahandler
mkdir build
cd build
cmake ..
cmake --build .
```

Then, open up GoldenSunTLA_Editor_bugfix.gm81 in Game Maker 8.1 (which can be found on archive.org). Change the includes to point to the dll in GSdatahandler/build and the ini files in resources.
