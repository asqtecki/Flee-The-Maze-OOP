# Flee The Maze - OOP Project

---

## Description
"Flee The Maze" is a 3D Game written in C++ using Raylib. It implements basic OOP concepts including Abstraction, Encapsulation, 
Inheritance, Polymorphism, Operator Overloading and etc.

---

## Features
* 3D implementation
* Random Maze generation using DFS Backtracking algorithm
* AI Ghosts using AStar tracking algorithm
* Random Entrance/Exit generation
* First Person Perspective
* Level Progression
* Larger maze with more ghosts each level
* Clearance after 3 levels
* Wall and collision detection

---

## Pre-requisites
* **Compiler**: g++ (MinGW-w64), C++17 compatible — comes with w64devkit
* **Build tool**: Make — also included with w64devkit
* **Graphics library**: Raylib 5.5 installed at C:/raylib/raylib
* **Platform**: Windows 10/11
* **Assets**: The `Graphics/` and `Music/` folders.

---

# Compilation and Setup
Type the following commands on your Command Prompt OR Terminal once the Pre-requisites are installed and set up.

* Clone the repository:
```cmd
git clone https://github.com/asqtecki/Flee-The-Maze.git
```
* Navigate to the folder (Say Desktop/Flee-The-Maze-OOP):
```cmd
cd Desktop/Flee-The-Maze
```
* To ensure the 64-bit compiler is used and to avoid architecture conflicts (for Windows):
```cmd
set PATH=C:\raylib\w64devkit\bin;%PATH%
```
* Clean the previous build (optional):
```cmd
make clean
```
* Make the file:
```cmd
make
```
* Execute:
```cmd
game
```
OR
```cmd
game.exe
```
Note: Run `game.exe` from the project root directory to ensure the 
`Graphics/` and `Music/` asset folders are found at their correct relative paths.

---

# How to Play
* There are total of three levels. Each level has a bigger maze with +2 number of ghosts. The number of ghosts in the first level is 1.
* Once executed, you'll spawn at the maze entrance.
* Your goal is to avoid the ghosts and find the exit in order to progress to the next level.
* The player movement is controlled using WASD keys. The SHIFT key can be used for a speed booster.
* Clear all three levels to win the game.

---

# Future Improvements/Changes
* Better lightings and sound effects.
* Differ between entrance and exit using an icon.
* Introduce a goal before finding an exit.
* Introduce number of lives.