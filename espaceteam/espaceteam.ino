/*
  Spacetime using ESP32

  Modified by Tiffany Tseng for esp32 Arduino Board Definition 3.0+ 
  Originally created by Mark Santolucito for Barnard COMS 3930
  Based on DroneBot Workshop 2022 ESP-NOW Multi Unit Demo
*/

// Include Libraries
#include <WiFi.h>
#include <esp_now.h>
#include <TFT_eSPI.h>  // Graphics and font library for ST7735 driver chip
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

String cmd1 = "";
String cmd2 = "";
volatile bool scheduleCmd1Send = false;
volatile bool scheduleCmd2Send = false;

String cmdRecvd = "";
const String waitingCmd = "Wait for cmds";
bool redrawCmdRecvd = false;

// for drawing progress bars
int progress = 0;
bool redrawProgress = true;
int lastRedrawTime = 0;

bool showSuccessBackground = false;
unsigned long successDisplayTime = 0;
const unsigned long successDuration = 500;

//we could also use xSemaphoreGiveFromISR and its associated fxns, but this is fine
volatile bool scheduleCmdAsk = true;
hw_timer_t *askRequestTimer = NULL;
volatile bool askExpired = false;
hw_timer_t *askExpireTimer = NULL;
int expireLength = 25;

#define ARRAY_SIZE 10
const uint8_t squareImg[32] = {
  0xFF, 0xFF, // Row 1
  0xFF, 0xFF, // Row 2
  0xFF, 0xFF, // Row 3
  0xFF, 0xFF, // Row 4
  0xFF, 0xFF, // Row 5
  0xFF, 0xFF, // Row 6
  0xFF, 0xFF, // Row 7
  0xFF, 0xFF, // Row 8
  0xFF, 0xFF, // Row 9
  0xFF, 0xFF, // Row 10
  0xFF, 0xFF, // Row 11
  0xFF, 0xFF, // Row 12
  0xFF, 0xFF, // Row 13
  0xFF, 0xFF, // Row 14
  0xFF, 0xFF, // Row 15
  0xFF, 0xFF  // Row 16
};

const uint8_t circleImg[32] = {
  0x00, 0x00,
  0x03, 0xC0,
  0x0F, 0xF0,
  0x1F, 0xF8,
  0x3F, 0xFC,
  0x3F, 0xFC,
  0x7F, 0xFE,
  0x7F, 0xFE,
  0x7F, 0xFE,
  0x7F, 0xFE,
  0x3F, 0xFC,
  0x3F, 0xFC,
  0x1F, 0xF8,
  0x0F, 0xF0,
  0x03, 0xC0,
  0x00, 0x00
};

const uint8_t triangleImg[32] = {
  0x00, 0x00,
  0x00, 0x80,
  0x01, 0xC0,
  0x03, 0xE0,
  0x07, 0xF0,
  0x0F, 0xF8,
  0x1F, 0xFC,
  0x3F, 0xFE,
  0x7F, 0xFF,
  0x7F, 0xFF,
  0x7F, 0xFF,
  0x7F, 0xFF,
  0x7F, 0xFF,
  0x7F, 0xFF,
  0x7F, 0xFF,
  0x00, 0x00
};

const uint8_t starImg[32] = {
  0x00, 0x00,
  0x01, 0x80,
  0x09, 0x90,
  0x0F, 0xF0,
  0x3F, 0xFC,
  0x1F, 0xF8,
  0x7F, 0xFE,
  0x7F, 0xFE,
  0x1F, 0xF8,
  0x3F, 0xFC,
  0x0F, 0xF0,
  0x09, 0x90,
  0x01, 0x80,
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00
};

const uint8_t diamondImg[32] = {
  0x00, 0x00,
  0x00, 0x40,
  0x00, 0xE0,
  0x01, 0xF0,
  0x03, 0xF8,
  0x07, 0xFC,
  0x0F, 0xFE,
  0x1F, 0xFF,
  0x0F, 0xFE,
  0x07, 0xFC,
  0x03, 0xF8,
  0x01, 0xF0,
  0x00, 0xE0,
  0x00, 0x40,
  0x00, 0x00,
  0x00, 0x00
};

const uint8_t hexagonImg[32] = {
  0x00, 0x00,
  0x03, 0xC0,
  0x0F, 0xF0,
  0x1F, 0xF8,
  0x3F, 0xFC,
  0x7F, 0xFE,
  0x7F, 0xFE,
  0x7F, 0xFE,
  0x7F, 0xFE,
  0x7F, 0xFE,
  0x3F, 0xFC,
  0x1F, 0xF8,
  0x0F, 0xF0,
  0x03, 0xC0,
  0x00, 0x00,
  0x00, 0x00
};

