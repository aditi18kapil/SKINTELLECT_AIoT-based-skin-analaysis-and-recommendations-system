/*
[Summary of the code]
Arduino code for STM32 Nucleo-144 board.
It uses W5300-TOE-Shield for internet connections (HTTP, MQTT).
For the W5300 connection, it uses the Serial3 port
For the OpenMV camera connection, it uses the Serial port.
It received a capture command through MQTT and send the request to OpenMV camera via the Serial port.
It receives a captured image via the Serial port and send it to the HTTP server with POST method.
Once it receives a HTTPP response, it publish the response to the MQTT broker.
*/

#include <Ethernet.h>
#include <Wire.h>
// MQTT [[ 
#include <PubSubClient.h>
// ]]
#include <ArduinoJson.h>
#include "HardwareSerial.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


int trigPin = 14;    // TRIG pin
int echoPin = 15;  
float duration_us, distance_cm;
const char* device_unique_id = "ArdCam1_" __DATE__ "_" __TIME__;

#define PIN0_SDA 0
#define PIN0_SCL 1

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const unsigned char myImage[] = {
// '64-01', 128x64px
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x31, 0x87, 0x87, 0xfe, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x31, 0xc3, 0x87, 0xfe, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf3, 0xff, 0x31, 0xe0, 0x06, 0x66, 0x7f, 0xef, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf3, 0xff, 0x31, 0xe0, 0x02, 0xfe, 0x7f, 0xef, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf3, 0x03, 0x31, 0x9f, 0xe1, 0xfe, 0x60, 0x6f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf3, 0x03, 0x31, 0xcf, 0xe1, 0xfe, 0x60, 0x6f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf3, 0x03, 0x3d, 0xe0, 0x66, 0x66, 0x60, 0x6f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf3, 0x03, 0x3d, 0xe0, 0x23, 0xfe, 0x60, 0x6f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf3, 0x03, 0x3f, 0x81, 0x81, 0x9e, 0x60, 0x6f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf3, 0xff, 0x37, 0x09, 0x81, 0x8e, 0x7f, 0xef, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf3, 0xff, 0x36, 0x19, 0x99, 0x86, 0x7f, 0xef, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x36, 0x22, 0x37, 0x66, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x36, 0x66, 0x66, 0x66, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf1, 0xfe, 0x18, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf1, 0xfe, 0x18, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0x71, 0xdf, 0x9f, 0x98, 0x1e, 0x0f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0x31, 0x9f, 0x9f, 0x98, 0x1e, 0x07, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xce, 0x66, 0x78, 0x61, 0xe1, 0xe7, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xce, 0x66, 0x78, 0x61, 0xe1, 0xe7, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf0, 0x03, 0x06, 0x79, 0xfe, 0x1f, 0x81, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf0, 0x03, 0x06, 0x79, 0xfe, 0x1f, 0x81, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf0, 0x3c, 0xc6, 0x78, 0x00, 0x06, 0x60, 0x1f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf0, 0x3c, 0xc6, 0x7c, 0x00, 0x06, 0x60, 0x1f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x38, 0xfe, 0x01, 0x86, 0x19, 0x8f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x3d, 0xfe, 0x01, 0x86, 0x19, 0x87, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf8, 0xff, 0xdf, 0xfe, 0x23, 0xc0, 0x79, 0x9f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xcf, 0xfe, 0x27, 0xe0, 0xfb, 0x9f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xfc, 0xc3, 0x01, 0x99, 0x87, 0xf9, 0xe7, 0xe7, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xfc, 0xc3, 0x01, 0x99, 0x87, 0xf9, 0xe7, 0xe7, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf0, 0x3c, 0xf9, 0x80, 0x0f, 0xc1, 0xe3, 0x87, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf0, 0x3c, 0xfd, 0x80, 0x1f, 0x81, 0xe1, 0x87, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf3, 0x30, 0x76, 0x79, 0x87, 0x79, 0xe0, 0x67, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf3, 0x30, 0x37, 0x79, 0x82, 0x79, 0xe0, 0x67, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf3, 0x0c, 0xf7, 0x80, 0x01, 0xf9, 0x9e, 0x07, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf3, 0x0c, 0xf3, 0x80, 0x01, 0xf9, 0x9e, 0x0f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf3, 0xc0, 0x30, 0x19, 0xe1, 0xff, 0x9f, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf3, 0xc0, 0x70, 0x19, 0xe1, 0xff, 0x9f, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf3, 0x30, 0xfc, 0x07, 0xe1, 0xe6, 0x18, 0x1f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf3, 0x30, 0xfc, 0x07, 0xe1, 0xe6, 0x18, 0x1f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf3, 0x0c, 0x0e, 0x06, 0x66, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xfb, 0x0c, 0x1f, 0x02, 0x76, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf7, 0x81, 0xf8, 0x67, 0xe1, 0x87, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf7, 0x81, 0xf8, 0x67, 0xe3, 0x8f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x3c, 0x06, 0x7e, 0x66, 0x67, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x3c, 0x06, 0x7f, 0xe7, 0xe7, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf3, 0xff, 0x3e, 0x66, 0x7f, 0xe7, 0xe1, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf3, 0xff, 0x3f, 0x66, 0x3f, 0xe3, 0xc1, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf3, 0x03, 0x37, 0x86, 0x1f, 0xe0, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf3, 0x03, 0x33, 0xc2, 0x0f, 0xe0, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf3, 0x03, 0x31, 0xe1, 0x81, 0xe6, 0x7f, 0x9f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf3, 0x03, 0x30, 0x0f, 0xf9, 0xfe, 0x67, 0x9f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf3, 0x03, 0x30, 0x1f, 0xf9, 0xfe, 0x67, 0x9f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf3, 0xff, 0x3f, 0x9e, 0x7e, 0xf1, 0xe3, 0x9f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf3, 0xff, 0x3f, 0x9e, 0x7e, 0x61, 0xe1, 0x9f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x3c, 0x07, 0xf1, 0xe1, 0xe7, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x3c, 0x07, 0xe1, 0xe1, 0xe7, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

//#define SERIAL_OUTPUT
#ifdef SERIAL_OUTPUT
#define PRINT(x)   Serial.print(x)
#define PRINTLN(x) Serial.println(x)
#else
#define PRINT(x)   
#define PRINTLN(x) 
#endif

#define SYNC  0x96

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0x77, 0xF7
};
IPAddress ip(192, 168, 0, 77);
//IPAddress ip(10, 5, 15, 109);

