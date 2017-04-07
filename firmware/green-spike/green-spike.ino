#include <aJSON.h>
#include "SPI.h"
#include "WiFi.h"

#include "M2XStreamClient.h"

char ssid[] = "HackathonV4"; //  your network SSID (name)
char pass[] = "hackhack"; // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

#define LED RED_LED

char deviceId[] = "0904d16113289ea7b0f7d0dcbf8167ed"; // Feed you want to post to
char m2xKey[] = "5fa8ca77601afee8c23ea6f52fd0cb33"; // Your M2X access key
char streamName[] = "wetness"; // Stream you want to post to

bool pushed = false;
unsigned long lastReport = 0;
unsigned long lastPrint = 0;

const unsigned long printDelay = 500;
const unsigned long minReportDelay = 500;
const unsigned long maxReportDelay = 60000;

WiFiClient client;
M2XStreamClient m2xClient(&client, m2xKey);

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}

void setup() {

    lastReport = 0;
    Serial.begin(9600);
    pinMode(PUSH1, INPUT_PULLUP);
    pinMode(LED, OUTPUT);     
    
    // attempt to connect to Wifi network:x
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid); 
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    WiFi.begin(ssid, pass);
    while ( WiFi.status() != WL_CONNECTED) {
      // print dots while we wait to connect
      Serial.print(".");
      delay(250);
    }
  
    Serial.println("\nYou're connected to the network");
    Serial.println("Waiting for an ip address");
  
    while (WiFi.localIP() == INADDR_NONE) {
      // print dots while we wait for an ip addresss
      Serial.print(".");
      delay(250);
    }

    Serial.println("\nIP Address obtained");
  
    // you're connected now, so print out the status  
    printWifiStatus();
}

void loop() {
  int button = !digitalRead(PUSH1);
  int w = analogRead(A5);
  unsigned long now = millis();
  
  if (lastPrint + printDelay < now) {
    Serial.print("ADC: ");
    Serial.println(w);
    lastPrint = now;
  }
  
  if ((lastReport + maxReportDelay < now) || (button && (lastReport + minReportDelay < now))) {
    Serial.print("Reporting: ");
    int response = m2xClient.updateStreamValue(deviceId, streamName, w);
    Serial.println(response);    
    lastReport = now;
  }
  digitalWrite(LED, (now % 1000) < 621);
}
