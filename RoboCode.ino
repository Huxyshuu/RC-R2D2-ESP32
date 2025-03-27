#include <Bluepad32.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include <HTTPClient.h>

// Telegram Bot Credentials
const char *botToken = "xxx";                                         // Replace with your bot token 
                                                                      // ( should look something like bot1111111111:AAAAA_-AAAAaAAaaAaAAaaaAAAAAAAAAAA ) 
const char *chatID = "xxx";                                           // Replace with your Telegram chat ID

// WiFi Credentials
const char *ssid = "xxx";                                             // Replace with your Wifi name
const char *password = "xxx";                                         // Replace with your Wifi password
AsyncWebServer server(80);

// Motor Control Pins
#define R_MOTOR_PWM1 15  
#define R_MOTOR_PWM2 33
#define L_MOTOR_PWM1 32  
#define L_MOTOR_PWM2 14
#define HEAD_MOTOR_PWM1 26  
#define HEAD_MOTOR_PWM2 25

// Status Indicator Pins
#define WIFI_PIN 27
#define SETTINGS_PIN 13
#define BT_PIN 12

// DFPlayer Serial Connection
HardwareSerial DFSerial(2);
DFRobotDFPlayerMini myDFPlayer;

// Controller Setup
ControllerPtr myControllers[BP32_MAX_GAMEPADS];

// System Variables
uint8_t battery = 0;
uint8_t volume = 25;
bool BTConnection = false;
bool DFConnection = false;
bool inSettings = false;
bool webControl = false;
bool muted = false;

String controllerModel = "Controller not connected";
String macAddress = "Error...";

uint8_t folderNum = 1;
uint8_t fileNum = 1;
int legSpeed = 0;
int headSpeed = 0;

unsigned long lastPlayTime = 0;
unsigned long playInterval = 0;

