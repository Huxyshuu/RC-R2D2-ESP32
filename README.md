#### Update (27.09.2025)
Fixed old image links for GitHub

<img src="https://i.imgur.com/ZxnPFW1.png" align="right"
     alt="R2D2 Poster" width="350">

# Remote-Controlled R2D2
🚀 R2D2 Replica – Bluetooth-Controlled Droid with Web UI
This project brings R2D2 to life with **custom-designed, 3D-printed parts** and **Bluetooth control!** 
The droid is fully operable via a **PS4 controller**, allowing for smooth, responsive movement.

But that’s not all—a web-based interface lets you:

🎵 **Choose iconic R2D2 sound effects**

🔊 **Adjust volume levels** for a personalized experience

📡 **Monitor real-time status updates** as the droid moves and connects

Built from scratch, this project blends **electronics, CAD modeling, and programming** into a fully interactive experience.

# Table of Contents
1. [Video & Images](#video--images)
2. [Repository Organization](#repository-organization)
3. [Components](#components)
4. [Electronics](#electronics)
5. [CAD Models & 3D Prints](#cad-models--3d-prints)
6. [Web Interface](#web-interface)
7. [Code](#code)
8. [Future](#future)
9. [Thank You](#thank-you) ❤️

## Video & Images
Video and images taken from the Mechatronics Circus 2025 in Aalto University

https://github.com/user-attachments/assets/bc08d23b-32bb-44b7-bdc1-37275ab68ecd

<img src="https://i.imgur.com/uAkL5tT.jpeg"
     alt="Image1 from the Mechatronics Circus 2025" height="300">
     <img src="https://i.imgur.com/vqMbkkU.jpeg"
     alt="Image2 from the Mechatronics Circus 2025" height="300">
     

## Repository organization
The file structure will be as follows:
```
📦 RC-R2D2-ESP32
 ├─ 📄 README.md
 ├─ 📄 RoboCode.Ino
 ├─ 📄 webUI.html
 ├─ 📂 Prints (contains all the printable files in .STL format)
 │  ├─ 📄 torso.stl
 │  ├─ 📄 head.stl
 │  └─ ...
...
```

## Components
Most of the components are listed in the table with price estimates (shipping not included), however smaller components such as wires, resistors, bolts as well as 3D printed PLA parts are not listed. (**images are from online**)

<table>
  <tr>
     <th width="300">Adafruit HUZZAH32 – ESP32 Feather Board</th>
     <th width="300">MP2307 - Buck Regulator</th>
     <th width="300">DFR0299 - DFPlayer</th>
     <th width="300">MX1508 - DC Motor Driver</th>
  </tr>
  <tr>
      <td><img src="https://i.imgur.com/Npj6KwO.jpeg" alt="ESP32" width="300"></td>
      <td><img src="https://i.imgur.com/E8Ccj8q.jpeg" alt="MP2307" width="300"></td>
      <td><img src="https://i.imgur.com/YsXlxTt.jpeg" alt="DFR0299" width="300"></td>
      <td><img src="https://i.imgur.com/kmfp4qb.jpeg" alt="MX1508" width="300"></td>
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
      <td><img src="https://i.imgur.com/DNhwS6L.jpeg" alt="ROB-12205" width="300"></td>
      <td><img src="https://i.imgur.com/EdJCTX1.png" alt="LSM-45F-8" width="300"></td>
      <td><img src="https://i.imgur.com/oHG3ylP.png" alt="Switch" width="300"></td>
      <td><img src="https://i.imgur.com/3Xnewmg.jpeg" alt="Battery" width="300"></td>
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
      <td><img src="https://i.imgur.com/EJeyLDY.png" alt="SD-Card" width="300"></td>
      <td><img src="https://i.imgur.com/u2u0g9x.jpeg" alt="PS4-controller" width="300"></td>
      <td><img src="https://i.imgur.com/h35EqO6.png" alt="Blue spray can" width="300"></td>
      <td><img src="https://i.imgur.com/rOUwNbN.png" alt="Magnets" width="300"></td>
  </tr>
  <tr>
      <td><strong>~8€</strong></td>
      <td><strong>~40€</strong></td>
      <td><strong>5€</strong></td>
      <td><strong>4 X 0.5€</strong></td>
  </tr>
</table>

The total cost of the components and accessories from the table comes to **98€ (+ 40€ Wireless Controller)** but to give some room for error, shipping and considering the parts not included such as 3D prints, bolts, wires, etc., rounding it up to **150-170€** range should probably cover everything. I have not tested other wireless bluetooth controllers with the code, so using anything other than PS4 controllers might require tweaking the code or using a completely different library to get it connected with the ESP.

## Electronics
I soldered all of the components to the protoboard as seen in the pictures. Most of the wires and connections are below the board and can't be seen from the camera images, but the wiring diagram should accurately show all details.

<img src="https://i.imgur.com/Qs9Jmv6.png" alt="Wiring" width="300"><img src="https://i.imgur.com/p6Wi7DN.jpeg" alt="Wiring" width="300"><img src="https://i.imgur.com/ofP7zTy.png" alt="Wiring" width="300">

## CAD Models & 3D Prints
Custom 3D models to fit all the necessary electronics and components inside. I included a small hatch in the back to have easy access to electronics in case there's any need to update the code or a problem occurs. The whole build consists of 17 printed parts.

The branding will be removed from the hatch in the .STL files in case someone is interested in printing this ^^

<img src="https://i.imgur.com/CYhFhS5.png"
     alt="R2D2 Poster" height="300"><img src="https://i.imgur.com/OfLIeBo.png"
     alt="R2D2 Poster" height="300">
     
<img src="https://i.imgur.com/xLYjFZ6.png"
     alt="front view" height="250"><img src="https://i.imgur.com/VVzoz1w.png"
     alt="side view" height="250"><img src="https://i.imgur.com/poVChxk.png"
     alt="back view" height="250"><img src="https://i.imgur.com/grrUFfI.png"
     alt="up view" height="250"><img src="https://i.imgur.com/DROu2Uq.png"
     alt="exploded view" height="250">



<img src="https://i.imgur.com/XjvP6Dn.png" align="right"
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

<img src="https://i.imgur.com/zZ1taMK.png" alt="Instructions" width="300">

The full program contains over 1400 lines of code (mostly html). The program uses a few external libraries to achieve all the necessary functions such as connecting the PS4 controller to the ESP and running an asynchronous web server for additional controls. The **.ino file contains the entire program.** I have added a separate .html file, just to make it a bit easier to see and edit.

[Project code in Github](https://github.com/Huxyshuu/RC-R2D2-ESP32/blob/main/RoboCode.ino)

## Things to improvement
While most of the project came out as intended and worked smoothly, there are a few areas that would benefit from further improvement.

- The legs have a very flimsy attachment to the torso and are not rigid. Reworking the attachment would eliminate this problem.
- The head turns well with the current setup but creates a lot of noise from the edge of the head hitting the bearings. Removing the bearings from the torso and lifting the head up from the center beam in a rigid way should improve this.
- The middle leg is also attached to the torso in a rather silly way as well as having the caster wheel which makes the turning and climbing small bumps hard.

### Thank you
As with most "great" things, they are rarely done alone, so thank you to everyone who helped me and kept me company with this project.

Merihelinä Larimo

Paavo Miettinen

Szilard Papp

Johannes Malinen

& everyone involved

^^ ❤️
