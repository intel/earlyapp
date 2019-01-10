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
- GStreamer
- ALSA
- Intel Media SDK


## Program options
 - --help: Show usage.
 - -v [ --version ]: Print version number.
 - -c [ --camera-input ] &lt;cam input&gt; Camera input source selection. Only supported with use-gstreamer option.
 - -s [--splash-video] &lt;file path&gt;: Set splash video path.
 - -d [--cbc-device] &lt;device path&gt;: Set CBC device path.
 - --bootup-sound &lt;file path&gt;: Set bootup sound path.
 - --rvc-sound &lt;file path&gt;: Set RVC sound path.
 - -w [--width] &lt;nubmer&gt;: Set display width.
 - -h [--height] &lt;number&gt;: Set display height.
 - --gpio-number &lt;number&gt;: GPIO number for KPI measurements. Negative values will be ignored.
 - --gpio-sustain &lt;number&gt;: GPIO sustaining time in ms for KPI measurements.
 - --use-gstreamer : Use GStreamer for auido, camera and video.
 - --gstcamcmd &lt;custom definition&gt;: Custom GStreamer camera command. Only supported with use-gstreamer option.


## Building

1. Download sources.

2. Build:

  ```shell
  $ mkdir build&&cd build
  $ cmake ..
  $ make
  ```

3. Run:

  ```shell
  $ src/earlyapp [options]
  ```

### Compilation options
 - USE_LOGOUTPUT
 : Enable detailed log output to standard out.
 
  ```shell
  $ cmake -DUSE_LOGOUTPUT=ON ..
  ```

 - USE_DMESGLOG
 : Enable log output to dmesg.
 
  ```shell
  $ cmake -DUSE_DMESGLOG=ON ..
  ```


## Earlyapp in UEFI environment

1. Service enablement for splash video:

  ```shell
  $ sudo systemctl enable earlyapp_gst.target
  ```

2. Using camera on runtime

   Pre-requisite: export XDG_RUNTIME_DIR, WAYLAND_DISPLAY and GST_PLUGIN_PATH

  ```shell
  $ src/earlyapp --use-gstreamer --camera-input test 
  ```

To use earlyapp with CBC please refer to our Early App user guide. 
