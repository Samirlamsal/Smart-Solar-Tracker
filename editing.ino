#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

ESP8266WebServer server;
uint8_t pin_led = 16;
char* ssid = "barsa01_fpkhr";
char* password = "FL2k76eXoP";

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
  "GET /data/2.5/weather?q=Pokhara, NP&APPID=46781e6fdefe1ac670c572907fa80b7d HTTP/1.1\r\n"
  "User-Agent: ESP8266/0.1\r\n"
  "Accept: */*\r\n"
  "Host: api.openweathermap.org\r\n"
  "Connection: close\r\n"
  "\r\n";
  Serial.println(request);
  //client.flush();
  delay(1000);
  uint16_t index = 0;
  client.connect("api.openweathermap.org",80);
  while(!client.connected()){}
  client.print(request);
  if(client.connected())
  {      //respBuffer[i++] = client.read();
    while(!client.available()){}
    while(client.available())
    {
      respBuffer[index++] = client.read();
      delay(1);
    }
    char* json = strchr(respBuffer,'{');
    Serial.println(json);
    
   DynamicJsonBuffer jBuffer;
  JsonObject& root = jBuffer.parseObject(json);
  JsonObject& current = root["main"];
  String temp = current["temp"];
  //String weather = current["weather"];
  String msg = "Temp(C): "+ temp ;
  server.send(200, "text/plain",msg);
  //Serial.println(respBuffer);
  }
  client.stop();
//  char* json = strchr(respBuffer,'{');
//
//  DynamicJsonBuffer jBuffer;
//  JsonObject& root = jBuffer.parseObject(json);
//  //JsonObject& current = root["list"];
//  String temp = root["cod"];
// // String weather = current["weather"];
//  String msg = "Temp(C): "+ temp ;
//  server.send(200, "text/plain", respBuffer);
//  Serial.println(json);
}

//void getData(){
//  char* respBuffer="{'id':[{'a':22}],'main':'Mist','description':'mist','icon':'50d'}";
//  char* json = strchr(respBuffer,'{');
//
//  DynamicJsonBuffer jBuffer;
//  JsonObject& root = jBuffer.parseObject(json);
//  //JsonObject& current = root["list"];
//  String temp = root["id"];
//  //String weather = current["weather"];
//  String msg = "Temp(C): "+ temp ;
//  server.send(200, "text/plain",msg);
//  Serial.println(respBuffer);
//}





  