const uint8_t octagonImg[32] = {
  0x00, 0x00,
  0x03, 0xC0,
  0x0F, 0xF0,
  0x1F, 0xF8,
  0x3E, 0x7C,
  0x7C, 0x3E,
  0x7C, 0x3E,
  0x7F, 0xFE,
  0x7F, 0xFE,
  0x7C, 0x3E,
  0x7C, 0x3E,
  0x3E, 0x7C,
  0x1F, 0xF8,
  0x0F, 0xF0,
  0x03, 0xC0,
  0x00, 0x00
};

const uint8_t crossImg[32] = {
  0x00, 0x00,
  0x00, 0x00,
  0x18, 0x18,
  0x3C, 0x3C,
  0x3C, 0x3C,
  0x1F, 0xF8,
  0x07, 0xE0,
  0x07, 0xE0,
  0x07, 0xE0,
  0x1F, 0xF8,
  0x3C, 0x3C,
  0x3C, 0x3C,
  0x18, 0x18,
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00
};

const uint8_t arrowImg[32] = {
  0x00, 0x00,
  0x00, 0x80,
  0x01, 0xC0,
  0x03, 0xE0,
  0x07, 0xF0,
  0x0F, 0xF8,
  0x1F, 0xFC,
  0x7F, 0xFF,
  0x1F, 0xFC,
  0x0F, 0xF8,
  0x07, 0xF0,
  0x03, 0xE0,
  0x01, 0xC0,
  0x00, 0x80,
  0x00, 0x00,
  0x00, 0x00
};

const uint8_t heartImg[32] = {
  0x00, 0x00,
  0x06, 0x60,
  0x0F, 0xF0,
  0x1F, 0xF8,
  0x3F, 0xFC,
  0x7F, 0xFE,
  0x7F, 0xFE,
  0x7F, 0xFE,
  0x3F, 0xFC,
  0x1F, 0xF8,
  0x0F, 0xF0,
  0x07, 0xE0,
  0x03, 0xC0,
  0x01, 0x80,
  0x00, 0x00,
  0x00, 0x00
};
const uint8_t *commandImages[ARRAY_SIZE] = { squareImg, circleImg, triangleImg, starImg, diamondImg, hexagonImg, octagonImg, crossImg, arrowImg, heartImg };
const String commandNounsFirst[ARRAY_SIZE] = { "oat", "fry", "bread", "rice", "bean", "cheese", "pie", "corn", "soup", "fish" };
const String commandNounsSecond[ARRAY_SIZE] = { "pup", "cat", "frog", "bear", "fox", "duck", "pig", "shark", "mouse", "wolf" };
int lineHeight = 30;

// Define LED and pushbutton pins
#define BUTTON_LEFT 0
#define BUTTON_RIGHT 35


