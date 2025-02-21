#define THINGER_SERIAL_DEBUG
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C+
#include <Wire.h>
#endif
U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ 22, /* data=*/ 21, /* reset=*/ U8X8_PIN_NONE); //ESP32 oled
#include <ThingerESP32.h>
#define USERNAME "Thingerio_username"
#define DEVICE_ID "Device_name"
#define DEVICE_CREDENTIAL "Device_passkey"
#define SSID "network_name"
#define SSID_PASSWORD "network_passkey"
ThingerESP32 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);
int rawvalue;
const char* apiKey = "WEATHERAPI_KEY";
float temperature;
//int humid=2047;
//int wet=716;
void setup() {
  
// put your setup code here, to run once:
Serial.begin(115200);
u8g2.begin();
pinMode(33, INPUT);
pinMode(23,OUTPUT);
pinMode(21,OUTPUT);
thing.add_wifi(SSID,SSID_PASSWORD);
thing["WATERSUPPLY"]<<digitalPin(23);
thing["SOIL"] >> [](pson& out)
{
out=rawvalue;
};
 WiFi.begin(SSID,SSID_PASSWORD );
  Serial.println("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  Serial.println("Connected to WiFi");

}
void getWeather() {
  HTTPClient http;
  String url = "http://api.openweathermap.org/data/2.5/weather?q=chennai&appid=" + String(apiKey)+"&units=metric";
  
  Serial.print("Getting weather data from: ");
  Serial.println(url);

  http.begin(url);
  int httpCode = http.GET();

  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);

      // Parse JSON
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, payload);
      
      temperature = doc["main"]["temp"];
      Serial.print("Temperature: ");
      Serial.println(temperature);
    }
  }
  else {
    Serial.println("Error on HTTP request");
  }

  http.end();
}
void oled_display(const char* msg){
  //u8g2.clearBuffer(); // clear the internal memory
  u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
  u8g2.firstPage();
  do{
    u8g2.drawStr(5,10,"SMART IRRIGATION");
  u8g2.drawStr(5,30,msg);  // write something to the internal memory
  }
  while(u8g2.nextPage());
delay(50); 
}
void motorOn3()
{
  digitalWrite(23,HIGH);
  Serial.println("MOTOR ON");
  Serial.print("RAWVALUE:");
  Serial.println(rawvalue);
  oled_display("SOIL-DRY");
  Serial.println("SOIL-DRY");
  delay(10000);
}
void motorOn2()
{
  digitalWrite(23,HIGH);
  Serial.println("MOTOR ON");
  Serial.print("RAWVALUE:");
  Serial.println(rawvalue);
  oled_display("SOIL-DRY");
  Serial.println("SOIL-DRY");
  delay(8000);
}
void motorOn1()
{
  digitalWrite(23,HIGH);
  Serial.println("MOTOR ON");
  Serial.print("RAWVALUE:");
  Serial.println(rawvalue);
  oled_display("SOIL-DRY");
  Serial.println("SOIL-DRY");
  delay(5000);
}
void motorOff(){
  digitalWrite(23,LOW);
  Serial.println("MOTOR OFF");
  Serial.print("RAWVALUE");
  Serial.println(rawvalue);
  oled_display("SOIL WET");
  Serial.println("SOIL-WET");
}
void loop() 
{
// put your main code here, to run repeatedly:
thing.handle();
getWeather();
rawvalue=analogRead(33);
digitalWrite(23,LOW);

if(temperature>=25){
  if(rawvalue >=3000)
  {
    motorOn3();
}
   
 else{
   motorOff();
 }
}
else if(temperature>=15){
   if(rawvalue>=2000)
      {
       motorOn2();
     }
   else
    {
      motorOff();
    }
  }
else{
  if(rawvalue>=1000)
      {
        motorOn1();
     }
   else
    {
      motorOff();
     }
  }
}
