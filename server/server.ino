#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <SoftwareSerial.h>

WiFiClient client;
WiFiServer server(80);
const char *ssid = "project";
const char *password = "project1234";

const char *host = "http://34.135.233.62";
const int httpsPort = 80;

String site = "http://34.135.233.62/measurements/iot-data?Data=";
char data[100];
String iot_data = "";
String final_data = "";

bool receive_data_bit = 0;

void setup()
{
  Serial.begin(9600);
  // Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");

  Serial.println(WiFi.localIP());

  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort))
  {
    Serial.println("connection failed");
    return;
  }
}

void loop()
{
  if (receive_data_bit == 1)
  {
    final_data = site + iot_data;
    send_data();
    receive_data_bit = 0;
  }
  receive_data();
}

void send_data()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;

    http.begin(client, final_data);
    int httpCode = http.GET();

    if (httpCode > 0)
    {
      String payload = http.getString();
      payload.trim();
      String upper_limit = payload.substring(payload.indexOf("Label1") + 8, payload.indexOf("Label1") + 10);
      String lower_limit = payload.substring(payload.indexOf("Label1") + 11, payload.indexOf("</spa"));
      Serial.println(payload);
    }
    http.end();
  }
  else
  {
    Serial.println("No wifi");
  }
  delay(1000);
  iot_data = "";
}

void receive_data()
{
  if (Serial.available() > 0)
  {
    while (Serial.available())
    {
      delay(2);
      char c = Serial.read();
      iot_data += c;
    }
    Serial.println("iot_data = " + String(iot_data));
    receive_data_bit = 1;
  }
  else
  {
    receive_data_bit = 0;
  }
}
