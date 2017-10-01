/*
* Written by Mosfex : www.mosfex.com
* www.fb.com/mosfex 
* plese refer to this when using the code
*/
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiClientSecure.h>

WiFiMulti wifiMulti;
LiquidCrystal_I2C lcd(0x3F, 16, 2); //maybe 27 2F 3F
const char* ssid = "mosfex.com";
const char* pass = "123456789";
const char* api_endpoint = "https://www.bx.in.th/api/";
const char* host_endpoint = "www.bx.in.th";

void parsePrint(String btc_str)
{
  int last_pos_st = btc_str.indexOf("last_price") + 12;
  int bits_pos_st = btc_str.indexOf("highbid") + 9;
  int asks_pos_st = btc_str.indexOf("highbid", bits_pos_st) + 9;
  String last = btc_str.substring(last_pos_st, btc_str.indexOf(",", last_pos_st));
  String bits = btc_str.substring(bits_pos_st, btc_str.indexOf("}", bits_pos_st));
  String asks = btc_str.substring(asks_pos_st, btc_str.indexOf("}", asks_pos_st));
  Serial.print("Last : "); Serial.println(last);
  Serial.print("sell : "); Serial.println(bits);
  Serial.print("buy : "); Serial.println(asks);
  lcd.clear();  
  lcd.setCursor(0,0);
  lcd.print("BTC : ");  
  lcd.print(last);
  lcd.setCursor(0,1);
  lcd.print(">");
  lcd.print(bits);
  lcd.print(">");
  lcd.print(asks);
}
void parseSecurePrint(WiFiClientSecure client)
{
  String line = client.readStringUntil('\n');
  parsePrint(line);
  client.stop();
}
WiFiClientSecure getSecureBtc()
{
  WiFiClientSecure client;
  //client.setCACert(ca);
  if (!client.connect(host_endpoint, 443))
    Serial.println("Connection failed!");
  else {
    Serial.println("Connected to server!");
    client.println(String("GET ") + api_endpoint + " HTTP/1.0");
    client.println(String("Host: ") + host_endpoint);
    client.println("Connection: close");
    client.println();
    delay(1000);
    //parse header
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        break;
      }
    }
    return client;
  }
}
void setup()
{
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  Serial.println();
  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t); Serial.flush(); delay(1000);
  }
  wifiMulti.addAP(ssid, pass);
  lcd.setCursor(0,0);
  lcd.print("WiFi Setup ...");
}
void loop()
{
  if ((wifiMulti.run() == WL_CONNECTED)) {
    parseSecurePrint(getSecureBtc());
  }
  delay(5 * 1000);
}