// Enter the IP address of the server you're connecting to:
IPAddress server(192, 168, 0, 107);
//IPAddress server(10, 21, 70, 16);
//IPAddress server(44, 195, 202, 69);
IPAddress myDns(192, 168, 0, 1);
uint16_t port = 5000;

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 23 is default for telnet;
// if you're using Processing's ChatServer, use port 10002):
#define max_transfer 1024
#define max_buffer  (100 * max_transfer)
byte img_buf[max_buffer];
EthernetClient client;

//[ MQTT
//IPAddress mqtt_server(10, 21, 70, 16);
IPAddress mqtt_server(44, 195, 202, 69);
EthernetClient mqttClient;
PubSubClient mqtt_client(mqttClient);
bool capture_requested = 0;
//]

//===================================================================================================
// HTTP POST 

int count = 0;
char c = 0;

void client_write_large(byte *bptr, size_t len) {
  size_t sent = 0;
  for (; sent + max_transfer < len; ) {
    client.write(bptr, max_transfer);
    sent += max_transfer;
    bptr += max_transfer;
    //PRINTLN(max_transfer);
  }
  client.write(bptr, len - sent);

  PRINT("Sent: "); PRINTLN(len);
}

void httpPostForm(byte *imageData, uint32_t imageSize) {
  String textData = "OpenMVCam1"; // Replace with your text data

  // Prepare the POST request body
  String requestBody = "";
  // Append the text data
  requestBody += "--ArduinoBoundary_OpenMVCam1\r\n";
  requestBody += "Content-Disposition: form-data; name=\"text\"\r\n\r\n";
  requestBody += textData;
  // Append the image data
  requestBody += "\r\n--ArduinoBoundary_OpenMVCam1\r\n";
  requestBody += "Content-Disposition: form-data; name=\"image\"; filename=\"image.jpg\"\r\n\r\n";

  //requestBody.append(imageData, imageSize);

  // Append the closing boundary
  String requestBodyEnd = "";
  requestBodyEnd += "\r\n--ArduinoBoundary_OpenMVCam1--\r\n";

  // Prepare the POST request headers
  String requestHeaders = "POST /upload HTTP/1.1\r\n";
  requestHeaders += "Host: 192.168.0.107:5000\r\n";
  requestHeaders += "Content-Type: multipart/form-data; boundary=ArduinoBoundary_OpenMVCam1\r\n";
  requestHeaders += "Connection: close\r\n";
  requestHeaders += "Content-Length: " + String(requestBody.length()+imageSize+requestBodyEnd.length()) + "\r\n\r\n";

  // Send the POST request headers
  client.print(requestHeaders);

  // Send the POST request body
  client.print(requestBody);
  client_write_large(imageData, imageSize);
  client.print(requestBodyEnd);
  client.flush();

  delay(100);
}

