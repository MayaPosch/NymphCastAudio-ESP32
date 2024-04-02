# NymphCast Audio - ESP32 #

This repository contains the source code for the firmware of NymphCast Audio, targeting the ESP32 platform as supported by the Espressif ESP-IDF SDK. 

[NymphCast](https://github.com/MayaPosch/NymphCast) is a multi-platform audio-visual streaming system that allows anyone to stream local multimedia content from NymphCast media servers and clients to receivers (NymphCast Server). NymphCast Audio is one such receiver, but audio-only.

The NymphCast Audio - ESP32 (NCA-ESP32) project is an implementation of NymphCast Audio on the Espressif ESP32 platform, offering the following features:

- Auto-discovery using NyanSD.
- Streaming of most audio and video formats (audio track only).
- Playback of online (RTSP) streams and similar.

## Reference Design ##

The reference NCA-ESP32 design consists out of the ESP32-S3-WROOM-1 module with UDA1334A audio codec:

![NymphCast Audio ESP32-S3 reference design](art/nymphcast_audio_esp32-s3_reference_setup.jpg)

<!--The reference NCA-ESP32 design is an easy to assemble hardware design. It includes:

- a 3D printable enclosure.
- ESP32 development board (Espressif DevKitC V4 compatible).
- UDA1334A board (CJMCU-1334).-->

<!--For more details, see its [separate document](reference_design/reference_design.md).-->

## Compatible Hardware ##

Hardware characteristics & requirements:

- 4+ MB Flash
- 4 MB PSRAM
- ESP32-S3 (recommended, modules using octal PSRAM like N16R8)
- ESP32 (not recommended due to slow PSRAM)
- I2S audio codec like the UDA1334A.

**Supported boards:**

| Image | Type | Description |
|--- | --- | --- |
|![ESP32-S3-DevKitC-1](art/esp32-s3-devkitc-1-v1.1-isometric.png)| ESP32-S3-DevKitC-1 | Official [Espressif ESP32-S3](https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/hw-reference/esp32s3/user-guide-devkitc-1.html) board. Budget alternative is the [YD-ESP32-S3](https://github.com/vcc-gnd/YD-ESP32-S3) board in N8R8 or N16R8 configuration. |
|![ESP32-DevkitC v4 WROVER-E](art/esp32-devkitc_v4_wrover-e.jpg)| ESP32-DevKitC V4 - ESP32-WROVER-E | Version of the official [Espressif ESP32-DevKitC V4](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/hw-reference/esp32/get-started-devkitc.html) with ESP32-WROVER-E module (see modules section). |
|![ESP32-WROVER-DEV](art/esp32-wrover-dev.jpg)| ESP32-WROVER-DEV | Originally produced by [Freenove](https://github.com/Freenove/Freenove_ESP32_WROVER_Board). Very similar to the ESP32-DevKitC V4 (above), but with one extra GND and 5V pin, as well as a camera module. Uses same ESP32-WROVER-E module (see modules section). |

**Supported modules:**

| Image | Type | Description |
|--- | --- | --- |
|![ESP32-S3-WROOM-1 & 2](art/ESP32-S3-WROOM-1_S_0.png)| Espressif ESP32-S3-WROOM-1 & 2 | ESP32-S3 modules with at least 8 MB of octal SPI PSRAM and 4 MB of Flash. Includes the WROOM-1/1U/2 variants. |
|![ESP-WROVER-E](art/ESP32-WROVER-E.png)| Espressif ESP32-WROVER-E module | 4 MB Flash, 8 MB PSRAM. Has v3 silicon, with PSRAM cache and other bugs fixed. The ESP32-WROVER-IE variant has an external antenna. Found on the official ESP32-DevkitC V4 boards, the Freenove ESP32-WROVER-DEV and other boards. |
|![](art/ESP32-WROVER-B.jpg)| Espressif ESP32-WROVER-B module | Has 4 MB Flash, 8 MB PSRAM, with v2 silicon. Requires PSRAM cache workaround. The ESP32-WROVER-IB variant has an external antenna. Found on older Espressif development boards and third-party boards. |

Most development and testing has been performed using the ESP32-WROVER-E and ESP32-WROVER-B modules, specifically the Freenove ESP32-WROVER-DEV (WROVER-E) board, which is almost identical to the official [Espressif ESP32-DevKitC V4](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/hw-reference/esp32/get-started-devkitc.html) development board with ESP32-WROVER-E module.

**Supported I2S codecs:**
<table>
<tr>
<th>
Image</th><th>Description</th></tr>
<tr>
<td width="50%"><image src="art/UDA1334A-i2s-digital-audio-breakout_778x507.jpg" height="40%"></td>
<td>NXP UDA1334A-based I2S codec with stereo output. Board is usually found with the model ID <i>CJMCU-1334</i> and is available from a wide variety of sources.</td>
</table>

## Hardware Wiring ##

Based on an I2S codec like the UDA1334A with its three-wire I2S interface, these should be wired up as follows:

|Name	| GPIO|
|-------|-------|
|BCLK	| 14|
|WSEL	| 15|
|DATA	| 13|

Naturally, Vin on the audio board should be wired up to (usually) 3.3V and GND to ground. Check with the documentation for your I2S device if not using the UDA1334A.

## ESP-IDF Project ##

As an ESP-IDF 5.x project, its requirements for building the firmware include only a recent copy of ESP-IDF 5.x from Espressif.

Following this, use the ESP-IDF shell or local equivalent to navigate to the top folder of the NCA-ESP32 project after cloning or downloading it.

In the project root directory the WiFi credentials should be configured in the `wifi_stuff.h` header file (creating this file from the provided template as needed) so that the new NCA-ESP32 device can connect to the local WiFi network.

In the top folder, create a new folder `build` and navigate into it:

```
mkdir build
cd build
```

Next execute `cmake .. -G Ninja`. This will create the build files separate from the project files. If building for ESP32-S3, use the following command:

```
cmake .. -DIDF_TARGET=esp32s3 -G Ninja
```

After running `ninja`, the project should successfully build and can be flashed to a connected (and compatible) ESP32 module with:

```
ninja flash
```

## Project Details ##

As said, the project is a basic ESP-IDF 5.x project with no external dependencies. The included dependencies are found in the `components` folder and are:

| Name | Description
|------|---------
|libavutil 		| Ffmpeg library.
|libswresample 	| Ffmpeg library.
|libswscale	 	| Ffmpeg library.
|libavcodec 	| Ffmpeg library.
|libavdevice 	| Ffmpeg library.
|libavfilter 	| Ffmpeg library.
|libavformat 	| Ffmpeg library.
|NPocoCore 		| [NPoco](https://github.com/MayaPosch/NPoco) portable components core.
|NPocoNet		| [NPoco](https://github.com/MayaPosch/NPoco): portable network components.
|NyanSD			| [NyanSD](https://github.com/MayaPosch/NyanSD): service discovery 
|NymphRPC		| [NymphRPC](https://github.com/MayaPosch/NymphRPC): remote procedure call library.

The **Ffmpeg** libraries were compiled using audio-only presets and optimised for size. In-memory tables and similar constant data were relocated to Flash or PSRAM. All internal Ffmpeg audio-related codecs are supported.

**NPoco** is a fork of the Poco desktop libraries, adapted for MCU usage and with FreeRTOS support added. It acts as the hardware abstraction layer for the main NymphCast code.

**NyanSD** is a lightweight, UDP broadcast-based discovery library. It was adapted for NCA-ESP32 due to the limitations of the ESP-IDF (and LwIP) environment. It enables NCA-ESP32 devices to be discovered on the local network.

**NymphRPC** is the Remote Procedure Call library that underlies all network communication between NymphCast servers and clients. It's a highly optimised, zero-copy RPC implementation that provides low-latency, high-throughput commands and data transfers for use with NymphCast.

## Missing Features ##

With the ESP32 being such a resource restricted platform, the NCA-ESP32 implementation misses one feature from the full NCA implementation: the AngelScript-based NymphCast Apps. There currently is no realistic way that this feature could be added to this MCU platform.

## Known Issues ##

These are all the currently known issues. Found a different issue? Please report it in the [issue tracker](https://github.com/MayaPosch/NymphCastAudio-ESP32/issues).

- Playback on ESP32 (non-S3) often has crackling in the audio output due to limited PSRAM bandwidth.

## License ##

NymphCastAudio-ESP32 is licensed under the terms of the 3-clause BSD license. See [license file](LICENSE) for details.
