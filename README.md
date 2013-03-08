BinarySearchMouseEmulator
=========================

A Windows mouse emulator that uses the keyboard and binary search for efficiency.

Controls:
Press F12 to toggle keyboard controlled mouse mode. 

NOTE: Windows will not respond to your keyboard presses when this is active

Press Home to center the mouse cursor on your screen

Use arrow keys to contol the mouse. If you get stuck just press Home again.

They way to get where you want is to always press the arrow key in the direction you need the mouse to move to get where you want to go. The mouse may overshoot, this is okay, now you just press the arrow key in the opposite direction until you arrive or you overshoot again. Notice that the distance the cursor moves is halved each move along the same axis. This binary search allows for rapid and precise movement of the mouse using the keyboard.

Press Caps Lock to left-click. 

NOTE: To allow for easily click and drag, left-click stays clicked until you press Caps Lock again.
Unfortunately this means double-click is four fast presses of Caps Lock.

Press Scroll Lock to right-click.

NOTE: see left-click NOTE.

NOTE: On Windows 7 and above this application stops functioning when a system control comes up, 
such as UAC or Task manager. This is a Windows security feature. I believe signing the application 
with a valid certificate gets around this, but I don't care enough to buy a certificate.


Build instructions:

Use Visual Studio if you don't trust the posted .exe. It is a keyboard hook after-all.
