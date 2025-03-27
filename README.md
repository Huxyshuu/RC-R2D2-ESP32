# Remote-Controlled R2D2
A mechatronics project

![Project poster](https://i.imgur.com/ZxnPFW1.png)

## Project Plan
The goal is to create an R2D2 model from scratch with custom designed parts, 3D printing all the parts to resemble the real thing (down-scaled). The droid will be controlled with a PS4 controller over bluetooth, allowing for seamless movement. There will also be a web UI that allows for picking sound effects, tweaking the volume of all the sounds that R2D2 makes as well as seeing the live status of the droid updating when it moves and connects. 

## Components
Most of the components are listed in the table with price estimates (shipping not included), however smaller components such as wires, resistors, bolts as well as 3D printed PLA parts are not listed.

TABLE HERE

The total cost of the components and accessories from the table comes to 98€ (+ 40€ Wireless Controller) but to give some room for error, shipping and considering the parts not included such as 3D prints, bolts, wires, etc., rounding it up to 150-170€ range should probably cover everything. I have not tested other wireless bluetooth controllers with the code, so using anything other than PS4 controller might require tweaking the code or using a completely different library to get it connected with the ESP.

## Electronics
I soldered all the components to the protoboard as seen in the pictures.
PICS

## CAD Models & 3D Prints
Custom 3D models to fit all the necessary electronics and components inside. I included a small hatch in the back to have easy access to electronics in case there's any need to update the code or a problem occurs. The whole build consists of 12 separate CAD files (not including mirrored parts), and 17 printed parts.

I will post all the necessary CAD files in the github repo and remove the branding on the hatch, in case someone is interested in using them.

## Web Interface
The illustration of the web interface that allows for more accessible control over the available settings as well as clunky movement commands. The ESP controller starts checking for a WiFi connection as soon as it is booted up. Pinging for 15 seconds while also blinking the WiFi LED. If the connection is found, it will send a telegram message containing the IP address to my telegram account. Accessing the IP address in any browser in the same network will show the web interface. If the pinging for a network fails, it will work just fine and continue without it, but there will be no access to the web interface and the additional settings provided there.

The interface includes:

Movement ( turning the head, rotating and moving the body )
Sound control ( switching soundtracks, volume, mute )
And some information about the hardware ( IP, Controller battery, DFPlayer status...)

## Code
### Instructions
In order to be able to boot the program onto the Adafruit ESP32 Feather board, you have to change the partition scheme to a larger one, which excludes the OTA and reduces the allocated memory for SPIFFS. This allows the app to be up to 3 MB, which is sufficient for this project. Picture below should help switching the switching in Arduino IDE.

I haven't displayed the code here, since it contains over 1400 lines of code (mostly html). The program uses a few external libraries to achieve all the necessary functions such as connecting the PS4 controller to the ESP and running an asynchronous web server for additional controls. I'll add the code to this wiki and to github. 

Github: https://github.com/Huxyshuu/RC-R2D2-ESP32

## Future
### Things learnt
I've learned a great deal in building this project and gained a lot of confidence in various things, from connecting and soldering electronics to designing and modelling compatible parts for 3D-printing.

- Connecting a bluetooth controller to ESP32 is rather easy.
- Electronics are fun
- 3D-printing is almost fun

### Things to improve
While most of the project came out as intended and worked smoothly, there are a few areas that would benefit from further improvement.

- The legs have a very flimsy attachment to the torso and are not rigid. Reworking the attachment would eliminate this problem.
- The head turns well with the current setup but creates a lot of noise from the edge of the head hitting the bearings. Removing the bearings from the torso and lifting the head up from the center beam in a rigid way might improve this.
- The middle leg is also attached to the torso in a rather silly way as well as having the caster wheel which makes the turning and climbing small bumps hard.

