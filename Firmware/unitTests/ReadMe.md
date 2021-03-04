# Tests ReadMe

To run this tests there are several preconditions:
## Installed programs:
- Visual Studio Code with following extensions:
    - [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
    - [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)
    - [CMake](https://marketplace.visualstudio.com/items?itemName=twxs.cmake)
- Installed Espressif toolchain with IDF (IoT-Development-Framework)
    - See [Getting started](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html) for details
    - Register path to cloned IDF as Environment variable `%IDF_PATH%`
- To compile and run tests you need also to install on you machine
    - [MinGW](https://osdn.net/projects/mingw/downloads/68260/mingw-get-setup.exe/)
    - [CMake](https://cmake.org/download/) 

## Execution
Note: Each folder is it's own test project and is made to test the appropriate component (from components folder)
The Execution of the test project is conducted in following steps:
    - Open VS Code in the root of the ModuleTests-project 
    - Execute `Ctrl+Shift+P` `> CMake: Configure` and select target platform (lets say GNU).
    - Then you can execute: Hit `Ctrl+Shift+P` -> Type `Tasks: RunTask` -> Select `CMake: Build Project` to build the project. 
    - Then you can execute: Hit `Ctrl+Shift+P` -> Type `Tasks: RunTask` -> Select `CMake: Run Test Project` to build the project. 
    - Note that you can also debug if the project is setup correctly
