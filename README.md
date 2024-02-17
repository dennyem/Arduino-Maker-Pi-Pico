An Arduino project based on the Maker Pi Pico development board.

This project showcases the major features of the board to create a talking clock:

Circular TFT connected to SPI0.
An SD Card Connected to SPI1.
A PWM wave file player that can be connected to a sound system.

The Clock display is a circular GC9A01 240 x 240 pixel TFT connected to SPI0. Pin connections are detailed in the Arduino Sketch, no soldering is required.

A micro SD Card of any size (I have tested the code with various cards from 2GB to 256GB, i.e. type SD1, SD2 and SDHC) is used to store wave files (*.WAV) that constitute the various sounds for speaking the time plus some others you can use just for fun (in the setup() code try changing the line playSound(&sd, "CHEERIO.WAV"); to playSound(&sd, "deleted.wav");

Copy the files in the “data” directory included in the project files to your SD card prior to uploading the project.

My choice of Earle Philhower’s SdFat library was due to the lack of reliable libraries that allowed connection other than SPI0 and support for larger capacity cards, plus the fact that Earle’s library is included in the RP2040 core board support as SdFat.h.

