# SeeedSpartanEdgeResources

The [Seed Studio Spartan Edge Accelerator](https://wiki.seeedstudio.com/Spartan-Edge-Accelerator-Board/) is a nice little development board that combines an ESP32 microcontroller and a Xilinx Spartan-7 FPGA chip.

There are a few examples for programming the MCU and the FPGA available, but not many. In this repository, I'm documenting a few things that I did with this board.

## [FPGA Bitstream from built-in flash loader](ESP32BitstreamLoader/)

The example provided by Seeed Studio loads the bitstream for the FPGA from a connected Micro-SD card. This is one option, but requires frequent plugging operations with the micro SD card.

The FPGA Bit from flash loader is an alternative. It reads the bitsteam in the second half of the Flash memory chip connected to the MCU. Since it has 100k write cycles endurance (according to the manufacturer), even frequent rewriting of that part of the flash should not be a problem. The micro SD card is then not needed any more.

Uploading the bitstream can be done either using *esptool.py* to write the bitstream file directly to the flash, or via Wifi. A Wifi access point is opened by the loader with a web interface for uploading the bitsteam file, using the ESP32's built in Wifi capabilities.

The web interface also enables changing the WiFi password and has some rudimentary other features for interfacing with the FPGA.

The FPGA Bit from flash loader is written in the Arduino framework.

