# Armadillo_Shell
This is our final project for IT&C 344 Operating Systems.

## Installation
* Clone the repository into a folder.
* Install g++ on your preferred terminal. Here is a video to help with this step.<br>
<https://www.youtube.com/watch?v=8CNRX1Bk5sY>
* Inside the folder you cloned into, compile the repository for your computer using this command:
```
g++ -std=c++17 -g armadillo.cpp Handlers/*.cpp inputLoop.cpp Globals/*.cpp -Iinclude -o Ardo.exe
```

## Use
* To see the commands available to run, use:
```
cmd
```
* To get help for how to use the commands, use:
```
help
```
* To get help for a specific command, use the command to see the manual for the command:
```
man <command>
```