void formatMacAddress(const uint8_t *macAddr, char *buffer, int maxLength)
// Formats MAC Address
{
  snprintf(buffer, maxLength, "%02x:%02x:%02x:%02x:%02x:%02x", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
}


void receiveCallback(const esp_now_recv_info_t *macAddr, const uint8_t *data, int dataLen)
/* Called when data is received
   You can receive 3 types of messages
   1) a "ASK" message, which indicates that your device should display the cmd if the device is free
   2) a "DONE" message, which indicates the current ASK? cmd has been executed
   3) a "PROGRESS" message, indicating a change in the progress of the spaceship
   
   Messages are formatted as follows:
   [A/D]: cmd
   For example, an ASK message for "Twist the wutangs":
   A: Twist the wutangs
   For example, a DONE message for "Engage the devnobs":
   D: Engage the devnobs
   For example, a PROGESS message for 75% progress
   P: 75
*/

{
  // Only allow a maximum of 250 characters in the message + a null terminating byte
  char buffer[ESP_NOW_MAX_DATA_LEN + 1];
  int msgLen = min(ESP_NOW_MAX_DATA_LEN, dataLen);
  strncpy(buffer, (const char *)data, msgLen);

  // Make sure we are null terminated
  buffer[msgLen] = 0;
  String recvd = String(buffer);
  Serial.println(recvd);
  // Format the MAC address
  char macStr[18];
  // formatMacAddress(macAddr, macStr, 18);

  // Send Debug log message to the serial port
  Serial.printf("Received message from: %s \n%s\n", macStr, buffer);
  if (recvd[0] == 'A' && cmdRecvd == waitingCmd && random(100) < 30)  //only take an ask if you don't have an ask already and only take it XX% of the time
  {
    recvd.remove(0, 3);
    cmdRecvd = recvd;
    redrawCmdRecvd = true;
    timerStart(askExpireTimer);  //once you get an ask, a timer starts
  } else if (recvd[0] == 'D' && recvd.substring(3) == cmdRecvd) {
    timerWrite(askExpireTimer, 0);
    timerStop(askExpireTimer);
    cmdRecvd = waitingCmd;
    progress = progress + 1;
    broadcast("P: " + String(progress));
    redrawCmdRecvd = true;
    showSuccessBackground = true;
    successDisplayTime = millis();
  } else if (recvd[0] == 'P') {
    recvd.remove(0, 3);
    progress = recvd.toInt();
    redrawProgress = true;
  }
}

void sentCallback(const uint8_t *macAddr, esp_now_send_status_t status)
// Called when data is sent
{
  char macStr[18];
  formatMacAddress(macAddr, macStr, 18);
  Serial.print("Last Packet Sent to: ");
  Serial.println(macStr);
  Serial.print("Last Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void broadcast(const String &message)
// Emulates a broadcast
{
  // Broadcast a message to every device in range
  uint8_t broadcastAddress[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
  esp_now_peer_info_t peerInfo = {};
  memcpy(&peerInfo.peer_addr, broadcastAddress, 6);
  if (!esp_now_is_peer_exist(broadcastAddress)) {
    esp_now_add_peer(&peerInfo);
  }
  // Send message
  esp_err_t result = esp_now_send(broadcastAddress, (const uint8_t *)message.c_str(), message.length());
}

void IRAM_ATTR sendCmd1() {
  scheduleCmd1Send = true;
}

void IRAM_ATTR sendCmd2() {
  scheduleCmd2Send = true;
}

void IRAM_ATTR onAskReqTimer() {
  scheduleCmdAsk = true;
}

void IRAM_ATTR onAskExpireTimer() {
  askExpired = true;
  timerStop(askExpireTimer);
  timerWrite(askExpireTimer, 0);
}

void espnowSetup() {
  // Set ESP32 in STA mode to begin with
  delay(500);
  WiFi.mode(WIFI_STA);
  Serial.println("ESP-NOW Broadcast Demo");

  // Print MAC address
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  // Disconnect from WiFi
  WiFi.disconnect();

  // Initialize ESP-NOW
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESP-NOW Init Success");
    esp_now_register_recv_cb(receiveCallback);
    esp_now_register_send_cb(sentCallback);
  } else {
    Serial.println("ESP-NOW Init Failed");
    delay(3000);
    ESP.restart();
  }
}

void buttonSetup() {
  pinMode(BUTTON_LEFT, INPUT);
  pinMode(BUTTON_RIGHT, INPUT);

  attachInterrupt(digitalPinToInterrupt(BUTTON_LEFT), sendCmd1, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_RIGHT), sendCmd2, FALLING);
}

void textSetup() {
  tft.init();
  tft.setRotation(0);

  tft.setTextSize(2);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_SKYBLUE, TFT_BLACK);
  drawControls();

  cmdRecvd = waitingCmd;
  redrawCmdRecvd = true;
}

void timerSetup() {
  // https://espressif-docs.readthedocs-hosted.com/projects/arduino-esp32/en/latest/api/timer.html
  askRequestTimer = timerBegin(1000000); // 1MHz
  timerAttachInterrupt(askRequestTimer, &onAskReqTimer);
  timerAlarm(askRequestTimer, 5 * 1000000, true, 0);  //send out an ask every 5 secs

  askExpireTimer = timerBegin(80000000);
  timerAttachInterrupt(askExpireTimer, &onAskExpireTimer);
  timerAlarm(askExpireTimer, expireLength * 1000000, true, 0);
  timerStop(askExpireTimer);
}
void setup() {
  Serial.begin(115200);

  textSetup();
  buttonSetup();
  espnowSetup();
  timerSetup();
}

String genCommand() {
  String noun1 = commandNounsFirst[random(ARRAY_SIZE)];
  String noun2 = commandNounsSecond[random(ARRAY_SIZE)];
  return noun1 + noun2;
}

void drawShape(const uint8_t *shapeImg, int x, int y, int height, uint16_t color) {
  tft.drawBitmap(x, y, shapeImg, 16, 16, color); // Draws the shape with the given color
}

const int textOffset = 25;     // Offset for the text after "B1:" or "B2:"

void drawControls() {
  cmd1 = genCommand();
  cmd2 = genCommand();

   // Extract parts of the command for alignment
  String noun1 = cmd1.substring(cmd1.indexOf(' ') + 1);
  String noun2 = cmd2.substring(cmd2.indexOf(' ') + 1);
  
  tft.drawString("1: ", 0, 90, 1.5);
  drawShape(commandImages[random(ARRAY_SIZE)], 5, 120, 50, color1);
  tft.drawString(noun1, textOffset, 120, 1.5);
  tft.drawString("2: ", 0, 180, 1.5);
  drawShape(commandImages[random(ARRAY_SIZE)], 5, 200, 50, color2);
  tft.drawString(noun2, textOffset, 200, 1.5);
}

// Recolor text for pressed button indication
void recolorTextLeft(uint16_t color) {
  tft.setTextColor(color);
  // Draw the command shape and noun for left button
  drawShape(commandImages[random(ARRAY_SIZE)], 5, 120, 50, color);
  String noun1 = cmd1.substring(cmd1.indexOf(' ') + 1); // Extract noun from cmd1
  tft.drawString(noun1, textOffset, 120, 1.5);
}

void recolorTextRight(uint16_t color) {
  tft.setTextColor(color);
  // Draw the command shape and noun for right button
  drawShape(commandImages[random(ARRAY_SIZE)], 5, 200, 50, color);
  String noun2 = cmd2.substring(cmd2.indexOf(' ') + 1); // Extract noun from cmd2
  tft.drawString(noun2, textOffset, 200, 1.5);
}

void loop() {

  if (scheduleCmd1Send) {
    broadcast("D: " + cmd1);
    scheduleCmd1Send = false;
  }
  if (scheduleCmd2Send) {
    broadcast("D: " + cmd2);
    scheduleCmd2Send = false;
  }
  if (scheduleCmdAsk) {
    String cmdAsk = random(2) ? cmd1 : cmd2;
    broadcast("A: " + cmdAsk);
    scheduleCmdAsk = false;
  }
  if (askExpired) {
    progress = max(0, progress - 1);
    broadcast(String(progress));
    //tft.fillRect(0, 0, 135, 90, TFT_RED);
    cmdRecvd = waitingCmd;
    redrawCmdRecvd = true;
    askExpired = false;
  }
  if (showSuccessBackground) {
    // Display green background
    tft.fillScreen(TFT_GREEN);

    // Check if the display duration has passed, then reset the screen
    if (millis() - successDisplayTime > successDuration) {
      showSuccessBackground = false;
      redrawCmdRecvd = true;
      redrawProgress = true;  // Set flag to redraw the progress bar as well
      tft.fillScreen(TFT_BLACK); 
      drawControls();         // Redraw button commands after green flash
    }
  }
  if ((millis() - lastRedrawTime) > 50) {
    tft.fillRect(15, lineHeight * 2 + 14, 100, 6, TFT_GREEN);
    tft.fillRect(16, lineHeight * 2 + 14 + 1, (((expireLength * 1000000.0) - timerRead(askExpireTimer)) / (expireLength * 1000000.0)) * 98, 4, TFT_RED);
    if (digitalRead(BUTTON_LEFT) == 0) {
      recolorTextLeft(TFT_GREEN);
    } else if (digitalRead(BUTTON_RIGHT) == 0) {
      recolorTextRight(TFT_GREEN);
    } else {
      recolorTextLeft(TFT_SKYBLUE);
      recolorTextRight(TFT_SKYBLUE);
    }
    lastRedrawTime = millis();
  }

  if (redrawCmdRecvd || redrawProgress) {
    tft.fillRect(0, 0, 135, 90, TFT_BLACK);
    tft.drawString(cmdRecvd.substring(0, cmdRecvd.indexOf(' ')), 0, 0, 2);
    tft.drawString(cmdRecvd.substring(cmdRecvd.indexOf(' ') + 1), 0, 0 + lineHeight, 2);
    redrawCmdRecvd = false;

    if (progress >= 100) {
      tft.fillScreen(TFT_BLUE);
      tft.setTextSize(3);
      tft.setTextColor(TFT_WHITE, TFT_BLUE);
      tft.drawString("GO", 45, 20, 2);
      tft.drawString("COMS", 20, 80, 2);
      tft.drawString("3930!", 18, 130, 2);
      delay(6000);
      ESP.restart();
    } else {
      tft.fillRect(15, lineHeight * 2 + 5, 100, 6, TFT_GREEN);
      tft.fillRect(16, lineHeight * 2 + 5 + 1, progress, 4, TFT_BLUE);
    }
    redrawProgress = false;
  }
}
