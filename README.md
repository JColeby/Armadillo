# Armadillo
Originally was a group project in college that I decided to rework and add too.

## Installation
* Clone the repository into a folder.
* Install g++ on your preferred terminal. Installation instructions can be found here:<br>
  <https://www.msys2.org>
* Inside the folder you cloned into, run the build.bat file. You can do this by running the following command in the terminal:
```
cmd.exe /C build.bat  
```
NOTE: The build.bat file will build using CMake if installed. 
If you have CMake installed, you may attempt to build Armadillo using a different compiler. <br>
NOTE: Armadillo has not been tested with other compilers! Compatability is not guaranteed
<hr>

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
<hr>

## Configurations
The configurations folder contains all the configuration information for Armadillo.
**PLEASE READ BEFORE EDITING ANYTHING**

### aliases.config
This contains all the command aliases for armadillo.
Aliases can be added directly from the shell using the following command:
```
alias <newAlias> <command>
```
If you want to add an alias manually, add a new line to the file using the following syntax:
```
alias->command
```
Make sure there are no spaces before and after the arrow!!!
<hr>

## Adding your own Custom Commands
If you want to create your own command, feel free to copy the cmd/custom/example template folder
always name the manual file "manual.txt" so the 'man' command will be able to find it.

***NOTICE***
the directory name must match the command name, otherwise Armadillo will not be able to find your command.
Once you have created your command, add the name of the folder/command to the customList.config file on it own line and rerun build.bat to create an executable.
Don't tamper with the standard and builtin list unless you know what you are doing!!!
Download and install custom commands from the internet at your own risk!!!
