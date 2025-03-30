<img src="https://i.imgur.com/ZxnPFW1.png" align="right"
     alt="R2D2 Poster" width="327" height="463">

# Remote-Controlled R2D2
🚀 R2D2 Replica – Bluetooth-Controlled Droid with Web UI
This project brings R2D2 to life with custom-designed, 3D-printed parts and Bluetooth control! 🛠️🔵 The droid is fully operable via a PS4 controller, allowing for smooth, responsive movement.

But that’s not all—a web-based interface lets you:
🎵 Choose iconic R2D2 sound effects
🔊 Adjust volume levels for a personalized experience
📡 Monitor real-time status updates as the droid moves and connects

Built from scratch, this project blends electronics, CAD modeling, and programming into a fully interactive experience.

# Table of Contents
1. [Components](#components)
2. [Electronics](#electronics)
3. [CAD Models & 3D Prints](#cad-models--3d-prints)
4. [Web Interface](#web-interface)
5. [Code](#code)
6. [Future](#future)

## Components
Most of the components are listed in the table with price estimates (shipping not included), however smaller components such as wires, resistors, bolts as well as 3D printed PLA parts are not listed.

<table>
  <tr>
     <th width="300">Adafruit HUZZAH32 – ESP32 Feather Board</th>
     <th width="300">MP2307 - Buck Regulator</th>
     <th width="300">DFR0299 - DFPlayer</th>
     <th width="300">MX1508 - DC Motor Driver</th>
  </tr>
  <tr>
      <td><img src="https://wiki.aalto.fi/download/thumbnails/357139402/esp32feather.jpg?version=1&modificationDate=1739307948398&api=v2" alt="ESP32" width="300"></td>
      <td><img src="https://wiki.aalto.fi/download/thumbnails/357139402/DC%20Converter1.8A.jpg?version=1&modificationDate=1739308136385&api=v2" alt="MP2307" width="300"></td>
      <td><img src="https://wiki.aalto.fi/download/thumbnails/357139402/DFR0299.jpg?version=1&modificationDate=1739308307415&api=v2" alt="DFR0299" width="300"></td>
      <td><img src="https://wiki.aalto.fi/download/thumbnails/357139402/mx1508-dc-motor-driver-800x800.jpg?version=1&modificationDate=1739308954731&api=v2" alt="MX1508" width="300"></td>
  </tr>
  <tr>
      <td><strong>30€</strong></td>
      <td><strong>2€</strong></td>
      <td><strong>3€</strong></td>
      <td><strong>2 X 6€</strong></td>
  </tr>
</table>

<table>
  <tr>
     <th width="300">ROB-12205 - GEARMOTOR 175 RPM</th>
     <th width="300">LSM-45F-8 Speaker</th>
     <th width="300">Round Rocker Switch</th>
     <th width="300">9V Battery</th>
  </tr>
  <tr>
      <td><img src="https://wiki.aalto.fi/download/thumbnails/357139402/ROB-12205.jpg?version=1&modificationDate=1739309082745&api=v2" alt="ROB-12205" width="300"></td>
      <td><img src="https://wiki.aalto.fi/download/thumbnails/357139402/speaker.png?version=1&modificationDate=1741212618783&api=v2" alt="LSM-45F-8" width="300"></td>
      <td><img src="https://wiki.aalto.fi/download/thumbnails/357139402/switch.png?version=1&modificationDate=1741212690717&api=v2" alt="Switch" width="300"></td>
      <td><img src="https://wiki.aalto.fi/download/thumbnails/357139402/MFG_9V-MN1604.jpg?version=1&modificationDate=1739310014141&api=v2" alt="Battery" width="300"></td>
  </tr>
  <tr>
      <td><strong>3 X 10€</strong></td>
      <td><strong>3€</strong></td>
      <td><strong>1€</strong></td>
      <td><strong>2 X 3.5€</strong></td>
  </tr>
</table>

<table>
  <tr>
     <th width="300">SD Card (8GB is more than enough)</th>
     <th width="300">PS4 Wireless Controller</th>
     <th width="300">Blue spray paint</th>
     <th width="300">Magnets (D: 10mm, h: 5mm)</th>
  </tr>
  <tr>
      <td><img src="https://wiki.aalto.fi/download/thumbnails/357139402/sdcard.jpeg?version=1&modificationDate=1739828342183&api=v2" alt="ROB-12205" width="300"></td>
      <td><img src="https://wiki.aalto.fi/download/thumbnails/357139402/ps4wireless.jpg?version=1&modificationDate=1739829117388&api=v2" alt="LSM-45F-8" width="300"></td>
      <td><img src="https://wiki.aalto.fi/download/thumbnails/357139402/spraypaint.png?version=1&modificationDate=1742031885414&api=v2" alt="Switch" width="300"></td>
      <td><img src="https://wiki.aalto.fi/download/thumbnails/357139402/magnet.png?version=1&modificationDate=1743078399876&api=v2" alt="Battery" width="300"></td>
  </tr>
  <tr>
      <td><strong>~8€</strong></td>
      <td><strong>~40€</strong></td>
      <td><strong>5€</strong></td>
      <td><strong>4 X 0.5€</strong></td>
  </tr>
</table>

The total cost of the components and accessories from the table comes to **98€ (+ 40€ Wireless Controller)** but to give some room for error, shipping and considering the parts not included such as 3D prints, bolts, wires, etc., rounding it up to **150-170€** range should probably cover everything. I have not tested other wireless bluetooth controllers with the code, so using anything other than PS4 controller might require tweaking the code or using a completely different library to get it connected with the ESP.

## Electronics
I soldered all the components to the protoboard as seen in the pictures.

<img src="https://wiki.aalto.fi/download/attachments/357139402/Electronics.png?version=2&modificationDate=1741602204848&api=v2" alt="Wiring" width="300"><img src="https://wiki.aalto.fi/download/attachments/357139402/Electronics_p2.jpeg?version=1&modificationDate=1741648177560&api=v2" alt="Wiring" width="300"><img src="https://wiki.aalto.fi/download/attachments/357139402/Electronics_p1.png?version=1&modificationDate=1741648177779&api=v2" alt="Wiring" width="300">

## CAD Models & 3D Prints
Custom 3D models to fit all the necessary electronics and components inside. I included a small hatch in the back to have easy access to electronics in case there's any need to update the code or a problem occurs. The whole build consists of 12 separate CAD files (not including mirrored parts), and 17 printed parts.

I will post all the necessary CAD files in the github repo and remove the branding on the hatch, in case someone is interested in using them.

<img src="https://wiki.aalto.fi/download/attachments/357139402/WebIllustration.png?version=1&modificationDate=1741213402976&api=v2" align="right"
     alt="WEB UI" width="420">
## Web Interface
The illustration of the web interface that allows for more accessible control over the available settings as well as clunky movement commands. The ESP controller starts checking for a WiFi connection as soon as it is booted up. Pinging for 15 seconds while also blinking the WiFi LED. If the connection is found, it will send a telegram message containing the IP address to my telegram account. Accessing the IP address in any browser in the same network will show the web interface. If the pinging for a network fails, it will work just fine and continue without it, but there will be no access to the web interface and the additional settings provided there.

**The interface includes:**

**Movement ( turning the head, rotating and moving the body )**
**Sound control ( switching soundtracks, volume, mute )**
**And some information about the hardware ( IP, Controller battery, DFPlayer status...)**

## Code
### Instructions
In order to be able to boot the program onto the Adafruit ESP32 Feather board, **you have to change the partition scheme to a larger one**, which excludes the OTA and reduces the allocated memory for SPIFFS. This allows the app to be up to 3 MB, which is sufficient for this project. Picture below should help switching the switching in Arduino IDE.

I haven't displayed the code here, since it contains over 1400 lines of code (mostly html). The program uses a few external libraries to achieve all the necessary functions such as connecting the PS4 controller to the ESP and running an asynchronous web server for additional controls.

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

