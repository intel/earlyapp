# Early App

## Introduction
This software is a testing application that runs on Clear Linux to verify
cold boot to audio, video and RVC (Reverse View Camera) functionalities on Intel platforms.


## License
Early App is distributed under the MIT License.
You may obtain a copy of the License at:
https://opensource.org/licenses/MIT


## Dependencies
- CMake
- Boost libraries
- GStreamer-1.0


## Program options
 - --help: Show usage.
 - -c [--camera-input] <[ici|v4l2]>: Set camera input source, default is ici.
 - -s [--splash-video] <file path>: Set splash video path.
 - -d [--cbc-device] <device path>: Set CBC device path.
 - --bootup-sound <file path>: Set bootup sound path.
 - --rvc-sound <file path>: Set RVC sound path.
 - -w [--width] <nubmer>: Set display width.
 - -h [--height] <number>: Set display height.


## Building

1. Download sources.

2. Build:

  ```shell
  $ cmake .
  $ make
  ```

3. Run:

  ```shell
  $ src/earlyapp [options]
  ```


## Compilation options
 - USE_LOGOUTPUT
 : Enable detailed log output to standard out.
```shell
$ cmake -DUSE_LOGOUTPUT=ON .
```

 - USE_DMESGLOG
 : Enable log output to dmesg.
```shell
$ cmake -DUSE_DMESGLOG=ON .
```

