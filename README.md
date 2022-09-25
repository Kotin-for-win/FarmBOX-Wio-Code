# FarmBOX-Wio-Code
The code required to run on the SEEED Wio Terminal for a FarmBOX.
This includes .cpp and .h files cloned from this Git Repository (https://github.com/limengdu/Disk91_LoRaE5), thank you very much to limengdu.
Note that these .cpp and .h files are under the BSD License, but it is compadible with the LGPL v2.1 license used by the wio-farmbox.ino file, created by myself.
Please Enjoy!

wio-farmbox.ino -> the actual code that runs on the Wio maintaining a connection with the Raspberry Pi
disk91_LoRaE5.cpp amd dosl91_LoRaE5.h -> code sourced from limengdu to enable communication with the Helium Network

Problems we encountered and how community memebers reading this README can overcome them:

Compilation error: invalid conversion from 'char*' to 'uint8_t* {aka unsigned char*}' -> Use this repository https://github.com/limengdu/Disk91_LoRaE5 NOT this repositroy https://github.com/disk91/Disk91_LoRaE5

expected ';' before -> please ensure all of your lines end with semicolons (;), this is required by the Adurino IDE