// Embedded HTML with CSS & JavaScript
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>R2D2 Controller</title>
    <link href="https://fonts.googleapis.com/css2?family=Inconsolata:wdth,wght@50..200,200..900&display=swap" rel="stylesheet">
    <style>
        :root {
            --primary-color: #eb9032;
            --secondary-color: #2D2E32;
            --hover-color: #835638;
            --active-color: #c07e52;
        }
    
        * {
            color: var(--primary-color);
            font-family: "Inconsolata", serif;
            font-optical-sizing: auto;
            font-style: normal;
            font-variation-settings: "wdth" 87.5;
        }

        body {
            text-align: center;
            background-color: var(--secondary-color);
            margin: 0;
        }

        h1 { font-weight: 300; }
        button {
            margin: 5px;
            font-size: 1em;
            border: none;
            outline: none;
        }

        #mainContainer {
            border: 2px solid var(--primary-color);
            margin: 0.5em;
            min-height: 100dvh; 
        }

        #mainDiv {
            display: flex;
            flex-wrap: wrap;
            justify-content: center;
            align-items: center;
            gap: 5em 15em;
            margin-top: 4em;
        }

        #leftDiv { width: 20em; }

        #rightDiv { width: 15em;}

        #r2s {
            height: 20em;
            position: relative;
        }

        #r2s .imgleg, #r2s .imghead {
            opacity: 0;
        }

        #r2s img {
            position: absolute;
            top: 0;
            left: 3em;
            height: 100%;
        }

        #controller { font-size: 1.2em; }
        #info {
            display: flex;
            flex-direction: column;
            align-items: flex-start;
            text-align: start;
            flex-wrap: wrap;
            gap: 0.1em;
        }

        .small-grid {
            display: flex;
            justify-content: space-between;
        }

        .grid-container {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            grid-template-rows: repeat(2, auto);
            gap: 0.5em;
        }

        .grid-item {
            padding: 1.2em;
            border-radius: 0.15em;
        }

        .gib {
            background-color: var(--primary-color);
            cursor: pointer;
        }
        .gib:hover { background-color: var(--hover-color); }
        .gib:active { background-color: var(--active-color); }
        .gib:focus { background-color: var(--active-color); }
        .flip { transform: scaleX(-1); }

        #sound {
            display: flex;
            flex-direction: column;
            justify-content: center;
            margin-top: 3em;
        }

        #sound p { font-size: 1.2em; }
        #sound button {
            background-color: var(--primary-color);
            cursor: pointer;
            padding: 1em;
            border-radius: 0.15em;
            color: var(--secondary-color);
            font-weight: 600;
        }
        #sound button:hover { background-color: var(--hover-color); }
        #sound button:active { background-color: var(--active-color); }
        #changeSound { margin-top: 2em;}

        /* Style the slider */
        #volumeSlider {
            -webkit-appearance: none; /* Remove default styling */
            appearance: none;
            height: 6px;
            background: var(--primary-color); /* Orange track */
            border-radius: 5px;
            outline: none;
            transition: background 0.3s;
        }

        /* Handle (thumb) */
        #volumeSlider::-webkit-slider-thumb {
            -webkit-appearance: none;
            appearance: none;
            width: 18px;
            height: 18px;
            background: #ff8c00; /* Darker orange thumb */
            border-radius: 50%;
            cursor: pointer;
            transition: background 0.3s;
        }

        #volumeSlider::-moz-range-thumb {
            width: 18px;
            height: 18px;
            background: #ff8c00;
            border-radius: 50%;
            cursor: pointer;
            transition: background 0.3s;
        }

        /* Hover effect */
        #volumeSlider:hover {
            background: #ff9500; /* Slightly darker orange */
        }

        #volumeSlider::-webkit-slider-thumb:hover {
            background: #ff7400; /* Even darker orange */
        }

        .folders { display: flex; justify-content: center; gap: 3em; margin-top: 0em;}
        .folder1, .folder2 { position: relative; cursor: pointer; width: 4em; height: 3.5em;}
        .folds { display: none; position: absolute; width: 100%; left: 0; top: 0;}
        .active { display: block; }

        #fileContainer1 { display: none; align-items: center; justify-content: center; flex-direction: column; margin-bottom: 10em;}
        #fileContainer2 { display: none; align-items: center; justify-content: center; flex-direction: column; margin-bottom: 10em;}
        #fileContainer3 { display: none; align-items: center; justify-content: center; flex-direction: column; margin-bottom: 10em;}
        .activeFlex { display: flex !important;}
        .files {
            display: flex;
            align-items: flex-end;
            height: 10em;
            width: 50%;
            padding: 2em;
            overflow-x: scroll;
            scrollbar-width: thin; /* Firefox */
            scrollbar-color: var(--primary-color) transparent; /* Firefox */
            z-index: 1;
        }

        /* For Webkit browsers (Chrome, Edge, Safari) */
        .files::-webkit-scrollbar {
            height: 6px;
        }

        .files::-webkit-scrollbar-track {
            background: var(--primary-color);
        }

        .files::-webkit-scrollbar-thumb {
            background: var(--primary-color);
            border-radius: 10px;
        }

        .files::-webkit-scrollbar-thumb:hover {
            background: var(--hover-color);
        }
        .file { display: flex; cursor: pointer;}
        .fileName { transform: translate(-3em, -3.5em) rotate(-45deg); z-index: 2; text-wrap: nowrap; width: 2em;}
        .circle { border-radius: 50%; border: 2px solid var(--primary-color); width: 3em; height: 3em;}
        .circle:hover { background-color: var(--primary-color);} .circle:hover p { color: var(--secondary-color);}
    </style>
    <script>
        var activeFolder = 0;
        var currentSound = { folder: 1, file: 1, name: "Scream" }
        let HeadLeftID, HeadRightID, UpID, DownID, LeftID, RightID;
        var muted = false;

        function updateBattery() {
            fetch('/battery')
                .then(response => {
                    if (!response.ok) {
                        throw new Error('Failed to fetch battery state');
                    }
                    return response.text();
                })
                .then(data => {
                    document.getElementById('battery').innerText = data;
                })
                .catch(error => {
                    console.error('Error fetching battery state:', error);
                    document.getElementById('battery').innerText = "Error...";
                });
        }

        function updateIP() {
            fetch('/ip')
                .then(response => {
                    if (!response.ok) {
                        throw new Error('Failed to fetch IP address');
                    }
                    return response.text();
                })
                .then(data => document.getElementById('ipAddress').innerText = data)
                .catch(error => {
                    console.error('Error fetching IP:', error);
                    document.getElementById('ipAddress').innerText = "Error...";
                });
        }

        function checkBTConnection() {
            fetch('/btcon')
                .then(response => {
                    if (!response.ok) {
                        throw new Error('Failed to fetch Bluetooth status');
                    }
                    return response.text();
                })
                .then(data => document.getElementById('BTConnection').innerText = data)
                .catch(error => {
                    console.error('Error fetching BT Status:', error);
                    document.getElementById('BTConnection').innerText = "Error...";
                });
        }

        function updateDFConnection() {
            fetch('/dfcon')
                .then(response => {
                    if (!response.ok) {
                        throw new Error('Failed to fetch DFPlayer status');
                    }
                    return response.text();
                })
                .then(data => document.getElementById('DFConnection').innerText = data)
                .catch(error => {
                    console.error('Error fetching DF Status:', error);
                    document.getElementById('DFConnection').innerText = "Error...";
                });
        }

        function updateModel() {
            fetch('/model')
                .then(response => {
                    if (!response.ok) {
                        throw new Error('Failed to fetch controller model');
                    }
                    return response.text();
                })
                .then(data => document.getElementById('model').innerText = data)
                .catch(error => {
                    console.error('Error fetching controller model:', error);
                    document.getElementById('model').innerText = "Error...";
                });
        }

        function updateMac() {
            fetch('/mac')
                .then(response => {
                    if (!response.ok) {
                        throw new Error('Failed to fetch MAC Address');
                    }
                    return response.text();
                })
                .then(data => document.getElementById('mac').innerText = data)
                .catch(error => {
                    console.error('Error fetching MAC Address:', error);
                    document.getElementById('mac').innerText = "Error...";
                });
        }

        function sendCommand(endpoint, params = "") {
            fetch(`/${endpoint}?${params}`)
                .then(response =>  {
                    if (!response.ok) {
                        console.error('Failed to send command:');
                    }
                })
                .catch(error => console.error('Error:', error));
        }

        function onTurnHead(dir) {
            sendCommand("turnHead", `dir=${dir}`);
        }

        function offTurnHead() {
            sendCommand("turnHead", "dir=stop");
        }

        function onMove(dir) {
            sendCommand("move", `dir=${dir}`);
        }

        function offMove() {
            sendCommand("move", "dir=stop");
        }

        function onTurn(dir) {
            sendCommand("turn", `dir=${dir}`);
        }

        function offTurn() {
            sendCommand("turn", "dir=stop");
        }



        function updateStatus() {
            checkBTConnection();
            updateModel();
            updateBattery();
            updateDFConnection();
            updateMac();
        }

        function checkLegs() {
            fetch('/legs')
                .then(response => {
                    if (!response.ok) {
                        throw new Error('Failed to fetch leg movement status');
                    }
                    return response.text();
                })
                .then(data =>{
                    let opacityValue = Math.min(Math.max(data / 255, 0), 1);
                    Array.from(document.getElementsByClassName('imgleg')).forEach(e => e.style.opacity = opacityValue);
                })
                .catch(error => {
                    console.error('Error fetching leg movement status:', error);
                    document.getElementById('model').innerText = "Error...";
                });
        };
        function checkHead() {
            fetch('/head')
                .then(response => {
                    if (!response.ok) {
                        throw new Error('Failed to fetch head movement status');
                    }
                    return response.text();
                })
                .then(data =>{
                    let opacityValue = Math.min(Math.max(data / 255, 0), 1);
                    Array.from(document.getElementsByClassName('imghead')).forEach(e => e.style.opacity = opacityValue);
                })
                .catch(error => {
                    console.error('Error fetching head movement status:', error);
                    document.getElementById('model').innerText = "Error...";
                });
        };

        function updateMovement() {
            checkLegs();
            checkHead();
        }

        function changeFolder(folderNum) {
            const folders = [1, 2, 3];
            
            folders.forEach(num => {
                const isActive = (num === folderNum);
                document.getElementById(`fold${num}fill`).classList.toggle("active", isActive);
                document.getElementById(`fold${num}`).classList.toggle("active", !isActive);
                document.getElementById(`fileContainer${num}`).classList.toggle("activeFlex", isActive);
            });

            activeFolder = folders.includes(folderNum) ? folderNum : 0;
        }

        function changeSound({ folder, file, name }) {
            currentSound = { folder: folder, file: file, name: name };
            document.getElementById('currentSound').innerText = `/0${folder}/${file.toString().padStart(3, '0')}.mp3 - ${name}`;

            fetch(`/changeSound?folder=${folder}&file=${file}`)
                .then(response => {
                    if (!response.ok) {
                        console.error('Failed to change sound:', response.status);
                    }
                })
                .catch(error => {
                    console.error('Error changing sound:', error);
                });
        }

        function playSound() {
            document.getElementById('playButton').innerText = "Playing...";

            fetch(`/playSound`)
                .then(response => {
                    if (!response.ok) {
                        console.error('Failed to play sound:', response.status);
                    }
                })
                .catch(error => {
                    console.error('Error playing sound:', error);
                });

            setTimeout(() => {
                document.getElementById('playButton').innerText = "Play sound";
            }, 2000);
        }

        function stopSound() {
            muted = !muted;
            if (muted) {
                fetch(`/stopSound`).catch(error => console.error('Error stopping sound:', error));
            }
        }

        function muteSound() {
            muted = !muted;
            if (muted) {
                fetch(`/muteSound`).catch(error => console.error('Error:', error));
                document.getElementById('muteButton').innerText = "Muted"
            } else {
                fetch(`/unmuteSound`).catch(error => console.error('Error:', error));
                document.getElementById('muteButton').innerText = "Mute"
            }
        }

        function changeVolume(volume) {
            fetch(`/setVolume?level=${volume}`).catch(error => console.error('Error:', error));
        }

        function changeVolumeHTML(volume) {
            document.getElementById('volumeValue').innerText = volume;
        }

        updateIP();

        setInterval(updateStatus, 1000);
        setInterval(updateMovement, 200);
    </script>
