#include <Bluepad32.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"

const char* ssid = "Redmi_Huxy";
const char* password = "Wh1rlW1nd@03";
AsyncWebServer server(80);

#define R_MOTOR_PWM1 15 // Right foot pins
#define R_MOTOR_PWM2 33
#define L_MOTOR_PWM1 14 // Left foot pins
#define L_MOTOR_PWM2 32

#define WIFI_PIN 27
#define SETTINGS_PIN 13
#define BT_PIN 12

HardwareSerial DFSerial(2);
DFRobotDFPlayerMini myDFPlayer;

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

uint8_t battery = 0;
bool BTConnection = false;
String controllerModel = "Controller not connected";
String macAddress = "Error...";
bool DFConnection = false;
bool inSettings = false;
bool webControl = false;
int folderNum = 1;
int fileNum = 1;

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

        #leftDiv { width: 18.6em; }

        #r2s {
            height: 20em;
            position: relative;
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
            gap: 8em;
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

        function changeFolder(folderNum) {
            const folders = [1, 2];
            
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
    </script>
</head>
<body>
    <div id="mainContainer">
        <h1>R2D2 Controller</h1>

        <div id="mainDiv">
            <div id="leftDiv">
                <div id="r2s">
                    <img id="r2img"src="https://i.imgur.com/zJa6jQe.png" alt="R2D2">
                    <img src="https://i.imgur.com/iss6W4f.png" alt="R2D2-left">
                    <img src="https://i.imgur.com/fEttW5p.png" alt="R2D2-right">
                    <img src="https://i.imgur.com/6LFWx3D.png" alt="R2D2-head">
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

            <div>
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
                <div class="file" onclick="changeSound({ folder: 2, file: 1, name: 'Jazz' })">
                    <div class="circle">
                        <p>001</p>
                    </div>
                    <p class="fileName">Jazz</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 2, file: 2, name: 'Switch Off' })">
                    <div class="circle">
                        <p>002</p>
                    </div>
                    <p class="fileName">Switch Off</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 2, file: 3, name: 'All Systems' })">
                    <div class="circle">
                        <p>003</p>
                    </div>
                    <p class="fileName">All Systems</p>
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
                <div class="file" onclick="changeSound({ folder: 2, file: 6, name: 'Fool' })">
                    <div class="circle">
                        <p>006</p>
                    </div>
                    <p class="fileName">Fool</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 2, file: 7, name: 'Destroy' })">
                    <div class="circle">
                        <p>007</p>
                    </div>
                    <p class="fileName">Destroy</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 2, file: 8, name: 'Be back' })">
                    <div class="circle">
                        <p>008</p>
                    </div>
                    <p class="fileName">Be back</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 2, file: 9, name: 'Drug Dealer' })">
                    <div class="circle">
                        <p>009</p>
                    </div>
                    <p class="fileName">Drug Dealer</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 2, file: 10, name: 'Impostor' })">
                    <div class="circle">
                        <p>010</p>
                    </div>
                    <p class="fileName">Impostor</p>
                </div>
                <div class="file" onclick="changeSound({ folder: 2, file: 11, name: 'FU' })">
                    <div class="circle">
                        <p>011</p>
                    </div>
                    <p class="fileName">FU</p>
                </div>
            </div>
        </div>
    </div>
