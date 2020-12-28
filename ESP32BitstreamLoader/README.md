# FPGA Bitstream from built-in flash loader

This is a small Arduino project for the ESP32 MCU on the https://wiki.seeedstudio.com/Spartan-Edge-Accelerator-Board/ Seed Studio Spartan Edge Accelerator].

It reads the bitsteam in the second half of the Flash memory chip connected to the MCU. Since it has 100k write cycles endurance (according to the manufacturer), even frequent rewriting of that part of the flash should not be a problem. The micro SD card is then not needed any more.

Uploading the bitstream can be done either using *esptool.py* to write the bitstream file directly to the flash, or via Wifi. A Wifi access point is opened by the loader with a web interface for uploading the bitsteam file, using the ESP32's built in Wifi capabilities.

The web interface also enables changing the WiFi password and has some rudimentary other features for interfacing with the FPGA.

The FPGA Bit from flash loader is written in the Arduino framework.

This loader has an undefined license as it bases on the [https://github.com/Pillar1989/spartan-edge-esp32-boot Spartan EDGE ESP32 Boot] library which does not have a defined license either.

## Installation

1. Install the Arduino IDE
2. Install the ESP32 support for the Arduino IDE (described [https://wiki.seeedstudio.com/Spartan-Edge-Accelerator-Board/ here]
3. Select tool->board->DOIT ESP32 DEVKIT
4. Connect board and select the correct USB/Serial device for uploading
5. Compile the attached Arduino sketch and upload it (hit CTRL+U after loading the sketch)


## Connecting to the Wifi.

After starting the MCU, the Wifi access point is started after a few seconds (approx. 3-4 seconds). The name of the Wifi is *FPGA-*, followed by the MAC addess of the MCU. 

The default Wifi Password is *Radius210*. You can change it by calling *http://192.168.4.1/passwd/yourPassword* from your browser after connecting to the board's Wifi. The newly chosen password is printed back to you in the browser. Do not use special characters. If your web browser escapes characters, the escaped version will become your Wifi password!

The currect password is always printed to the serial console (115200 Baud) during the start-up of the MCU.

## Notes

This is quite a hacky project. For the Web-based uploader, the input file is split into 2 kB chunks in Javascript and these chunks are uploaded one-by-one. This is to enable the use of a very simple web server that does not understand the HTTP POST method. 

The project bases on the assumption that the second half of the Flash memory (and the 4 Kilobytes before that) of the MCU is not used. If you extend this project heavily, this assumption is no longer satisfied. The Wifi password is stored at 2044 Kilobytes after the start of the Flash memory. The FPGA bitstream starts at Kilobyte 2048 of the Flash memory. Note that normally, ESP32 programs use a partition table for the Flash memory. This program just ignores it. This is normally no problem, as all partition table elements should be placed at the beginning of the Flash memory, but there is no guarantee for this. 

