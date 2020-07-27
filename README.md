# atinybasic

`atinybasic` is a straight-forward port of Dennis Allison's famous ["Tiny BASIC"](https://en.wikipedia.org/wiki/Tiny_BASIC) from ["The People's Computer Company"](https://en.wikipedia.org/wiki/People%27s_Computer_Company) [newsletter circa September 1975](https://archive.org/details/1975-09-peoples-computer-company). In the spirit of the original, `atinybasic` does not attempt to be a complete BASIC environment per se. Instead it's meant to be a starting point for building a BASIC suited for a particular project or task. I have added `RND`, `ABS` and an array to the basic feature set, but this is more to show how you might add more features than anything else.

# Video

[![Alt text](https://img.youtube.com/vi/GVN8z__guZE/0.jpg)](https://www.youtube.com/watch?v=GVN8z__guZE)

# Installation

Copy the `basic_blink` folder to where you keep your Arduino sketch folders.

# Setup

By default, the included `basic_blink` sketch will start a headless BASIC interpreter running a (flash stored) program that blinks the LED attached to pin 13 on Arduino Uno boards. To enable a serial console, change the `#define CONSOLE  0` line of `config.ino` to `#define CONSOLE 1`. Also change `#define RAM  0` to `#define RAM  1` and `#define FLASH 1` to `#define FLASH  0` (this changes the program source from being the onboard flash to the onboard RAM). 
