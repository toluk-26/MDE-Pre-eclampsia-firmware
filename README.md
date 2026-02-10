it takes a while for the environment to set up. you MUST use PlatformIO so that we can use the same libraries and tools easily.

# Getting Started
open the folder in VSCode with the [PlatformIO extension](vscode:extension/platformio.platformio-ide)
## Configurations
there are three configurations in this project. you can configure it in "PlatformIO: Pick Project Environment"
1. `env:release` - release version
2. `env:debug`  - debug version
3. `env:TEST_*` for testing with a different entrypoint from the main program

you should be using `env:debug` for the most part. If you output to Serial, put it in preprocessor conditional blocks like so..
```cpp
#ifdef DEBUG
    Serial.print("howdy y'all");
#endif
```
im not sure if its actually better to do that. ig it will make the program be faster and smaller

# Code
## entrypoint
- the entrypoint is [src/main.cpp](/src/main.cpp) or whatever test.
- we should keep our code cool and abstract. Keep data structures in the [/lib](/lib/) folder. Avoid using `/include` and the implementation in `/src`
## contributions
you **MUST** create a branch to work on something. ive blocked pushes to main, but you can create a pull request or merge. this behavior is untested ngl

you **SHOULD** be putting most of your code in /lib. it helps to make /src clear for entrypoints. 

# Actions
the task can be easily accessed from the status bar at the bottom of the VSCode window. you can hover over it to see what it does.

![image of PIO actions in the VSCode status bar](/attachments/pio-status-bar.png)
- **compile** is the check mark
- **upload** is the right arrow
- **serial monitor** is the plug icon. you can get the [serial monitor extension](vscode:extension/ms-vscode.vscode-serial-monitor) but its sometimes easier to use the PIO one because it will auto close/open when you upload
- where you see `env:debug (Device Pre Eclampsia)`, you can click on it to bring up the menu to change the environment configuration


# Libraries
If you add a library please note to move it to the common version [`env`]. It's typically better to use PlatformIO library to add libraries, so please look there first
## github libraries

to add github libraries, append to `lib_deps =` in [platformio.ini](/platformio.ini). its preferred if you use the latest tagged version, but if you want to use the latest commit version, don't include `#<tag>`
```ini
    - <github link>#<tag>
```

# Testing
sorry yall, testing is a pain here. in [platform.ini](/platformio.ini) i have some TEST environments. you can follow the pattern. you can copy one of the [env:TEST_*]. you need to copy the `extends=env:debug` and follow the pattern with `build_src_filters`. you must modify the last `+<TEST_file.cpp>` with whatever you are working on. you can  make the .cpp file in src/ and it will be the entrypoint for that environment

# git review
to keep it simple, use vscode's source control tab

## create branch
1. go to the source control tab and select the more options

<img src="attachments/source control tab.png" alt="image of source control tab" height="200px">

1. select the more options button

<img src="attachments/scm options.png" alt="image of source control tab" height="150px">

3. select branch > create branch and follow the instructions
4. you probably need to publish the branch, but you can wait until you're ready for the next commit

## merge into main/create pr 
go to branch > merge... and select the current branch you are in and then main. if there are merge conflicts lmk or figure it out i believe in you.

## update from main
go to branch > merge... and select main and then the current branch you are in. its the reverse of the previous