</head>
<body>
    <div id="mainContainer">
        <h1>R2D2 Controller</h1>

        <div id="mainDiv">
            <div id="leftDiv">
                <div id="r2s">
                    <img id="r2img"src="https://i.imgur.com/zJa6jQe.png" alt="R2D2">
                    <img class="imgleg" src="https://i.imgur.com/iss6W4f.png" alt="R2D2-left">
                    <img class="imgleg" src="https://i.imgur.com/fEttW5p.png" alt="R2D2-right">
                    <img class="imghead" src="https://i.imgur.com/6LFWx3D.png" alt="R2D2-head">
                </div>
                <p id="controller">Bluetooth Controller:<br><strong id="BTConnection">Loading...</strong></p>
                <div id="info">
                    <p>Controller Battery: <strong><span id="battery">Controller not connected</span></strong></p>
                    <p>Controller Model: <span id="model">Controller not connected</span></p>
                    <p>ESP Mac Address: <span id="mac">Loading...</span></p>
                    <p>IP Address: <span id="ipAddress">Loading...</span></p>
                    <p>DF Player: <span id="DFConnection">Loading...</span></p>
                </div>
            </div>

            <div id="rightDiv">
                <div>
                    <div>
                        <p>Head</p>
                        <div class="small-grid">
                            <div onmousedown="onTurnHead('left')" onmouseup="offTurnHead()" class="grid-item gib"><svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24"><g fill="none" stroke="#2D2E32" stroke-linecap="round" stroke-linejoin="round" stroke-width="3"><path d="m8 5l-5 5l5 5"/><path d="M3 10h8c5.523 0 10 4.477 10 10v1"/></g></svg></div>
                            <div onmousedown="onTurnHead('right')" onmouseup="offTurnHead()" class="grid-item gib flip"><svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24"><g fill="none" stroke="#2D2E32" stroke-linecap="round" stroke-linejoin="round" stroke-width="3"><path d="m8 5l-5 5l5 5"/><path d="M3 10h8c5.523 0 10 4.477 10 10v1"/></g></svg></div>
                        </div>
                    </div>
                    <div>
                        <p>Body</p>
                        <div class="grid-container">
                            <div class="grid-item"></div>
                            <div onmousedown="onMove('forward')" onmouseup="offMove()" class="grid-item gib"><svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24"><path fill="none" stroke="#2D2E32" stroke-linecap="round" stroke-linejoin="round" stroke-width="3" d="M12 20V4m-7 7l7-7l7 7"/></svg></div>
                            <div class="grid-item"></div>
                            <div onmousedown="onTurn('left')" onmouseup="offTurn()" class="grid-item gib"><svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24"><path fill="none" stroke="#2D2E32" stroke-linecap="round" stroke-linejoin="round" stroke-width="3" d="m11 5l-7 7l7 7m-7-7h16"/></svg></div>
                            <div onmousedown="onMove('backward')" onmouseup="offMove()" class="grid-item gib"><svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24"><path fill="none" stroke="#2D2E32" stroke-linecap="round" stroke-linejoin="round" stroke-width="3" d="M12 20V4m-7 9l7 7l7-7"/></svg></div>
                            <div onmousedown="onTurn('right')" onmouseup="offTurn()" class="grid-item gib"><svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24"><path fill="none" stroke="#2D2E32" stroke-linecap="round" stroke-linejoin="round" stroke-width="3" d="M4 12h16m-7-7l7 7l-7 7"/></svg></div>
                        </div>
                    </div>
                </div>

                <div id="sound">
                    <p>Current sound:<br><strong id="currentSound">/01/001.mp3 - Scream</strong></p>
                    <button id="playButton" onclick="playSound()">Play sound</button>
                    <button id="stopButton" onclick="stopSound()">Stop sound</button>
                    <button id="muteButton" onclick="muteSound()">Mute</button>

                    <label for="volumeSlider">Volume:</label>
                    <input type="range" id="volumeSlider" min="0" max="30" value="25" onmouseup="changeVolume(this.value)" oninput="changeVolumeHTML(this.value)">
                    <span id="volumeValue">25</span>

                    <p id="changeSound">Change sound</p>
                    <div class="folders">
                        <div class="folder1" onclick="changeFolder(1)">
                            <svg class="folds active" id="fold1" width="50" height="52" viewBox="0 0 50 52" fill="none" xmlns="http://www.w3.org/2000/svg"><path d="M3.125 16.25C3.125 12.7497 3.125 10.9995 3.80621 9.66256C4.40542 8.48655 5.36155 7.53042 6.53756 6.93121C7.87451 6.25 9.62467 6.25 13.125 6.25H16.3367C18.4937 6.25 19.5722 6.25 20.5326 6.56946C21.3823 6.85206 22.1612 7.31391 22.8168 7.92375C23.558 8.61312 24.0754 9.55938 25.1104 11.4519L26.3672 13.75H36.875C40.3753 13.75 42.1255 13.75 43.4624 14.4312C44.6385 15.0304 45.5946 15.9865 46.1938 17.1626C46.875 18.4995 46.875 20.2497 46.875 23.75V33.75C46.875 37.2503 46.875 39.0005 46.1938 40.3374C45.5946 41.5135 44.6385 42.4696 43.4624 43.0688C42.1255 43.75 40.3753 43.75 36.875 43.75H13.125C9.62467 43.75 7.87451 43.75 6.53756 43.0688C5.36155 42.4696 4.40542 41.5135 3.80621 40.3374C3.125 39.0005 3.125 37.2503 3.125 33.75V16.25Z" stroke="#EB9032" stroke-width="2"/><path d="M20.7166 22.008L24.8686 19.68H27.2686V36H24.4366V22.848L21.9886 24.192L20.7166 22.008Z" fill="#EB9032"/></svg>
                            <svg class="folds" id="fold1fill" width="50" height="52" viewBox="0 0 50 52" fill="none" xmlns="http://www.w3.org/2000/svg"><path d="M3.125 16.25C3.125 12.7497 3.125 10.9995 3.80621 9.66256C4.40542 8.48655 5.36155 7.53042 6.53756 6.93121C7.87451 6.25 9.62467 6.25 13.125 6.25H16.3367C18.4937 6.25 19.5722 6.25 20.5326 6.56946C21.3823 6.85206 22.1612 7.31391 22.8168 7.92375C23.558 8.61312 24.0754 9.55938 25.1104 11.4519L26.3672 13.75H36.875C40.3753 13.75 42.1255 13.75 43.4624 14.4312C44.6385 15.0304 45.5946 15.9865 46.1938 17.1626C46.875 18.4995 46.875 20.2497 46.875 23.75V33.75C46.875 37.2503 46.875 39.0005 46.1938 40.3374C45.5946 41.5135 44.6385 42.4696 43.4624 43.0688C42.1255 43.75 40.3753 43.75 36.875 43.75H13.125C9.62467 43.75 7.87451 43.75 6.53756 43.0688C5.36155 42.4696 4.40542 41.5135 3.80621 40.3374C3.125 39.0005 3.125 37.2503 3.125 33.75V16.25Z" fill="#EB9032" stroke="#EB9032" stroke-width="2"/><path d="M20.7166 22.008L24.8686 19.68H27.2686V36H24.4366V22.848L21.9886 24.192L20.7166 22.008Z" fill="#2D2E32"/></svg>
                        </div>
                        <div class="folder2" onclick="changeFolder(2)">
                            <svg class="folds active" id="fold2" width="50" height="52" viewBox="0 0 50 52" fill="none" xmlns="http://www.w3.org/2000/svg"><path d="M3.125 16.25C3.125 12.7497 3.125 10.9995 3.80621 9.66256C4.40542 8.48655 5.36155 7.53042 6.53756 6.93121C7.87451 6.25 9.62467 6.25 13.125 6.25H16.3367C18.4937 6.25 19.5722 6.25 20.5326 6.56946C21.3823 6.85206 22.1612 7.31391 22.8168 7.92375C23.558 8.61312 24.0754 9.55938 25.1104 11.4519L26.3672 13.75H36.875C40.3753 13.75 42.1255 13.75 43.4624 14.4312C44.6385 15.0304 45.5946 15.9865 46.1938 17.1626C46.875 18.4995 46.875 20.2497 46.875 23.75V33.75C46.875 37.2503 46.875 39.0005 46.1938 40.3374C45.5946 41.5135 44.6385 42.4696 43.4624 43.0688C42.1255 43.75 40.3753 43.75 36.875 43.75H13.125C9.62467 43.75 7.87451 43.75 6.53756 43.0688C5.36155 42.4696 4.40542 41.5135 3.80621 40.3374C3.125 39.0005 3.125 37.2503 3.125 33.75V16.25Z" stroke="#EB9032" stroke-width="2"/><path d="M29.7894 36H19.9014V33.72C19.9014 32.568 20.0534 31.608 20.3574 30.84C20.6614 30.056 21.0694 29.416 21.5814 28.92C22.0934 28.408 22.6534 28 23.2614 27.696L24.8934 26.88C25.5174 26.56 26.0214 26.224 26.4054 25.872C26.8054 25.504 27.0054 25.032 27.0054 24.456V24.024C27.0054 23.432 26.7974 22.968 26.3814 22.632C25.9814 22.28 25.3334 22.104 24.4374 22.104C23.8934 22.104 23.3414 22.184 22.7814 22.344C22.2374 22.504 21.5734 22.728 20.7894 23.016L19.9014 20.544C20.7974 20.144 21.6134 19.864 22.3494 19.704C23.1014 19.528 23.8294 19.44 24.5334 19.44C25.8294 19.44 26.8614 19.648 27.6294 20.064C28.4134 20.48 28.9734 21.04 29.3094 21.744C29.6614 22.432 29.8374 23.192 29.8374 24.024V24.456C29.8374 25.32 29.6614 26.056 29.3094 26.664C28.9734 27.272 28.5254 27.784 27.9654 28.2C27.4054 28.616 26.7974 28.984 26.1414 29.304L24.5094 30.12C23.9174 30.408 23.4774 30.808 23.1894 31.32C22.9174 31.832 22.7654 32.488 22.7334 33.288H29.7894V36Z" fill="#EB9032"/></svg>
                            <svg class="folds" id="fold2fill" width="50" height="52" viewBox="0 0 50 52" fill="none" xmlns="http://www.w3.org/2000/svg"><path d="M3.125 16.25C3.125 12.7497 3.125 10.9995 3.80621 9.66256C4.40542 8.48655 5.36155 7.53042 6.53756 6.93121C7.87451 6.25 9.62467 6.25 13.125 6.25H16.3367C18.4937 6.25 19.5722 6.25 20.5326 6.56946C21.3823 6.85206 22.1612 7.31391 22.8168 7.92375C23.558 8.61312 24.0754 9.55938 25.1104 11.4519L26.3672 13.75H36.875C40.3753 13.75 42.1255 13.75 43.4624 14.4312C44.6385 15.0304 45.5946 15.9865 46.1938 17.1626C46.875 18.4995 46.875 20.2497 46.875 23.75V33.75C46.875 37.2503 46.875 39.0005 46.1938 40.3374C45.5946 41.5135 44.6385 42.4696 43.4624 43.0688C42.1255 43.75 40.3753 43.75 36.875 43.75H13.125C9.62467 43.75 7.87451 43.75 6.53756 43.0688C5.36155 42.4696 4.40542 41.5135 3.80621 40.3374C3.125 39.0005 3.125 37.2503 3.125 33.75V16.25Z" fill="#EB9032" stroke="#EB9032" stroke-width="2"/><path d="M29.7894 36H19.9014V33.72C19.9014 32.568 20.0534 31.608 20.3574 30.84C20.6614 30.056 21.0694 29.416 21.5814 28.92C22.0934 28.408 22.6534 28 23.2614 27.696L24.8934 26.88C25.5174 26.56 26.0214 26.224 26.4054 25.872C26.8054 25.504 27.0054 25.032 27.0054 24.456V24.024C27.0054 23.432 26.7974 22.968 26.3814 22.632C25.9814 22.28 25.3334 22.104 24.4374 22.104C23.8934 22.104 23.3414 22.184 22.7814 22.344C22.2374 22.504 21.5734 22.728 20.7894 23.016L19.9014 20.544C20.7974 20.144 21.6134 19.864 22.3494 19.704C23.1014 19.528 23.8294 19.44 24.5334 19.44C25.8294 19.44 26.8614 19.648 27.6294 20.064C28.4134 20.48 28.9734 21.04 29.3094 21.744C29.6614 22.432 29.8374 23.192 29.8374 24.024V24.456C29.8374 25.32 29.6614 26.056 29.3094 26.664C28.9734 27.272 28.5254 27.784 27.9654 28.2C27.4054 28.616 26.7974 28.984 26.1414 29.304L24.5094 30.12C23.9174 30.408 23.4774 30.808 23.1894 31.32C22.9174 31.832 22.7654 32.488 22.7334 33.288H29.7894V36Z" fill="#2D2E32"/></svg>
                        </div>
                        <div class="folder2" onclick="changeFolder(3)">
                            <svg class="folds active" id="fold3" width="50" height="52" viewBox="0 0 50 52" fill="none" xmlns="http://www.w3.org/2000/svg"><path d="M3.125 16.25C3.125 12.7497 3.125 10.9995 3.80621 9.66256C4.40542 8.48655 5.36155 7.53042 6.53756 6.93121C7.87451 6.25 9.62467 6.25 13.125 6.25H16.3367C18.4937 6.25 19.5722 6.25 20.5326 6.56946C21.3823 6.85206 22.1612 7.31391 22.8168 7.92375C23.558 8.61312 24.0754 9.55938 25.1104 11.4519L26.3672 13.75H36.875C40.3753 13.75 42.1255 13.75 43.4624 14.4312C44.6385 15.0304 45.5946 15.9865 46.1938 17.1626C46.875 18.4995 46.875 20.2497 46.875 23.75V33.75C46.875 37.2503 46.875 39.0005 46.1938 40.3374C45.5946 41.5135 44.6385 42.4696 43.4624 43.0688C42.1255 43.75 40.3753 43.75 36.875 43.75H13.125C9.62467 43.75 7.87451 43.75 6.53756 43.0688C5.36155 42.4696 4.40542 41.5135 3.80621 40.3374C3.125 39.0005 3.125 37.2503 3.125 33.75V16.25Z" stroke="#EB9032" stroke-width="2"/><path d="M29.6923 31.56C29.6923 32.408 29.5163 33.192 29.1643 33.912C28.8123 34.616 28.2443 35.184 27.4603 35.616C26.6923 36.032 25.6603 36.24 24.3643 36.24C23.6283 36.24 22.8603 36.152 22.0603 35.976C21.2763 35.8 20.4123 35.504 19.4683 35.088L20.3563 32.616C21.2043 32.92 21.9163 33.16 22.4923 33.336C23.0843 33.496 23.6763 33.576 24.2683 33.576C25.1163 33.576 25.7563 33.408 26.1883 33.072C26.6363 32.72 26.8603 32.176 26.8603 31.44V31.104C26.8603 30.496 26.6683 30.008 26.2843 29.64C25.9163 29.256 25.3403 29.064 24.5563 29.064H22.0123V26.472H24.5563C25.3083 26.472 25.8603 26.296 26.2123 25.944C26.5643 25.576 26.7403 25.104 26.7403 24.528V24.192C26.7403 23.456 26.5323 22.928 26.1163 22.608C25.7163 22.272 25.1243 22.104 24.3403 22.104C23.7803 22.104 23.2123 22.184 22.6363 22.344C22.0763 22.488 21.3963 22.712 20.5963 23.016L19.7083 20.544C20.6043 20.128 21.4363 19.84 22.2043 19.68C22.9723 19.52 23.7163 19.44 24.4363 19.44C25.6683 19.44 26.6603 19.648 27.4123 20.064C28.1643 20.48 28.7083 21.04 29.0443 21.744C29.3963 22.448 29.5723 23.224 29.5723 24.072V24.408C29.5723 25.16 29.3803 25.824 28.9963 26.4C28.6283 26.96 28.1403 27.408 27.5323 27.744C28.1883 28.096 28.7083 28.568 29.0923 29.16C29.4923 29.736 29.6923 30.424 29.6923 31.224V31.56Z" fill="#EB9032"/></svg>
                            <svg class="folds" id="fold3fill" width="50" height="52" viewBox="0 0 50 52" fill="none" xmlns="http://www.w3.org/2000/svg"><path d="M3.125 16.25C3.125 12.7497 3.125 10.9995 3.80621 9.66256C4.40542 8.48655 5.36155 7.53042 6.53756 6.93121C7.87451 6.25 9.62467 6.25 13.125 6.25H16.3367C18.4937 6.25 19.5722 6.25 20.5326 6.56946C21.3823 6.85206 22.1612 7.31391 22.8168 7.92375C23.558 8.61312 24.0754 9.55938 25.1104 11.4519L26.3672 13.75H36.875C40.3753 13.75 42.1255 13.75 43.4624 14.4312C44.6385 15.0304 45.5946 15.9865 46.1938 17.1626C46.875 18.4995 46.875 20.2497 46.875 23.75V33.75C46.875 37.2503 46.875 39.0005 46.1938 40.3374C45.5946 41.5135 44.6385 42.4696 43.4624 43.0688C42.1255 43.75 40.3753 43.75 36.875 43.75H13.125C9.62467 43.75 7.87451 43.75 6.53756 43.0688C5.36155 42.4696 4.40542 41.5135 3.80621 40.3374C3.125 39.0005 3.125 37.2503 3.125 33.75V16.25Z" fill="#EB9032" stroke="#EB9032" stroke-width="2"/><path d="M29.6923 31.56C29.6923 32.408 29.5163 33.192 29.1643 33.912C28.8123 34.616 28.2443 35.184 27.4603 35.616C26.6923 36.032 25.6603 36.24 24.3643 36.24C23.6283 36.24 22.8603 36.152 22.0603 35.976C21.2763 35.8 20.4123 35.504 19.4683 35.088L20.3563 32.616C21.2043 32.92 21.9163 33.16 22.4923 33.336C23.0843 33.496 23.6763 33.576 24.2683 33.576C25.1163 33.576 25.7563 33.408 26.1883 33.072C26.6363 32.72 26.8603 32.176 26.8603 31.44V31.104C26.8603 30.496 26.6683 30.008 26.2843 29.64C25.9163 29.256 25.3403 29.064 24.5563 29.064H22.0123V26.472H24.5563C25.3083 26.472 25.8603 26.296 26.2123 25.944C26.5643 25.576 26.7403 25.104 26.7403 24.528V24.192C26.7403 23.456 26.5323 22.928 26.1163 22.608C25.7163 22.272 25.1243 22.104 24.3403 22.104C23.7803 22.104 23.2123 22.184 22.6363 22.344C22.0763 22.488 21.3963 22.712 20.5963 23.016L19.7083 20.544C20.6043 20.128 21.4363 19.84 22.2043 19.68C22.9723 19.52 23.7163 19.44 24.4363 19.44C25.6683 19.44 26.6603 19.648 27.4123 20.064C28.1643 20.48 28.7083 21.04 29.0443 21.744C29.3963 22.448 29.5723 23.224 29.5723 24.072V24.408C29.5723 25.16 29.3803 25.824 28.9963 26.4C28.6283 26.96 28.1403 27.408 27.5323 27.744C28.1883 28.096 28.7083 28.568 29.0923 29.16C29.4923 29.736 29.6923 30.424 29.6923 31.224V31.56Z" fill="#2D2E32"/></svg>
                        </div>
                    </div>
                </div>
            </div>
        </div>

        <div id="fileContainer1">           
            <div class="files">
                <div class="file" onclick="changeSound({ folder: 1, file: 1, name: 'Scream' })">
                    <div class="circle">
                        <p>001</p>
                    </div>
                    <p class="fileName">Scream</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 1, file: 2, name: 'Ack 1' })">
                    <div class="circle">
                        <p>002</p>
                    </div>
                    <p class="fileName">Ack 1</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 1, file: 3, name: 'Ack 2' })">
                    <div class="circle">
                        <p>003</p>
                    </div>
                    <p class="fileName">Ack 2</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 1, file: 4, name: 'Chat' })">
                    <div class="circle">
                        <p>004</p>
                    </div>
                    <p class="fileName">Chat</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 1, file: 5, name: 'Excited 1' })">
                    <div class="circle">
                        <p>005</p>
                    </div>
                    <p class="fileName">Excited 1</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 1, file: 6, name: 'Excited 2' })">
                    <div class="circle">
                        <p>006</p>
                    </div>
                    <p class="fileName">Excited 2</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 1, file: 7, name: 'Shout' })">
                    <div class="circle">
                        <p>007</p>
                    </div>
                    <p class="fileName">Shout</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 1, file: 8, name: 'Worried' })">
                    <div class="circle">
                        <p>008</p>
                    </div>
                    <p class="fileName">Worried</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 1, file: 9, name: 'Crazy' })">
                    <div class="circle">
                        <p>009</p>
                    </div>
                    <p class="fileName">Crazy</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 1, file: 20, name: 'Random Noise' })">
                    <div class="circle">
                        <p>020</p>
                    </div>
                    <p class="fileName">Random Noise</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 1, file: 21, name: 'Random Noise' })">
                    <div class="circle">
                        <p>021</p>
                    </div>
                    <p class="fileName">Random Noise</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 1, file: 22, name: 'Random Noise' })">
                    <div class="circle">
                        <p>022</p>
                    </div>
                    <p class="fileName">Random Noise</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 1, file: 23, name: 'Random Noise' })">
                    <div class="circle">
                        <p>023</p>
                    </div>
                    <p class="fileName">Random Noise</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 1, file: 24, name: 'Random Noise' })">
                    <div class="circle">
                        <p>024</p>
                    </div>
                    <p class="fileName">Random Noise</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 1, file: 25, name: 'Random Noise' })">
                    <div class="circle">
                        <p>025</p>
                    </div>
                    <p class="fileName">Random Noise</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 1, file: 26, name: 'Random Noise' })">
                    <div class="circle">
                        <p>026</p>
                    </div>
                    <p class="fileName">Random Noise</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 1, file: 27, name: 'Random Noise' })">
                    <div class="circle">
                        <p>027</p>
                    </div>
                    <p class="fileName">Random Noise</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 1, file: 28, name: 'Random Noise' })">
                    <div class="circle">
                        <p>028</p>
                    </div>
                    <p class="fileName">Random Noise</p>
                </div>                
            </div>
        </div>

        <div id="fileContainer2">           
            <div class="files">
                <div class="file" onclick="changeSound({ folder: 2, file: 1, name: 'Startup' })">
                    <div class="circle">
                        <p>001</p>
                    </div>
                    <p class="fileName">Startup</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 2, file: 2, name: 'Error' })">
                    <div class="circle">
                        <p>002</p>
                    </div>
                    <p class="fileName">Error</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 2, file: 3, name: 'Hisashiburi' })">
                    <div class="circle">
                        <p>003</p>
                    </div>
                    <p class="fileName">Hisashiburi</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 2, file: 4, name: 'Nani' })">
                    <div class="circle">
                        <p>004</p>
                    </div>
                    <p class="fileName">Nani</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 2, file: 5, name: 'Senpai' })">
                    <div class="circle">
                        <p>005</p>
                    </div>
                    <p class="fileName">Senpai</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 2, file: 6, name: 'Kitten meow' })">
                    <div class="circle">
                        <p>006</p>
                    </div>
                    <p class="fileName">Kitten meow</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 2, file: 7, name: 'Angry cat' })">
                    <div class="circle">
                        <p>007</p>
                    </div>
                    <p class="fileName">Angry cat</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 2, file: 8, name: 'Impostor' })">
                    <div class="circle">
                        <p>008</p>
                    </div>
                    <p class="fileName">Impostor</p>
                </div>
            </div>
        </div>

        <div id="fileContainer3">           
            <div class="files">
                <div class="file" onclick="changeSound({ folder: 3, file: 1, name: 'Jazz' })">
                    <div class="circle">
                        <p>001</p>
                    </div>
                    <p class="fileName">Jazz</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 3, file: 2, name: 'Cool jazz' })">
                    <div class="circle">
                        <p>002</p>
                    </div>
                    <p class="fileName">Cool jazz</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 3, file: 3, name: 'Empire Theme' })">
                    <div class="circle">
                        <p>003</p>
                    </div>
                    <p class="fileName">Empire Theme</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 3, file: 4, name: 'Main Theme' })">
                    <div class="circle">
                        <p>004</p>
                    </div>
                    <p class="fileName">Star Wars Theme</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 3, file: 5, name: 'Still Alive' })">
                    <div class="circle">
                        <p>005</p>
                    </div>
                    <p class="fileName">Still Alive</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 3, file: 6, name: 'Baka Mitai' })">
                    <div class="circle">
                        <p>006</p>
                    </div>
                    <p class="fileName">Baka Mitai</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 3, file: 7, name: 'AC2 Theme' })">
                    <div class="circle">
                        <p>007</p>
                    </div>
                    <p class="fileName">AC2 Theme</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 3, file: 8, name: 'Cyberpunk 2077' })">
                    <div class="circle">
                        <p>008</p>
                    </div>
                    <p class="fileName">Cyberpunk 2077</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 3, file: 9, name: 'San Andreas' })">
                    <div class="circle">
                        <p>009</p>
                    </div>
                    <p class="fileName">San Andreas</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 3, file: 10, name: 'Helldivers 2' })">
                    <div class="circle">
                        <p>010</p>
                    </div>
                    <p class="fileName">Helldivers 2</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 3, file: 11, name: 'Jojo' })">
                    <div class="circle">
                        <p>011</p>
                    </div>
                    <p class="fileName">Jojo</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 3, file: 12, name: 'Megalovania' })">
                    <div class="circle">
                        <p>012</p>
                    </div>
                    <p class="fileName">Megalovania</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 3, file: 13, name: 'Undertale Calm' })">
                    <div class="circle">
                        <p>013</p>
                    </div>
                    <p class="fileName">Undertale Calm</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 3, file: 14, name: 'Spongebob' })">
                    <div class="circle">
                        <p>014</p>
                    </div>
                    <p class="fileName">Spongebob</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 3, file: 15, name: 'Terraria' })">
                    <div class="circle">
                        <p>015</p>
                    </div>
                    <p class="fileName">Terraria</p>
                </div>
            </div>
        </div>
    </div>
