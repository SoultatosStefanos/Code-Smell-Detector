# An architecture miner of C++ compilation units   


# Set-Up / Build

GraphGenerator uses CMake as its build system on all supported platforms (tested on 
ubuntu debian Pop!_OS version 22.04 LTS).  
This guide will explain to you how to use CMake to build it from source,
under this OS.


## Table of Contents  

[Getting dependencies](#getting-dependencies)  
[CMake optional variables](#cmake-optional-variables)  
[Building](#building)  

## Getting dependencies

* Clang  
Follow the steps described at: https://github.com/llvm/llvm-project.  
Make sure to configure the build process with: `-DLLVM_ENABLE_PROJECTS="clang"`.  
(Tested with the 15.0.2 clang version).  

* Jsoncpp  
`sudo apt-get install libjsoncpp-dev`  
This will install the compatible JsonCpp library on your system. GraphGenerator should 
now be able to link against the libjsoncpp target.  

* wxWidgets  
For building this library, please see platform-specific documentation under `docs/<port>` directory 
at: https://github.com/wxWidgets/wxWidgets.  
(Tested with the 3.1 w.x version).  
GraphGenerator should now be able to link against the wxWidgets_LIBRARIES target.  


## CMake optional variables

Before building the application, some final CMake variables can optionally be
set. Namely, the: CLANG_INCLUDE_DIR, INCREMENTAL_GENERATION, GUI.  

**CLANG_INCLUDE_DIR** (`string`)  
The directory path to the .h files for clang.  
E.g: /usr/lib/llvm-15/lib/clang/15.0.2/.  
If this variable is not configured appropriately, many minining attempts may result to compilation failures.  

**INCREMENTAL_GENERATION** (`bool`)  
Controls wether to mine incrementally after cancel and re-run, (true by default).  
When mining incrementally, GraphGenerator can pickup and continue previous mining sessions on launch.  

**GUI** (`bool`)  
Controls wether to display a GUI progress bar, in order to monitor the mining session and cancel it if needed, 
(true by default).  


## Building

Clone the repο:  
`git clone https://github.com/SoultatosStefanos/Code-Smell-Detector`  
You should create a build directory for GraphGenerator somewhere outside GraphGenerator's sources.  
Then:  
`cd build && cmake ..`  
`make`  
In order to build the application.  