void http_postData(byte *buf, uint32_t length) {
  count = 0;

  // if the server's disconnected, reconnect the client:
  while (!client.connected()) {
    PRINTLN();
    PRINTLN("disconnected. Reconnecting...");
    if (client.connect(server, port)) {
      PRINTLN("connected");
      break;
    } else {
      // if you didn't get a connection to the server:
      if (++count > 5) {  // Retry 5 times.
        PRINTLN("HTTP Post failed. Give up.");
        return;
      }      
      PRINTLN("connection failed");
      delay(1000);
    }
  }
  delay(200);
  
  httpPostForm(buf, length);

  int len = client.available();

  String msg = "HTTP Response: ";
  msg += len;
  //mqtt_client.publish("W5300-MQTT", msg.c_str());

  if (len > 0) {
    byte buffer[500];
    if (len > 500) len = 500;
    int recvlen = 0;
        
    recvlen = client.read(buffer+recvlen, len);
    if (recvlen < len) {
      delay(10);
      recvlen += client.read(buffer+recvlen, len-recvlen);
    }

    String msg = "HTTP Received: ";
    msg += recvlen;
    //mqtt_client.publish("W5300-MQTT", msg.c_str());

    //Serial.write(buffer, len); // show in the serial monitor (slows some boards)
    //PRINTLN("");
    //byteCount = byteCount + len;

    byte prev_char = 0;
    String response = "";
    int index = 0;
    for (index=0; index<len; ++index) {
      if (buffer[index] == '\n') {
        if (prev_char == '\n')
          break;
      } else if (buffer[index] == '\r')
        continue;
      prev_char = buffer[index];
    }

    for (; index<len; ++index)
      response += (char)buffer[index];
    response.trim();

    PRINTLN("Contents: " + response);
    mqtt_client.publish("W5300-MQTT", response.c_str());
  }

  client.stop();
}

//===================================================================================================
// MQTT [[ 

void callback(char* topic, byte* payload, unsigned int length) {
  PRINT(">>>>>>>>>>> Message arrived [");
  PRINT(topic);
  PRINT("] ");
  String cmd = "";
  for (int i=0;i<length;i++) {
    cmd += (char)payload[i];
    PRINT((char)payload[i]);
  }
  PRINTLN();

  if (cmd == "cmd:capture") {
    capture_requested = 1;
    PRINTLN("Capture requested!!!");
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!mqtt_client.connected()) {
    PRINT("MQTT: Attempting MQTT connection...");
    // Attempt to connect
    if (mqtt_client.connect(device_unique_id)) {
      PRINTLN("MQTT: connected");
      mqtt_client.publish("W5300-MQTT", "Ready");
      // ... and resubscribe
      mqtt_client.subscribe("MQTT-W5300");
    } else {
      PRINT("MQTT: failed, rc=");
      PRINT(mqtt_client.state());
      PRINTLN(" MQTT: try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(2000);
    }
  }
}
// ]]

//===================================================================================================

void setup() {

  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Wire.setSDA(PIN0_SDA);
  Wire.setSCL(PIN0_SCL);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    for (;;) ;
  }
  display.display();
  delay(2000);
  display.clearDisplay(); 

    // Open serial communications and wait for port to open:
  Serial3.setRx(PC11);
  Serial3.setTx(PC10);  
  delay(50);
  
  // Open serial communications and wait for port to open:
#ifdef SERIAL_OUTPUT
  Serial.begin(9600);
#else
  Serial.setRx(0);
  Serial.setTx(1);  
  //Serial.begin(1000000);
  Serial.begin(500000);
  //Serial.begin(38400);
  //Serial.begin(19200);
  delay(50);
#endif

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
   // start the Ethernet connection:
  PRINTLN("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    PRINTLN("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      PRINTLN("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      PRINTLN("Ethernet cable is not connected.");
    }
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
  } else {
    PRINT("  DHCP assigned IP ");
    PRINTLN(Ethernet.localIP());
  }
  
  // give the Ethernet shield a second to initialize:
  delay(1000);

  // MQTT [[ 
  mqtt_client.setServer(mqtt_server, 1883);
  mqtt_client.setCallback(callback);
  // ]]

  // give the Ethernet shield a second to initialize:
  delay(500);

}