</body>
</html>
)rawliteral";

void sendTelegramMessage(String message) {
  HTTPClient http;

  String encodedMessage = message;
  encodedMessage.replace(" ", "%20");
  encodedMessage.replace("\n", "%0A");

  String url = "https://api.telegram.org/" + String(botToken) + "/sendMessage?chat_id=" + String(chatID) + "&text=" + encodedMessage;

  Serial.println("Sending Telegram message...");

  http.begin(url);
  http.setTimeout(5000);    // Set 5s timeout

  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) {
    Serial.println("Message sent successfully!");
  } else {
    Serial.printf("Error sending message: %d\n", httpResponseCode);
  }

  http.end();
}

// This callback gets called any time a new gamepad is connected.
// Up to 4 gamepads can be connected at the same time.
void onConnectedController(ControllerPtr ctl) {
  bool foundEmptySlot = false;
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == nullptr) {
      Serial.printf("CALLBACK: Controller is connected, index=%d\n", i);
      // Additionally, you can get certain gamepad properties like:
      // Model, VID, PID, BTAddr, flags, etc.
      ControllerProperties properties = ctl->getProperties();
      Serial.printf("Controller model: %s, VID=0x%04x, PID=0x%04x\n", ctl->getModelName().c_str(), properties.vendor_id,
                    properties.product_id);
      myControllers[i] = ctl;
      foundEmptySlot = true;
      break;
    }
  }
  battery = ctl->battery() / 2.55;
  Serial.println(battery);
  controllerModel = ctl->getModelName();
  BTConnection = true;
  digitalWrite(BT_PIN, HIGH);
  if (!foundEmptySlot) {
    Serial.println("CALLBACK: Controller connected, but could not found empty slot");
  }
}

