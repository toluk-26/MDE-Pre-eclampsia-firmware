it takes a while for the environment to set up. you MUST use PlatformIO so that we can use the same libraries and tools easily.

# Getting Started
open the folder in VSCode with the PlatformIO extension
## Configurations
there are three configurations in this project. you can configure it in "PlatformIO: Pick Project Environment"
1. `env:seeed_xiao_nrf52840_sense` - release version
2. `env:debug`  - debug version
3. `test_native` for testing [/lib](/lib/) on your local computer

you should be using `env:debug` for the most part. If you output to Serial, put it in preprocessor conditional blocks like so..
```cpp
#ifdef DEBUG
    Serial.print("howdy y'all");
#endif
```

## Code
### entrypoint
- the entrypoint is [src/main.cpp](/src/main.cpp).
- we should keep our code cool and abstract. Keep data structures in the [/lib](/lib/) folder. Avoid using `/include` and the implementation in `/src`

# Actions
the task can be easily accessed from the status bar at the bottom of the VSCode window. you can hover over it to see what it does.

![image of PIO actions in the VSCode status bar](/attachments/pio-status-bar.png)
- to **test**: press the chemistry lookin ahh glass
    - **NOTE:** if you are testing on test_native, be sure to use the conical flask button. compiling/uploading will give you errors.
- **compile** is the check mark
- **upload** is the right arrow
- where you see `env:debug (Device Pre Eclampsia)`, you can click on it to bring up the menu to change the environment configuration


# Libraries
If you add a library please note to move it to the release version (`env:seeed_xiao_nrf52840_sense`). It's typically better to use PlatformIO library to add libraries, so please look there first
## github libraries

to add github libraries, append to `lib_deps =` in [platformio.ini](/platformio.ini). its preferred if you use the latest tagged version, but if you want to use the latest commit version, don't include `#<tag>`
```ini
    - <github link>#<tag>
```

# Unit Testing
we can make unit testing in PlatformIO. it's kinda dumb but thats alright. You can't tell what failed in a test_type so its stupid. to compare it to catch2, each "test type" is held in a folder and needs it's own `main()`. you can follow the examples i have or look at the [doc](https://docs.platformio.org/en/latest/advanced/unit-testing/frameworks/unity.html). each test type folder must start with `test_` or else it will ignore it. you can see that the native vs device test are put within two different folders. be sure to write your test to the right one. 

within each test type you can have a test block defined by functions
```cpp
void test_block1(){ // test block
    TEST_ASSERT(true);
}

int main(){ // combine all the test blocks for this test type
    UNITY_BEING();
    // all the test functions...
    RUN_TEST(test_block1);
    //...
    return UNITY_END();
}
```