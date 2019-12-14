#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
int i=0;
ESP8266WebServer server;
uint8_t pin_led = 16;
char* ssid = "barsa01_fpkhr";
char* password = "FL2k76eXoP";

#define WU_API_KEY "78f9532e84f04355b9532e84f063554a"
#define WU_LOCATION "Nepal/Pokhara"
#define WU_URL "api.wunderground.com"

static char respBuffer[4096];

void setup(){
  pinMode(pin_led, OUTPUT);
  WiFi.begin(ssid, password);
  Serial.begin(115200);
  while(WiFi.status()!= WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
    
  }
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/",[](){server.send(200,"text/plain", "Hello World!");});
  server.on("/toggle", toggleLED);
  server.on("/getdata", getData);
  server.begin();
}


void loop(){
  server.handleClient();
}

void toggleLED(){
  digitalWrite(pin_led, !digitalRead(pin_led));
  server.send(204, "");
}

WiFiClient client;

void getData(){
  const char request[]=
  "GET https://samples.openweathermap.org/data/2.5/history/city?q=Pokhara,Nepal&appid=b1b15e88fa797225412429c1c50c122a1.json HTTP/1.1\r\n"
  "User-Agent: ESP8266/0.1\r\n"
  "Accept: */*\r\n"
  "Host: "WU_URL "\r\n"
  "Connection: close\r\n"
  "r\n";
  Serial.println(request);
  client.flush();
  delay(1000);
  while(client.connected())
  {      //respBuffer[i++] = client.read();

    if(client.available())
    {
      respBuffer[i++] = client.read();
      delay(1);
    }
  }
  client.stop();
  char* json = strchr(respBuffer,'{');

  DynamicJsonBuffer jBuffer;
  JsonObject& root = jBuffer.parseObject(json);
  JsonObject& current = root["current_observation"];
  String temp = current["pressure"];
  String weather = current["weather"];
  String msg = "Temp(C): "+ temp +", Weather: "+ weather;
  server.send(150, "text/plain", msg);
  
}





  