void onDisconnectedController(ControllerPtr ctl) {
  bool foundController = false;

  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == ctl) {
      Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
      myControllers[i] = nullptr;
      foundController = true;
      break;
    }
  }

  BTConnection = false;
  digitalWrite(BT_PIN, LOW);
  if (!foundController) {
    Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
  }
}

void dumpGamepad(ControllerPtr ctl) {
  Serial.printf(
    "idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: %4d, %4d, brake: %4d, throttle: %4d, "
    "misc: 0x%02x, gyro x:%6d y:%6d z:%6d, accel x:%6d y:%6d z:%6d\n",
    ctl->index(),        // Controller Index
    ctl->dpad(),         // D-pad
    ctl->buttons(),      // bitmask of pressed buttons
    ctl->axisX(),        // (-511 - 512) left X Axis
    ctl->axisY(),        // (-511 - 512) left Y axis
    ctl->axisRX(),       // (-511 - 512) right X axis
    ctl->axisRY(),       // (-511 - 512) right Y axis
    ctl->brake(),        // (0 - 1023): brake button
    ctl->throttle(),     // (0 - 1023): throttle (AKA gas) button
    ctl->miscButtons(),  // bitmask of pressed "misc" buttons
    ctl->gyroX(),        // Gyro X
    ctl->gyroY(),        // Gyro Y
    ctl->gyroZ(),        // Gyro Z
    ctl->accelX(),       // Accelerometer X
    ctl->accelY(),       // Accelerometer Y
    ctl->accelZ()        // Accelerometer Z
  );
}

