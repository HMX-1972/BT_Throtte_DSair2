# BT_Throtte_DSair2

Bluetooth Keyboard controller using M5 STAMP C3.
![IMG20221217112718](https://github.com/HMX-1972/BT_Throtte_DSair2/assets/110798387/11b07cd1-3403-4906-9e16-517006c0720e)

The standard firmware is a Bluetooth keyboard specification for the DCC model railroad controller DSAir2 (manufactured by Desktop Station), but it can be used with various devices by reorganizing the firmware.

It can also be connected to other devices via Bluetooth and WiFi. Please note that serial communication cannot be used (this is for exclusive use with the keyboard. Firmware can be rewritten via USB).

[Parts list]

M5Stamp C3 (technically suitable)
rotary encoder switch
keyboard (4x4)
OLED panel (0.91 128×32)
Battery case (AA x 4)
Acrylic panel x1 set
8 sets of M3 spacers
M2xL8 flat screw and nuts x 4
cord, header pin

[How to use for the first time]

Turn on the power switch on the back.
Connect your smartphone or tablet to DSair2.
Enable Bluetooth on your smartphone or tablet to add and pair devices. The pendant is displayed as ESP32 keyboard.

If the track power supply of DSair2 is ON, rotate the dial clockwise and confirm that the speed display changes. If it changes, it is connected.

If the track power is off, enter "99#" from the keyboard. If the line power is turned on, the line is connected.

[how to drive]

<speed controle>
By rotating the dial to the right, an instruction to accelerate is given to DSair2. The faster you turn, the more you accelerate. By rotating the dial to the left, an instruction to decelerate is issued. When the dial is pressed, an instruction to stop is issued.

<Truck power supply>
Enter "99#" from the keyboard to turn the track power on and off.

<Direction>
Press the "*" key on the keyboard to reverse the direction of travel. "88#" has the same effect.

<Function>
By entering a number from 0 to 29 from the keyboard and pressing the "#" or A to D keys, DSair2 will be instructed to turn the function ON or OFF. A to D memorize the numbers, so next time you can press the A to D key to instruct the ON / OFF of the function.

<Keyboard input>
If you type a wrong number, enter the appropriate number to make it a three-digit number. Input is reset.

[How to end]
Turn off the power button on the back.

[How to use after the second time]
No pairing operation is required.

[FAQ]

Q1: The Bluetooth keyboard is connected, but does not respond.
A1: Turn off the Bluetooth on your smartphone or tablet and turn it on again.
