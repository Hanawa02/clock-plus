# Commands

```bash
alias get_idf='. $HOME/esp/esp-idf/export.sh'
get_idf
idf.py set-target esp32h2
idf.py build
idf.py -p /dev/ttyACM0 flash monitor


# dependencies
idf.py add-dependency esp-idf-lib/bme680
```



# Throubleshooting

## Connection

```bash
--- Error: device reports readiness to read but returned no data (device disconnected or multiple access on port?)
--- Waiting for the device to reconnect
```

1. The "Software Kick" (No touching the hardware)

If the monitor says Waiting for reconnection or Device disconnected:

    Kill the process: Press Ctrl + ] to exit the monitor completely.

    Restart the monitor: Run idf.py monitor again. Sometimes the software just needs to re-open the file handle to /dev/ttyACM0.

2. The "Physical Reboot"

If the screen is frozen or not sending data:

    Press the Reset (EN) button once.

    Watch the terminal. If you see a wall of text starting with ESP-ROM, it worked.

3. The "Power Cycle" (The Gold Standard)

If the port has "disappeared" from Linux (ls /dev/ttyACM* returns nothing):

    Unplug the USB cable from your laptop.

    Wait 3 seconds (this lets the capacitors on the board discharge).

    Plug it back in.

    Check if the port is back: ls /dev/ttyACM*.

4. The "Force Download" (If the code is crashed)

If your code has a "boot loop" (it crashes and restarts infinitely), the computer might struggle to grab the port.

    Hold the BOOT button.

    Press and release the Reset (EN) button.

    Release the BOOT button.

    The chip is now in a "silent" waiting state. You can now run idf.py flash successfully.


# References

https://github.com/UncleRus/esp-idf-lib/blob/master/docs/source/groups/bme680.rst
https://github.com/waveshareteam/e-Paper/tree/master/E-paper_Separate_Program/4in2_e-Paper_G/ESP32
https://www.waveshare.com/wiki/4.2inch_e-Paper_Module_Manual#ESP32.2F8266
https://www.waveshare.com/wiki/Bme680?srsltid=AfmBOoqPPj9ZYPLAmlMP35z1a8SaZOOrNOYNApiYSeoCBmXSgaey9XNJ
https://www.waveshare.com/w/upload/4/4f/ESP32-H2-Zero-details-inter.jpg


https://github.com/waveshareteam/e-Paper/trunk/E-paper_Separate_Program/4in2_e-Paper_G/ESP32