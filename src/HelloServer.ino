#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <MAX6675-library/max6675.h>

const char* ssid = "IoToaster";
const char* password = "calhacks";
const int ktc_SO = 12;
const int ktc_CS = 13;
const int ktc_CLK = 14;
const int ssr_1 = 10;
const int ssr_2 = 9;

MAX6675 ktc(ktc_CLK, ktc_CS, ktc_SO);

MDNSResponder mdns;

ESP8266WebServer server(80);

const int led = 13;

void handleRoot() {
  digitalWrite(led, 1);
  double temperature = ktc.readCelsius();
  String toprint = "";
  toprint += "Celcius: ";
  toprint += temperature;
  if( temperature > 25 )
  {
    toprint += "\nRelay is on";
    digitalWrite(ssr_1, 1);
  }
  else
  {
    toprint += "\nRelay is off";
    digitalWrite(ssr_1, 0);
  }
  server.send(200, "text/plain", toprint);
  digitalWrite(led, 0);
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

  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}
