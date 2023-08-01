/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-esp8266-web-server-outputs-momentary-switch/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include <Wire.h>
#include <TEA5767.h>

TEA5767 radio = TEA5767();

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "DECOX20";
const char* password = "1155668800";

const int output = 2;
const float frequencyStep = 0.1; // 0.1 MHz step

// HTML web page
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
  <head>
    <title>ESP Pushbutton Web Server</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      body { font-family: Arial; text-align: center; margin:0px auto; padding-top: 30px;}
      .button {
        padding: 10px 20px;
        font-size: 24px;
        text-align: center;
        outline: none;
        color: #fff;
        background-color: #2f4468;
        border: none;
        border-radius: 5px;
        box-shadow: 0 6px #999;
        cursor: pointer;
        -webkit-touch-callout: none;
        -webkit-user-select: none;
        -khtml-user-select: none;
        -moz-user-select: none;
        -ms-user-select: none;
        user-select: none;
        -webkit-tap-highlight-color: rgba(0,0,0,0);
      }  
      .button:hover {background-color: #1f2e45}
      .button:active {
        background-color: #1f2e45;
        box-shadow: 0 4px #666;
        transform: translateY(2px);
      }
    </style>
  </head>
  <body>
    <h1>TEA5767 Radio Web Server</h1>
    <button class="button" onclick="setRadioFrequency(88.5);">88.50 MHz</button>
    <button class="button" onclick="setRadioFrequency(89.3);">89.30 MHz</button><br><br>
    <button class="button" onclick="setRadioFrequency(90.3);">90.30 MHz</button>
    <button class="button" onclick="setRadioFrequency(91.3);">91.30 MHz</button><br><br>
    <button class="button" onclick="setRadioFrequency(92.5);">92.50 MHz</button>
    <button class="button" onclick="setRadioFrequency(94.3);">94.30 MHz</button><br><br>
    <button class="button" onclick="setRadioFrequency(95.1);">95.10 MHz</button>
    <button class="button" onclick="setRadioFrequency(95.9);">95.90 MHz</button><br><br>
    <button class="button" onclick="setRadioFrequency(96.9);">96.90 MHz</button>
    <button class="button" onclick="setRadioFrequency(97.7);">97.70 MHz</button><br><br>
    <button class="button" onclick="setRadioFrequency(98.5);">98.50 MHz</button>
    <button class="button" onclick="setRadioFrequency(100.7);">100.70 MHz</button><br><br>
    <button class="button" onclick="setRadioFrequency(103.3);">103.30 MHz</button>
    <button class="button" onclick="setRadioFrequency(105.7);">105.70 MHz</button><br><br>
    <button class="button" onclick="setRadioFrequency(107.3);">107.30 MHz</button>
    <button class="button" onclick="incrementFrequency();">+</button>
    <button class="button" onclick="decrementFrequency();">-</button><br><br>
   <script>

   function setRadioFrequency(frequency) {
     var xhr = new XMLHttpRequest();
     xhr.open("GET", "/radio?f=" + frequency, true);
     xhr.send();
  }

  function incrementFrequency() {
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/increment", true);
    xhr.send();
  }

  function decrementFrequency() {
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/decrement", true);
    xhr.send();
  }
  </script>
  </body>
</html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

AsyncWebServer server(80);

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.println();
  Serial.print("ESP IP Address: http://");
  Serial.println(WiFi.localIP());
  
  Wire.begin();

  // Send web page to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // Receive an HTTP GET request to set the radio frequency
  server.on("/radio", HTTP_GET, [] (AsyncWebServerRequest *request) {
    if (request->hasParam("f")) {
      String freqParam = request->getParam("f")->value();
      float frequency = freqParam.toFloat();
      radio.setFrequency(frequency);
      request->send(200, "text/plain", "ok");
    } else {
      request->send(400, "text/plain", "Bad Request");
    }
  });

  // Receive an HTTP GET request to increment the radio frequency
  server.on("/increment", HTTP_GET, [] (AsyncWebServerRequest *request) {
    radio.setFrequency(radio.getFrequency() + frequencyStep);
    request->send(200, "text/plain", "ok");
  });

  // Receive an HTTP GET request to decrement the radio frequency
  server.on("/decrement", HTTP_GET, [] (AsyncWebServerRequest *request) {
    radio.setFrequency(radio.getFrequency() - frequencyStep);
    request->send(200, "text/plain", "ok");
  });
  
  server.onNotFound(notFound);
  server.begin();
}

void loop() {
 
}