#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

String iot_data[10];
String send_data;
SoftwareSerial esp8266(3, 2);
SoftwareSerial rfid1(4, 5);

String inputString1 = "";
String inputString = "";
String user_1 = "1000CF57BC34";
String user_2 = "1000CFF62B02";

const int data0 = 13;
const int data1 = 12;
const int data2 = 11;
const int data3 = 10;

bool room1_1 = 0;
bool room2_2 = 0;
bool room1_2 = 0;
bool room2_1 = 0;
bool emergency1 = 0;
bool emergency2 = 0;

long previous_time = 0;


LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);

void setup()
{
  Serial.begin(9600);
  Serial.println("POWERED");
  esp8266.begin(9600);
  lcd.begin(20, 4);
  rfid1.begin(9600);
  pinMode(data0, INPUT);
  pinMode(data1, INPUT);
  pinMode(data2, INPUT);
  pinMode(data3, INPUT);
  lcd.clear();
  lcd.print("   IOT Worker");

  Serial.print("   IOT Worker");
  lcd.setCursor(0, 1);
  lcd.print("Tracking System");

  Serial.print("Tracking System");
  delay(5000); // delay 2 secs

  lcd.clear();
  lcd.print("connecting To WIFI");

  Serial.print("connecting To WIFI");
  delay(2000);
  lcd.clear();
  lcd.print("Please Tap Your");

  Serial.print("Please Tap Your");
  lcd.setCursor(0, 1);
  lcd.print("RFID Card...");


  Serial.print("Card");
  delay(5000);
  previous_time = millis();
}

void loop()
{
  rfid1.listen();
  while (rfid1.available())
  {
    delay(2); // delay to allow byte to arrive in input buffer
    char c = rfid1.read();
    inputString1 += c;
  }

  if (inputString1.length() > 0)
  {
    if (inputString1 == user_1)
    {
      room1_1 = 1;
      room2_1 = 0;
    }
    else if (inputString1 == user_2)
    {
      room1_2 = 1;
      room2_2 = 0;
    }
  }
  if ((inputString.length() > 0) || (inputString1.length() > 0))
  {
    inputString = "";
    inputString1 = "";
  }

  scan();
  fill_iot_data_string();
  send_data = (iot_data[0]) + "*" + String(iot_data[1]) + "*" + String(iot_data[2]) + "*" + String(iot_data[3]) + "*" + String(iot_data[4]) + "*" + String(iot_data[5]);
  // Serial.println("send_data = " + String(send_data));
  display();

  if ((millis() - previous_time >= 30000) || emergency1 > 0 || emergency2 > 0)
  {
    send_parameters();
    previous_time = millis();
  }
}

void send_parameters()
{
  esp8266.listen();
  lcd.setCursor(14, 3);
  lcd.print("Upload");

  Serial.print("Upload");
  esp8266.print(send_data);
  Serial.print(send_data);
  delay(5000);
}

void SerialEvent()
{
  while (Serial.available())
  {
    delay(2); // delay to allow byte to arrive in input buffer
    char c = Serial.read();
    inputString += c;
  }
  if (inputString.length() > 0)
  {
    //////Serial.println(inputString);
    if (inputString == user_2)
    {
      room2_2 = 1;
      room1_2 = 0;
    }
    if (inputString == user_1)
    {
      room2_1 = 1;
      room1_1 = 0;
    }
  }
}

void scan()
{
  if ((digitalRead(data0) == LOW) && (digitalRead(data1) == LOW) && (digitalRead(data2) == HIGH) && (digitalRead(data3) == LOW))
  {
    lcd.setCursor(0, 2);
    lcd.print("Miner-1 EMERGENCY");

    Serial.print("Miner-1 EMERGENCY");
    // ////Serial.println("MINER1-EMERGENCY");
    emergency1 = 1;
    delay(2000);
  }
  else if ((digitalRead(data0) == HIGH) && (digitalRead(data1) == LOW) && (digitalRead(data2) == HIGH) && (digitalRead(data3) == LOW))
  {
    emergency1 = 0;
    display();
  }
  if ((digitalRead(data0) == LOW) && (digitalRead(data1) == LOW) && (digitalRead(data2) == LOW) && (digitalRead(data3) == HIGH))
  {
    lcd.setCursor(0, 3);
    lcd.print("Miner-2 EMERGENCY");

    Serial.print("Miner-2 EMERGENCY");
    //////Serial.println("MINER2-EMERGENCY");
    emergency2 = 1;
    delay(2000);
  }
  else if ((digitalRead(data0) == LOW) && (digitalRead(data1) == HIGH) && (digitalRead(data2) == LOW) && (digitalRead(data3) == HIGH))
  {
    emergency2 = 0;
    display();
  }
}

void display()
{
  lcd.clear();
  if (room1_2 == 1 && room1_1 == 1)
  {
    lcd.print("Mine1 =MINER1 MINER2");

    Serial.print("Mine1 =MINER1 MINER2");
  }
  else if (room1_1 == 1)
  {
    lcd.print("Mine1 =MINER1");

    Serial.print("Mine1 =MINER1");
  }
  else if (room1_2 == 1)
  {
    lcd.print("Mine1 =MINER2");

    Serial.print("Mine1 =MINER2");
  }

  else
  {
    lcd.print("Mine1 = ");

    Serial.print("Mine1 = ");
  }
  lcd.setCursor(0, 1);
  if (room2_2 == 1 && room2_1 == 1)
  {
    lcd.print("Mine2 =MINER1 MINER2");

    Serial.print("Mine2 =MINER1 MINER2");
  }
  else if (room2_1 == 1)
  {
    lcd.print("Mine2 =MINER1");

    Serial.print("Mine2 =MINER1");
  }
  else if (room2_2 == 1)
  {
    lcd.print("Mine2 =MINER2");

    Serial.print("Mine2 =MINER2");
  }
  else
  {
    lcd.print("Mine2 = ");

    Serial.print("Mine2 = ");
  }
  //  lcd.setCursor(0, 3);
  //  lcd.print("D=");// for debuging

  //Serial.print("");
  //  lcd.print(send_data);// for debuging

  //Serial.print("");
  delay(100);
}

void fill_iot_data_string()
{
  if (room1_1 == 1)
  {
    iot_data[0] = "1";
  }
  else
  {
    iot_data[0] = "0";
  }

  if (room1_2 == 1)
  {
    iot_data[1] = "1";
  }
  else
  {
    iot_data[1] = "0";
  }

  if (room2_1 == 1)
  {
    iot_data[2] = "1";
  }
  else
  {
    iot_data[2] = "0";
  }

  if (room2_2 == 1)
  {
    iot_data[3] = "1";
  }
  else
  {
    iot_data[3] = "0";
  }
  iot_data[4] = String(emergency1);
  iot_data[5] = String(emergency2);
}