void processGamepad(ControllerPtr ctl) {
  if (ctl->a()) {
    static int colorIdx = 0;
    switch (colorIdx % 3) {
      case 0:
        // Red
        ctl->setColorLED(255, 0, 0);
        break;
      case 1:
        // Green
        ctl->setColorLED(112, 243, 254);
        break;
      case 2:
        // Blue
        ctl->setColorLED(0, 0, 255);
        break;
    }
    colorIdx++;
  }

  if (ctl->miscSelect()) {
    Serial.println("Clicked Settings!");
    Serial.printf("Before Toggle: inSettings = %d\n", inSettings);

    if (inSettings) {
      digitalWrite(SETTINGS_PIN, LOW);
      inSettings = false;
    } else {
      digitalWrite(SETTINGS_PIN, HIGH);
      inSettings = true;
    }

    Serial.printf("After Toggle: inSettings = %d\n", inSettings);
  }

  int speedFwd = map(ctl->throttle(), 0, 1023, 120, 255);
  int speedBwd = map(ctl->brake(), 0, 1023, 120, 255);

  if (ctl->r1()) {  // Rotate right
    webControl = false;
    if (speedFwd > 120) {
      ledcWrite(0, 255);                            // LEFT FWD
      ledcWrite(1, 0);                              // LEFT BWD
      ledcWrite(2, constrain(speedFwd, 120, 180));  // RIGHT FWD
      ledcWrite(3, 0);                              // RIGHT BWD
      legSpeed = speedFwd;
    } else if (speedBwd > 120) {
      ledcWrite(0, 0);
      ledcWrite(1, constrain(speedBwd, 120, 180));
      ledcWrite(2, 0);
      ledcWrite(3, 255);
      legSpeed = speedBwd;
    } else {
      ledcWrite(0, 255);
      ledcWrite(1, 0);
      ledcWrite(2, 0);
      ledcWrite(3, 255);
      legSpeed = 255;
    }

  } else if (ctl->l1()) {  // Rotate left
    webControl = false;
    if (speedFwd > 120) {
      ledcWrite(0, constrain(speedFwd, 120, 180));
      ledcWrite(1, 0);
      ledcWrite(2, 255);
      ledcWrite(3, 0);
      legSpeed = speedFwd;
    } else if (speedBwd > 120) {
      ledcWrite(0, 0);
      ledcWrite(1, 255);
      ledcWrite(2, 0);
      ledcWrite(3, constrain(speedBwd, 120, 180));
      legSpeed = speedBwd;
    } else {
      ledcWrite(0, 0);
      ledcWrite(1, 255);
      ledcWrite(2, 255);
      ledcWrite(3, 0);
      legSpeed = 255;
    }
  } else if (ctl->r2()) {  // Move Forward
    webControl = false;
    Serial.printf("Moving forward: Speed %d\n", speedFwd);
    ledcWrite(0, speedFwd);  // Forward motion
    ledcWrite(1, 0);
    ledcWrite(2, speedFwd);  // Forward motion
    ledcWrite(3, 0);
    legSpeed = speedFwd;
  } else if (ctl->l2()) {  // Move Backward
    int speedBwd = map(ctl->brake(), 0, 1023, 140, 255);
    webControl = false;
    ledcWrite(0, 0);
    ledcWrite(1, speedBwd);  // Reverse motion
    ledcWrite(2, 0);
    ledcWrite(3, speedBwd);  // Reverse motion
    legSpeed = speedBwd;
  } else if (ctl->throttle() == 0 && !webControl) {  // Stop Motor
    ledcWrite(0, 0);
    ledcWrite(1, 0);
    ledcWrite(2, 0);
    ledcWrite(3, 0);
    legSpeed = 0;
  }

  if (ctl->dpad() == DPAD_LEFT) {  // Turning head to the left
    headSpeed = 255;
    ledcWrite(4, 255);
    ledcWrite(5, 0);
  } else if (ctl->dpad() == DPAD_RIGHT) {  // Turning head to the right
    headSpeed = 255;
    ledcWrite(4, 0);
    ledcWrite(5, 255);
  } else if (ctl->dpad() == 0) {
    headSpeed = 0;
    ledcWrite(4, 0);
    ledcWrite(5, 0);
  }

  if (ctl->x()) {
    ctl->playDualRumble(0 /* delayedStartMs */, 250 /* durationMs */, 0x80 /* weakMagnitude */,
                        0x40 /* strongMagnitude */);
  }

  if (ctl->y()) {
    myDFPlayer.playFolder(1, 1);
  }

  if (ctl->b()) {
    myDFPlayer.playFolder(folderNum, fileNum);
  }
  // dumpGamepad(ctl);
  battery = ctl->battery() / 2.55;
}