unsigned long prevmillis = 0;

void loop() {

  // generate 10-microsecond pulse to TRIG pin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // measure duration of pulse from ECHO pin
  duration_us = pulseIn(echoPin, HIGH);

  // calculate the distance
  distance_cm = duration_us / 29 / 2;

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 5);

  // Check if someone is within 100 cm range
  if (distance_cm < 100 && distance_cm > 0) { // Check if distance is within the valid range (0 to 100 cm)
    // Display the image on the OLED screen
    display.drawBitmap(0, 0, myImage, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
    display.display();
  } else {
    display.print(F("Try Skintellect!!"));
    display.display();
  }

  // delay(1500);

  //*// MQTT [[ 
  if (!mqtt_client.connected()) {
    reconnect();
  }
  mqtt_client.loop();
  // ]]
  //*/

  //if (millis() - prevmillis > 5000)
  if (capture_requested) {
    capture_requested = 0;

    // Flush serial buffer.
    while (Serial.available())
      Serial.read();

    // Send SYNC code
    Serial.write(SYNC);

    uint32_t length = serial_read_length();
    if (length > 0) {
      String response = "Length: ";
      response += length;
      mqtt_client.publish("W5300-MQTT", response.c_str());
      
      uint32_t received = serial_read_data(img_buf, length);
      if (received != length) {
        // Time-out error!!!!
        String response = "Time-out: ";  // + received;
        response += received;
        mqtt_client.publish("W5300-MQTT", response.c_str());
      } else {
        // Send it to the server
        String response = "Image received: ";  // + length;
        //mqtt_client.publish("W5300-MQTT", response.c_str());
        http_postData(img_buf, length);
      }
    } else {
        mqtt_client.publish("W5300-MQTT", "No data");
    }
  }
}

uint32_t serial_read_length() {
  uint32_t length = 0;
  byte recv[4];
  int index = 0;

  prevmillis = millis();
  //Loop with 1sec timeout.
  while (millis()-prevmillis < 1000) {
    if (Serial.available()) {
      // Read the most recent byte
      recv[index++] = Serial.read();

      if (index >= 4) {
        // Big endian
        length = (recv[0] << 24) | (recv[1] << 16) | (recv[2] << 8) | recv[3];
        break;
      }
    }
  }

  return length;
}

uint32_t serial_read_data(byte *buf, uint32_t length) {
  int index = 0;

  uint32_t maxsize = 1024;
  uint32_t recvlen = 0;
  uint32_t remain = length;

  prevmillis = millis();
  while (remain > 0 && (millis()-prevmillis < 2000)) {
    if (remain > maxsize) {
      recvlen = serial_read_data0(buf, maxsize);
    } else {
      recvlen = serial_read_data0(buf, remain);
    }
    if (recvlen == -1) {
      return length - remain;
    }
    buf += recvlen;
    remain -= recvlen;

    /*
    String response = "RecvLen: ";
    response += recvlen;
    mqtt_client.publish("W5300-MQTT", response.c_str());
    */
  }

  return length - remain;
}

uint32_t serial_read_data0(byte *buf, uint32_t length) {
  int index = 0;

  prevmillis = millis();
  //Loop with 2sec timeout.
  while (millis()-prevmillis < 1000) {
    if (Serial.available()) {
      // Read the most recent byte
      buf[index++] = Serial.read();

      if (index >= length)
        return index;
    }
  }

  return -1;
}