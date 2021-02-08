# SwarmMusic2
A new version of [Swarm Music](https://github.com/ProgDroid/swarm-music)

Previous version in will remain in a separate repo for archival purposes. In there you can find a short presentation with some background and a functionality showcase of this app.

The version found here is intended to be an experiment, a review of the original project after some professional experience and how I could use that to improve this project.
Further improvements are planned, particularly on the interpretative side of the app (the music generation).

This version of the project also served as the basis for some modern OpenGL learning.

## What's new
Changes from the previous version include
- New libraries for the UI ([Nuklear](https://github.com/Immediate-Mode-UI/Nuklear)) and graphics ([GLFW](https://github.com/glfw/glfw), [GLAD](https://github.com/Dav1dde/glad))
- Changes to the code structure (e.g. new class for a Scale)
- Code base improvements from usage of code quality tools

## Dependencies
You will need `autoconf` and `make` to compile [Synthesis Toolkit](https://github.com/thestk/stk).

`cmake` is also needed to compile the app itself.

## How to run
Clone this repo with `--recurse-submodules` to get GLFW and STK

Run the following commands to build the app
```bash
./stk.sh

mkdir build

cd build

cmake ..

make
```

Run the app from the project root with `bin/swarmMusic`

## Screenshots

![New UI](https://user-images.githubusercontent.com/18398887/106136967-f5e3c880-6161-11eb-8c3b-4a1f5026ccee.png)
