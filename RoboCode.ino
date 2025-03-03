#include <Bluepad32.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"

const char* ssid = "Redmi_Huxy";
const char* password = "Wh1rlW1nd@03";
AsyncWebServer server(80);

#define R_MOTOR_PWM1 33 // Right foot pins
#define R_MOTOR_PWM2 15
#define L_MOTOR_PWM1 32 // Left foot pins
#define L_MOTOR_PWM2 14

HardwareSerial DFSerial(2);
DFRobotDFPlayerMini myDFPlayer;

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

uint8_t battery = 0;
bool BTConnection = false;
String controllerModel = "Controller not connected";
String macAddress = "Error...";
bool DFConnection = false;

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
        #sound button:focus { background-color: var(--active-color); }
        .folder { display: flex; gap: 2em; }
    </style>
    <script>
        function updateBattery() {
            fetch('/battery')
                .then(response => {
                    if (!response.ok) {
                        throw new Error('Failed to fetch battery state');
                    }
                    return response.text();
                })
                .then(data => document.getElementById('battery').innerText = data + "%")
                .catch(error => {
                    console.error('Error fetching battery state:', error);
                    document.getElementById('battery').innerText = "Error...";
                });
        }

        function playTrack(track) {
            fetch('/play?track=' + track)
                .catch(error => console.error('Error playing track:', error));
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


        function updateStatus() {
            checkBTConnection();
            updateModel();
            updateBattery();
            updateDFConnection();
            updateMac();
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
                            <div class="grid-item gib"><svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24"><g fill="none" stroke="#2D2E32" stroke-linecap="round" stroke-linejoin="round" stroke-width="3"><path d="m8 5l-5 5l5 5"/><path d="M3 10h8c5.523 0 10 4.477 10 10v1"/></g></svg></div>
                            <div class="grid-item gib flip"><svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24"><g fill="none" stroke="#2D2E32" stroke-linecap="round" stroke-linejoin="round" stroke-width="3"><path d="m8 5l-5 5l5 5"/><path d="M3 10h8c5.523 0 10 4.477 10 10v1"/></g></svg></div>
                        </div>
                    </div>
                    <div>
                        <p>Body</p>
                        <div class="grid-container">
                            <div class="grid-item"></div>
                            <div class="grid-item gib"><svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24"><path fill="none" stroke="#2D2E32" stroke-linecap="round" stroke-linejoin="round" stroke-width="3" d="M12 20V4m-7 7l7-7l7 7"/></svg></div>
                            <div class="grid-item"></div>
                            <div class="grid-item gib"><svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24"><path fill="none" stroke="#2D2E32" stroke-linecap="round" stroke-linejoin="round" stroke-width="3" d="m11 5l-7 7l7 7m-7-7h16"/></svg></div>
                            <div class="grid-item gib"><svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24"><path fill="none" stroke="#2D2E32" stroke-linecap="round" stroke-linejoin="round" stroke-width="3" d="M12 20V4m-7 9l7 7l7-7"/></svg></div>
                            <div class="grid-item gib"><svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24"><path fill="none" stroke="#2D2E32" stroke-linecap="round" stroke-linejoin="round" stroke-width="3" d="M4 12h16m-7-7l7 7l-7 7"/></svg></div>
                        </div>
                    </div>
                </div>

                <div id="sound">
                    <p>Current sound:<br><strong>/01/001.mp3 - Scream</strong></p>

                    <!-- & playing... -->
                    <button>Play sound</button> 
                    <button>Change sound</button>
                    <!-- & unmute -->
                    <button>Mute</button>

                    <div class="folder">
                        <img src="https://i.imgur.com/nspH2Hy.png" alt="Folder icon 1">
                        <p>R2D2 sounds</p>
                    </div>
                    <div class="folder">
                        <img src="https://i.imgur.com/xQulkar.png" alt="Folder icon 2">
                        <p>Other sounds</p>
                    </div>
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
    controllerModel = ctl->getModelName();
    BTConnection = true;
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

    if (ctl->r1()) {
      Serial.println("Turning right");
    }

    if (ctl->l1()) {
      Serial.println("Turning left");
    }

    if (ctl->r2()) {  // Move Forward
        int speedFwd = map(ctl->throttle(), 0, 1023, 140, 255);
        Serial.printf("Moving forward: Speed %d\n", speedFwd);
        ledcWrite(0, speedFwd);  // Forward motion
        ledcWrite(1, 0);
        ledcWrite(2, speedFwd);  // Forward motion
        ledcWrite(3, 0);
    } else if (ctl->l2()) {  // Move Backward
        int speedBwd = map(ctl->brake(), 0, 1023, 140, 255);
        Serial.printf("Moving backward: Speed %d\n", speedBwd);
        ledcWrite(0, 0);
        ledcWrite(1, speedBwd);  // Reverse motion
        ledcWrite(2, 0);
        ledcWrite(3, speedBwd);  // Reverse motion
    } else {  // Stop Motor
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

    // Get and check Bluetooth address
    Serial.printf("Firmware: %s\n", BP32.firmwareVersion());

    const uint8_t* addr = BP32.localBdAddress();
    if (addr) {
        char macStr[18]; // Buffer for formatted MAC address (17 chars + null terminator)
        sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", 
                addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

        macAddress = String(macStr); // Convert char array to String
        Serial.println("BD Addr: " + macAddress);
    } else {
        Serial.println("Error: Failed to get Bluetooth address.");
    }

    // WiFi connection with timeout
    WiFi.begin(ssid, password);
    unsigned long wifiStart = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - wifiStart) < 15000) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }

    // WiFi status check
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Connected to WiFi!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());

    } else {
        Serial.println("WiFi connection failed. Continuing without WiFi...");
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
    myDFPlayer.volume(30);

    // Start Web Server
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/html", index_html);
    });

    server.on("/ip", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", WiFi.localIP().toString().c_str());
    });

    server.on("/battery", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", String(battery).c_str());
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

    server.on("/play", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (request->hasParam("track")) {
            int trackNum = request->getParam("track")->value().toInt();
            Serial.printf("Playing track %d\n", trackNum);
            request->send(200, "text/plain", "Playing track " + String(trackNum));
        } else {
            request->send(400, "text/plain", "Invalid request");
        }
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
