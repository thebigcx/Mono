# Mono
A C++ Mono wrapper to make embedding C# into C++ easier. 

## Compiling
In your shell of choice:
```bash
vendor/premake/premake5 gmake Release
make
```
If you do not have gmake, swap 'gmake' for 'vs2019' to generate a .vsproj.

There should now be a static library in the bin/Debug/Mono directory. Copy this, along with Mono/include into your project.
Link the static library, and you should be good to go.
