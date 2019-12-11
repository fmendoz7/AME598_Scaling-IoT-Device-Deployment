/*
 * Programmer: Francis Mendoza
 * Function: IoT sensor firmware to transmit temp and humidity data 
 *           in concert w tertiary applications 
 */
//##################################################################
#include <TTGO.h>
TTGOClass *ttgo;

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
//Connect to D1 and D2 on nodemcu

#include <SimpleDHT.h>
int pinDHT11 = 21;
  //Connect DHT11 to D4 pin
SimpleDHT11 dht11(pinDHT11);
//-----------------------------------------------------------
//#include <ESP8266WiFi.h>
#include <WiFi.h>
  //Preprocessor command that enables Wifi connection

//CREATE WIFI NETWORK AND PASSWORD THAT LIMITS ACCESS TO WEBPAGE
const char* ssid = "Francis's iPhone";
const char* password =  "123456789";

  //REQUIRED: Wifi Hotspot details for in-class demonstration
  /* const char* ssid     = <name>;
     const char* password = <password>; */

//IP ADDRESS FOR AWS LIGHTSAIL INSTANCE OF SERVER
const char* host = "18.223.120.179";

//FINAL nA~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//USED ESP32 servo library instead
#include <ESP32Servo.h>
Servo needle;

int needleValT = 0;
int needleValH = 0;

int buttonHState = 0;
int buttonTState = 0;
int buttonFluxState = 0;
//-----------------------------------------------------------
void pressed()
{
    uint16_t color = random(0xFFFF);
    ttgo->eTFT->fillScreen(color);
    ttgo->eTFT->setTextColor(color, TFT_WHITE);
    ttgo->eTFT->drawString("MODE: HUMIDITY",  5, 100, 4);
}

void released()
{
    uint16_t color = random(0xFFFF);
    ttgo->eTFT->fillScreen(color);
    ttgo->eTFT->setTextColor(color, TFT_WHITE);
    ttgo->eTFT->drawString("MODE: TEMPERATURE",  5, 100, 4);
}

void setup()
{
    Serial.begin(115200);
    ttgo = TTGOClass::getWatch();
    ttgo->begin();
    ttgo->openBL();

    ttgo->eTFT->fillScreen(TFT_BLACK);
    ttgo->eTFT->setTextColor(TFT_WHITE, TFT_BLACK);
    ttgo->eTFT->setTextFont(4);
     ttgo->eTFT->drawString("MODE: TEMPERATURE",  5, 100, 4);

    ttgo->button->setPressedHandler(pressed);
    ttgo->button->setReleasedHandler(released);

  
  Serial.begin(115200);
  // initialize the LCD

  //FINAL nA~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //(!!!) WHAT PORT DO YOU USE FOR THE SERVO MOTOR!!!
    needle.attach(22);

    //pinMode(tLight, OUTPUT);
    //pinMode(hLight, OUTPUT);

    //(!!!) WHAT HAPPENS WHEN WE COMMENT OUT THIS LINE?
    //pinMode(D3, INPUT_PULLUP);
    //pinMode(D5, INPUT_PULLUP);
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  lcd.begin();
  lcd.backlight();

  print2Screen("LOADING...", "");

  //#####################################
  //Serial.println();
  //Serial.println();
  //Serial.print("Connecting to ");
  //Serial.println(ssid);

  /* Explicitly set the ESP8266 to be a WIFI-CLIENT, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
     
  //(???)What does this do?
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  //IF WIFI ISN'T CONNECTED
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }

  //(???)What does serial print do
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //#####################################
}
//-----------------------------------------------------------
void print2Screen(String s1, String s2)
{
  lcd.clear();
  lcd.print(s1);
  lcd.setCursor(0,1);
  lcd.print(s2);
}

float t = 0;
float h = 0;
//-----------------------------------------------------------
void loop()
{
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  //--------------------------------------------------------------------------------------
  //IF YOU WEREN'T ABLE TO READ ANYTHING
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) 
  {
    Serial.print("Read DHT11 failed, err="); Serial.println(err);delay(1000);
    return;
  }
  //--------------------------------------------------------------------------------------
  Serial.print("Sample OK: ");
  Serial.print((int)temperature); Serial.print(" *C, "); 
  Serial.print((int)humidity); Serial.println(" H"); 

  //CASTS AS FLOATS AND PERFORMS NECESSARY CONVERSION 
  t = (float)temperature * 1.8 + 32;
  h = (float)humidity;

  print2Screen("TEMP:" + String(t) + " *F", "HUM: " + String(humidity) + " %");

  //FINAL nA~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      needleValT = map(t, 0, 100, 30, 150);
      needleValH = map(h, 0, 100, 30, 150);
    
      //int buttonT = digitalRead(D3);
      //Serial.println(buttonT);
    
      //int buttonH = digitalRead(D5);
      //Serial.println(buttonH);

      int buttonFlux = digitalRead(36);
      
      if (pressed) {  
        buttonTState = 1;
        buttonHState = 0;
      }
    
      if (released) {  
        buttonHState = 1;
        buttonTState = 0;
      }

      //(!!!) Displays Temperature By Default
      if(buttonFluxState == 0){
          needle.write(needleValT);
        //digitalWrite(tLight, HIGH);
        //digitalWrite(hLight, LOW); 
      }

      //(!!!) Displays Humidity If Button Is 1
      else{
        needle.write(needleValH);
        //digitalWrite(hLight, HIGH);
        //digitalWrite(tLight, LOW);
      }
    
    Serial.println("h");
    Serial.println(buttonHState); 
    Serial.println("t");
    Serial.println(buttonTState);
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // DHT11 sampling rate is 1HZ.
  delay(1500);
 //################################################
 int value = 0;

//void loop() {
  delay(500);
  ++value;

  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) 
  {
    Serial.println("connection failed");
    return;
  }
  //--------------------------------------------------------------------------------------
  // We now create a URI for the request
  String path = "/setValue?t=" + String(t) + "&h=" + String(h);


  Serial.print("Requesting URL: ");
  Serial.println(path);
  //--------------------------------------------------------------------------------------
  // This will send the request to the server
  client.print(String("GET ") + path + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) 
  {
    if (millis() - timeout > 5000) 
    {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  int lineno = 1;
  String S1 = "";
  String S2 = "";

  //--------------------------------------------------------------------------------------
  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\r');
  }

  Serial.println();
  Serial.println("closing connection");
 //##########################################
  print2Screen("TEMP : " + String(t) + " *F", "HUM : " + String(h) + " %");

  //DHT11 sampling rate 1 HZ
  //delay(500);

    ttgo->button->loop();
}
