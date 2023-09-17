#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <SoftwareSerial.h>

//WiFiClient client;
WiFiClientSecure client;
WiFiServer server(80);
const char* ssid = "project";
const char* password = "project1234";

const char* host = "ccb-c931.onrender.com";
const int httpsPort = 443;

//String site = "http://iotgecko.com/IOTHit.aspx?Id=rs2101396@gmail.com&Pass=6821&Data=";//edit iot id and pass here
String site = "https://ccb-c931.onrender.com/measurements/iot-data?Id=karabomarope07@gmail.com&Pass=1166&Data=";  //edit iot id and pass here
char data[100];
String iot_data = "";
//String final_data = site + "1" + "*" + "0" + "*" + "100" ;
//String final_data = site + iot_data ;
String final_data = "";

bool receive_data_bit = 0;

void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");

  Serial.println(WiFi.localIP());

  // Use WiFiClientSecure class to create TLS connection
  //WiFiClientSecure client;
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
}

void loop() {
  if (receive_data_bit == 1) {
    final_data = site + iot_data;
    send_data();
    receive_data_bit = 0;
  }
  receive_data();
}

void send_data() {
  if (WiFi.status() == WL_CONNECTED) {  //Check WiFi connection status
    HTTPClient http;                    //Declare an object of class HTTPClient
    //http.begin("http://StreetLight.iotgecko.com/IOTHit.aspx?AId=A1001&Data=1*LST105*1001*E1");  //Specify request destination
    //http.begin("http://iotgecko.com/IOTHit.aspx?Id=salve.dnyaneshvar1@gmail.com&Pass=2276&Data=0*0*0*0");  //Specify request destination
    //http.begin("http://iotgecko.com/IOTHit.aspx?Id=salve.dnyaneshvar1@gmail.com&Pass=2276&Data=0*0*0*0");  //Specify request destination
    http.begin(client, final_data);
    int httpCode = http.GET();  //Send the request

    if (httpCode > 0) {                   //Check the returning code
      String payload = http.getString();  //Get the request response payload
      payload.trim();
      String upper_limit = payload.substring(payload.indexOf("Label1") + 8, payload.indexOf("Label1") + 10);  //payload.indexOf("*") );
      String lower_limit = payload.substring(payload.indexOf("Label1") + 11, payload.indexOf("</spa"));
      Serial.println(payload);
    }
    http.end();  //Close connection
  } else {
    Serial.println("No wifi");
  }
  delay(100);
  iot_data = "";
}


void receive_data() {
  if (Serial.available() > 0) {
    while (Serial.available()) {
      delay(2);  //delay to allow byte to arrive in input buffer
      char c = Serial.read();
      iot_data += c;
    }
    Serial.println("iot_data = " + String(iot_data));
    receive_data_bit = 1;
  } else {
    //    Serial.println("NO data");
    receive_data_bit = 0;
  }
}
