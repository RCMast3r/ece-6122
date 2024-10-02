
# compile steps (GRADER PLS NOTE)

```
mkdir build
cd build
cmake ..
make -j
```

# run steps
```
./build/game
```

I intentially didnt include a `code/` folder, however the code is split up between the `include/`s folder and the `src/` as a normal `CMake` project. allows for more easier out-of-source tree builds.