</body>
</html>
)rawliteral";

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
        // Some gamepads like DS4 and DualSense support changing the color LED.
        // It is possible to change it by calling:
        switch (colorIdx % 3) {
            case 0:
                // Red
                ctl->setColorLED(255, 0, 0);
                break;
            case 1:
                // Green
                ctl->setColorLED(0, 255, 0);
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


    if (ctl->r1()) {
      Serial.println("Turning right");
      webControl = false;
    }

    if (ctl->l1()) {
      Serial.println("Turning left");
      webControl = false;
    }

    if (ctl->r2()) {  // Move Forward
        int speedFwd = map(ctl->throttle(), 0, 1023, 140, 255);
        webControl = false;
        Serial.printf("Moving forward: Speed %d\n", speedFwd);
        ledcWrite(0, speedFwd);  // Forward motion
        ledcWrite(1, 0);
        ledcWrite(2, speedFwd);  // Forward motion
        ledcWrite(3, 0);
    } else if (ctl->l2()) {  // Move Backward
        int speedBwd = map(ctl->brake(), 0, 1023, 140, 255);
        webControl = false;
        Serial.printf("Moving backward: Speed %d\n", speedBwd);
        ledcWrite(0, 0);
        ledcWrite(1, speedBwd);  // Reverse motion
        ledcWrite(2, 0);
        ledcWrite(3, speedBwd);  // Reverse motion
    } else if (ctl->throttle() == 0 && !webControl) {  // Stop Motor
        ledcWrite(0, 0);
        ledcWrite(1, 0);
        ledcWrite(2, 0);
        ledcWrite(3, 0);
    }

    if (ctl->x()) {
        // Some gamepads like DS3, DS4, DualSense, Switch, Xbox One S, Stadia support rumble.
        // It is possible to set it by calling:
        // Some controllers have two motors: "strong motor", "weak motor".
        // It is possible to control them independently.
        ctl->playDualRumble(0 /* delayedStartMs */, 250 /* durationMs */, 0x80 /* weakMagnitude */,
                            0x40 /* strongMagnitude */);
    }

    if (ctl->y()) {
      Serial.println("Playing sound 1...");
      myDFPlayer.playFolder(1, 1);
    }

    if (ctl->b()) {
      Serial.println("Playing sound 2");
      myDFPlayer.playFolder(2, 1);
    }

    // Another way to query controller data is by getting the buttons() function.
    // See how the different "dump*" functions dump the Controller info.
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

void printDetail(uint8_t type, int value){
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

    const uint8_t* addr = BP32.localBdAddress();
    if (addr) {
        char macStr[18]; // Buffer for formatted MAC address (17 chars + null terminator)
        sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", 
                addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

        macAddress = String(macStr);
        Serial.println("BD Addr: " + macAddress);
    } else {
        Serial.println("Error: Failed to get Bluetooth address.");
    }

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
    } else {
        Serial.println("WiFi connection failed. Continuing without WiFi...");
        digitalWrite(WIFI_PIN, LOW);
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
    myDFPlayer.volume(25);

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
            batteryStr = String(battery) + "%";  // Append % to battery value
        } else {
            batteryStr = "Controller not connected";  // Or set to a default value or error message
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

    server.on("/changeSound", HTTP_GET, [](AsyncWebServerRequest *request) {
        String folder;
        String file;

        // Get the folder and file from the request parameters
        if (request->hasParam("folder") && request->hasParam("file")) {
            folder = request->getParam("folder")->value();
            file = request->getParam("file")->value();

            // Play the new sound file using the DFPlayer
            folderNum = folder.toInt();
            fileNum = file.toInt();

            // Send a response to the client
            request->send(204);
        } else {
            // If the parameters are not found, return an error
            request->send(400, "text/plain", "Missing folder or file parameter");
        }
    });

    server.on("/playSound", HTTP_GET, [](AsyncWebServerRequest *request) {
        myDFPlayer.playFolder(folderNum, fileNum);  // Play the sound using the DFPlayer
        request->send(204);
    });

    server.on("/stopSound", HTTP_GET, [](AsyncWebServerRequest *request) {
        myDFPlayer.stop();    // Stop the currently playing sound
        request->send(204);
    });

    server.on("/muteSound", HTTP_GET, [](AsyncWebServerRequest *request) {
        myDFPlayer.volume(0); // Mute the sound
        request->send(204);
    });

    server.on("/unmuteSound", HTTP_GET, [](AsyncWebServerRequest *request) {
        myDFPlayer.volume(25); // Mute the sound
        request->send(204);
    });

    server.on("/setVolume", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (request->hasParam("level")) {
            int volume = request->getParam("level")->value().toInt();
            volume = constrain(volume, 0, 30);  // Ensure the volume is within range
            myDFPlayer.volume(volume);
        }
        request->send(204);
    });

    server.on("/turnHead", HTTP_GET, [](AsyncWebServerRequest *request) {
        String dir = request->getParam("dir")->value();  // Get direction from query parameter
        Serial.println("Turning head on direction: " + dir);
        // request->send(200, "text/plain", "Head turned to " + dir);  // Response to the client
        request->send(204);
    });

    server.on("/move", HTTP_GET, [](AsyncWebServerRequest *request) {
        String dir = request->getParam("dir")->value();  // Get direction from query parameter
        
        if (dir == "forward") {
          webControl = true;
            int speedFwd = 254; // Adjust speed as needed (can also be dynamic like the gamepad version)
            Serial.printf("Moving forward: Speed %d\n", speedFwd);

            // Set motor speeds for forward movement
            ledcWrite(0, speedFwd);  // Forward motion
            ledcWrite(1, 0);
            ledcWrite(2, speedFwd);  // Forward motion
            ledcWrite(3, 0);

            // request->send(200, "text/plain", "Moving forward");
            request->send(204);
        } else if (dir == "backward") {
            webControl = true;
            int speedBwd = 254; // Adjust speed as needed
            Serial.printf("Moving backward: Speed %d\n", speedBwd);

            // Set motor speeds for backward movement
            ledcWrite(0, 0);
            ledcWrite(1, speedBwd);  // Reverse motion
            ledcWrite(2, 0);
            ledcWrite(3, speedBwd);  // Reverse motion

            request->send(204);
        } else if (dir == "stop") {
            webControl = true;
            int speedBwd = 0; // Adjust speed as needed
            Serial.printf("Stopped moving\n");

            // Set motor speeds for backward movement
            ledcWrite(0, 0);
            ledcWrite(1, speedBwd);  // Reverse motion
            ledcWrite(2, 0);
            ledcWrite(3, speedBwd);  // Reverse motion

            request->send(204);
        } else {
            request->send(400, "text/plain", "Invalid direction");
        }
    });

    server.on("/turn", HTTP_GET, [](AsyncWebServerRequest *request) {
        String dir = request->getParam("dir")->value();  // Get direction from query parameter
        Serial.println("Turning in direction: " + dir);
        request->send(204);
    });

    server.begin();

    // Motor PWM Setup
    pinMode(R_MOTOR_PWM1, OUTPUT);
    pinMode(R_MOTOR_PWM2, OUTPUT);

    ledcSetup(0, 1000, 8);
    ledcSetup(1, 1000, 8);
    ledcSetup(2, 1000, 8);
    ledcSetup(3, 1000, 8);

    ledcAttachPin(R_MOTOR_PWM1, 0);
    ledcAttachPin(R_MOTOR_PWM2, 1);
    ledcAttachPin(L_MOTOR_PWM1, 2);
    ledcAttachPin(L_MOTOR_PWM2, 3);

    ledcWrite(0, 0);
    ledcWrite(1, 0);
    ledcWrite(2, 0);
    ledcWrite(3, 0);

    // Bluepad32 setup
    BP32.setup(&onConnectedController, &onDisconnectedController);
    BP32.forgetBluetoothKeys();
    BP32.enableVirtualDevice(false);
}


// Arduino loop function. Runs in CPU 1.
void loop() {
    // This call fetches all the controllers' data.
    // Call this function in your main loop.
    bool dataUpdated = BP32.update();
    if (dataUpdated)
        processControllers();

    // The main loop must have some kind of "yield to lower priority task" event.
    // Otherwise, the watchdog will get triggered.
    // If your main loop doesn't have one, just add a simple `vTaskDelay(1)`.
    // Detailed info here:
    // https://stackoverflow.com/questions/66278271/task-watchdog-got-triggered-the-tasks-did-not-reset-the-watchdog-in-time

    //     vTaskDelay(1);
    delay(150);

    if (myDFPlayer.available()) {
      printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
    }
}
