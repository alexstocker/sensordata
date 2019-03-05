/*
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * * Neither the name of Majenko Technologies nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const int led = LED_BUILTIN;
const char* ssid = ""; // network ssid
const char* password = ""; // network passphrase
const char* apiUrl = "http://IP-ADDRESS-OR-HOST/index.php/apps/sensorlogger/api/v1/createlog/";
const char* username = ""; // username
const char* token = ""; // token or password
const char* host = "128.138.141.172"; // NTP Pool or Host

String dateTime = "";

ESP8266WebServer server(80);

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from Hell");
  digitalWrite(led, 0);
}

void getDateFromNtp() {
  WiFiClient client;
  const int httpPort = 13;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    delay(1000);
  }
  client.print("HEAD / HTTP/1.1\r\nAccept: */*\r\nUser-Agent: Mozilla/4.0 (compatible; ESP8266 NodeMcu Lua;)\r\n\r\n");
  delay(100);
  char buffer[12];
  while(client.available()) {
    String line = client.readStringUntil('\r');
    if (line.indexOf("Date") != -1) {

    } else {
      dateTime = line.substring(7, 24);
      Serial.println(dateTime);
    }
  }
}

void handleNotFound(){
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void){
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
  if(WiFi.status() == WL_CONNECTED) {
      DynamicJsonDocument root(1024);

    getDateFromNtp();

    int rTemp = rand() % 20; // random int
    int rHum = rand() % 99; // random int

    digitalWrite(led, 0);

    HTTPClient http;

    http.begin(apiUrl);

    http.setAuthorization(username,token);
    http.addHeader("Content-Type", "application/json");

    root["deviceId"] = "0100110-001100110-1101";
    root["temperature"] = rTemp;
    root["humidity"] = rHum;
    root["date"] = "20"+dateTime;

    String jsonStr;
    serializeJson(root, jsonStr);
    int httpCode = http.POST(jsonStr);

    http.end();

    if(httpCode != 200) {
      Serial.print("ERROR HTTP Response Code ");
      Serial.println(httpCode);
          digitalWrite(led,1);
          delay(250);
          digitalWrite(led,0);
          delay(250);
          digitalWrite(led,1);
          delay(250);
          digitalWrite(led,0);
          delay(1000);
    } else {
      digitalWrite(led, 1);
      delay(10000);
    }
  }else{
    digitalWrite(led,0);
    delay(250);
    digitalWrite(led,1);
    delay(5000);
 }

}
