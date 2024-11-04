# ConsoleSnake

A snake game for Console (Linux, Windows)

## Gameplay
![image](https://github.com/user-attachments/assets/6459f54e-456c-4d88-8f06-6bb85525f87e)

## Compile & Run
### Linux
- Required packages: `cmake`, `make`, `gcc`, `libncurses-dev`

Compile & Run
1. `cmake -DCMAKE_BUILD_TYPE=Release -S . -B release`
2. `cmake --build release`
3. `release/Snake`

### Windows
Required programs:
1. Install cmake and add it to $PATH
2. Install MinGW and add it to $PATH

Compile & Run
1. `cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -S . -B release`
2. `cmake --build release`
3. Go into the `release` folder and double click `Snake.exe`