void processControllers() {
  for (auto myController : myControllers) {
    if (myController && myController->isConnected() && myController->hasData()) {
      if (myController->isGamepad()) {
        processGamepad(myController);
      } else {
        Serial.println("Unsupported controller");
      }
    }
  }
}

void printDetail(uint8_t type, int value) {
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

void playRandomFile() {
  if (!muted) {
    unsigned long currentMillis = millis();

    if (currentMillis - lastPlayTime >= playInterval) {
      int randomFile;

      if (random(2) == 0) {
        randomFile = random(2, 10);  // Choose a file from 2-9
      } else {
        randomFile = random(20, 29);  // Choose a file from 20-28
      }

      int randomVolume = random(10, 20);  // Random volume between 10-25
      myDFPlayer.volume(randomVolume);
      myDFPlayer.playFolder(1, randomFile);

      Serial.printf("Playing file %d from folder 1 at volume %d\n", randomFile, randomVolume);

      lastPlayTime = currentMillis;
      playInterval = random(20000, 60001);  // Random interval for next play
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Init LEDS
  pinMode(WIFI_PIN, OUTPUT);
  digitalWrite(WIFI_PIN, LOW);
  pinMode(BT_PIN, OUTPUT);
  digitalWrite(BT_PIN, LOW);
  pinMode(SETTINGS_PIN, OUTPUT);
  digitalWrite(SETTINGS_PIN, LOW);

  Serial.printf("Firmware: %s\n", BP32.firmwareVersion());

  const uint8_t *addr = BP32.localBdAddress();
  if (addr) {
    char macStr[18];  // Buffer for formatted MAC address (17 chars + null terminator)
    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
    macAddress = String(macStr);
    Serial.println("BD Addr: " + macAddress);
  } else {
    Serial.println("Error: Failed to get Bluetooth address.");
  }

  // DFPlayer Mini initialization
  DFSerial.begin(9600, SERIAL_8N1, 16, 17);
  Serial.println("Initializing DFPlayer...");
  if (!myDFPlayer.begin(DFSerial)) {
    Serial.println("DFPlayer Mini not detected!");
    return;  // Exit setup to avoid further issues
  }

  Serial.println("DFPlayer Mini Ready.");
  DFConnection = true;
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
  volume = 20;
  myDFPlayer.volume(volume);
  myDFPlayer.playFolder(2, 1);

  // WiFi connection with timeout
  WiFi.begin(ssid, password);
  unsigned long wifiStart = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - wifiStart) < 15000) {
    digitalWrite(WIFI_PIN, HIGH);
    delay(500);
    digitalWrite(WIFI_PIN, LOW);
    delay(500);
    Serial.println("Connecting to WiFi...");
  }

  // WiFi status check
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to WiFi!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    digitalWrite(WIFI_PIN, HIGH);

    sendTelegramMessage("ESP32 Server is online!\nIP Address: " + WiFi.localIP().toString());
  } else {
    Serial.println("WiFi connection failed. Continuing without WiFi...");
    digitalWrite(WIFI_PIN, LOW);
    myDFPlayer.playFolder(2, 2);
  }



  // Start Web Server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  });

  server.on("/ip", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", WiFi.localIP().toString().c_str());
  });

  server.on("/battery", HTTP_GET, [](AsyncWebServerRequest *request) {
    String batteryStr;

    if (BTConnection) {
      batteryStr = String(battery) + "%";
    } else {
      batteryStr = "Controller not connected";
    }

    request->send(200, "text/plain", batteryStr.c_str());
  });

  server.on("/mac", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", macAddress.c_str());
  });

  server.on("/btcon", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", BTConnection ? "Connected" : "Not Connected");
  });

  server.on("/dfcon", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", DFConnection ? "Connected" : "Not Connected");
  });

  server.on("/model", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", controllerModel.c_str());
  });

  server.on("/legs", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(legSpeed));
  });

  server.on("/head", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(headSpeed));
  });

  server.on("/changeSound", HTTP_GET, [](AsyncWebServerRequest *request) {

    // Get the folder and file from the request parameters
    if (request->hasParam("folder") && request->hasParam("file")) {
      folderNum = request->getParam("folder")->value().toInt();
      fileNum = request->getParam("file")->value().toInt();
      request->send(204);
    } else {
      request->send(400, "text/plain", "Missing folder or file parameter");
    }
  });

  server.on("/playSound", HTTP_GET, [](AsyncWebServerRequest *request) {
    myDFPlayer.playFolder(folderNum, fileNum);
    request->send(204);
  });

  server.on("/stopSound", HTTP_GET, [](AsyncWebServerRequest *request) {
    myDFPlayer.stop();
    request->send(204);
  });

  server.on("/muteSound", HTTP_GET, [](AsyncWebServerRequest *request) {
    muted = true;
    request->send(204);
  });

  server.on("/unmuteSound", HTTP_GET, [](AsyncWebServerRequest *request) {
    muted = false;
    request->send(204);
  });

  server.on("/setVolume", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("level")) {
      volume = request->getParam("level")->value().toInt();
      volume = constrain(volume, 0, 30);
      myDFPlayer.volume(volume);
    }
    request->send(204);
  });

  server.on("/turnHead", HTTP_GET, [](AsyncWebServerRequest *request) {
    String dir = request->getParam("dir")->value();
    int speed = 255;

    if (dir == "left") {
      webControl = true;
      headSpeed = speed;
      ledcWrite(4, speed);
      ledcWrite(5, 0);
    } else if (dir == "right") {
      webControl = true;
      headSpeed = speed;
      ledcWrite(4, 0);
      ledcWrite(5, speed);
    } else if (dir == "stop") {
      webControl = true;
      headSpeed = 0;
      ledcWrite(4, 0);
      ledcWrite(5, 0);
    }
    request->send(204);
  });

  server.on("/move", HTTP_GET, [](AsyncWebServerRequest *request) {
    String dir = request->getParam("dir")->value();

    if (dir == "forward") {
      webControl = true;
      int speedFwd = 255;
      legSpeed = speedFwd;

      // Set motor speeds for forward movement
      ledcWrite(0, speedFwd);  // Forward motion
      ledcWrite(1, 0);
      ledcWrite(2, speedFwd);  // Forward motion
      ledcWrite(3, 0);

      request->send(204);
    } else if (dir == "backward") {
      webControl = true;
      int speedBwd = 255;
      legSpeed = speedBwd;

      ledcWrite(0, 0);
      ledcWrite(1, speedBwd);  // Reverse motion
      ledcWrite(2, 0);
      ledcWrite(3, speedBwd);  // Reverse motion

      request->send(204);
    } else if (dir == "stop") {
      webControl = true;
      Serial.printf("Stopped moving\n");
      ledcWrite(0, 0);
      ledcWrite(1, 0);
      ledcWrite(2, 0);
      ledcWrite(3, 0);
      legSpeed = 0;

      request->send(204);
    } else {
      request->send(400, "text/plain", "Invalid direction");
    }
  });

  server.on("/turn", HTTP_GET, [](AsyncWebServerRequest *request) {
    String dir = request->getParam("dir")->value();
    int speed = 255;

    if (dir == "left") {
      legSpeed = speed;
      webControl = true;
      ledcWrite(0, 0);      // LEFT FWD
      ledcWrite(1, speed);  // LEFT BWD
      ledcWrite(2, speed);  // RIGHT FWD
      ledcWrite(3, 0);      // RIGHT BWD
    } else if (dir == "right") {
      legSpeed = speed;
      webControl = true;
      ledcWrite(0, speed);  // LEFT FWD
      ledcWrite(1, 0);      // LEFT BWD
      ledcWrite(2, 0);      // RIGHT FWD
      ledcWrite(3, speed);  // RIGHT BWD
    } else if (dir == "stop") {
      legSpeed = 0;
      webControl = true;
      ledcWrite(0, 0);
      ledcWrite(1, 0);
      ledcWrite(2, 0);
      ledcWrite(3, 0);
    }
    request->send(204);
  });

  server.begin();

  ledcSetup(0, 1000, 8);
  ledcSetup(1, 1000, 8);
  ledcSetup(2, 1000, 8);
  ledcSetup(3, 1000, 8);
  ledcSetup(4, 1000, 8);
  ledcSetup(5, 1000, 8);

  ledcAttachPin(R_MOTOR_PWM1, 0);
  ledcAttachPin(R_MOTOR_PWM2, 1);
  ledcAttachPin(L_MOTOR_PWM1, 2);
  ledcAttachPin(L_MOTOR_PWM2, 3);
  ledcAttachPin(HEAD_MOTOR_PWM1, 4);
  ledcAttachPin(HEAD_MOTOR_PWM2, 5);

  ledcWrite(0, 0);
  ledcWrite(1, 0);
  ledcWrite(2, 0);
  ledcWrite(3, 0);
  ledcWrite(4, 0);
  ledcWrite(5, 0);

  BP32.setup(&onConnectedController, &onDisconnectedController);
  BP32.forgetBluetoothKeys();
  BP32.enableVirtualDevice(false);
}

void loop() {
  if (BP32.update())
    processControllers();

  delay(150);

  if (myDFPlayer.available()) {
    printDetail(myDFPlayer.readType(), myDFPlayer.read());  //Print the detail message from DFPlayer to handle different errors and states.
  }

  playRandomFile();
}
