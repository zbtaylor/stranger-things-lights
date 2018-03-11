#include "ESP8266WiFi.h"
#include "FastLED.h"
#include "ArduinoJson.h"

#define NUM_LEDS 50
#define DATA_PIN 12

// Login details for your router
const char* ssid     = "";
const char* password = "";

// Change host to the URL of your app
const char* host     = "";
String path          = "/lights.json";

CRGB leds[NUM_LEDS];

// Set LED color
CRGB setColor() {
  int colors[] = {CRGB::Red, CRGB::Blue, CRGB::Green, CRGB::Purple};
  int randomNum = random(0,4);
  return colors[randomNum];
}

// Convert each letter in the message to the location of the corresponding LED
int letterToNumber(char letter) {
  // This ordering accounts for the way the lights were strung on the wall
  char alphabet[] = {'A','B','C','D','E','F','G','H','\n','\n','Q','P','O','N','M','L','K','J','I','\n','\n','R','S','T','U','V','W','X','Y','Z'};
  for (int i = 0; i <= 30; i++) {
    if (alphabet[i] == letter) {
      return i;
    }
  }
}

// Spooky flickering
void flicker(int ledNum) {
  CRGB color = setColor();

  // Randomize number of flickers
  int flickers = random(0,4);

  for (int i = 0; i <= flickers; i++) {
    leds[ledNum] = color;
    FastLED.setBrightness(random(12,255));
    FastLED.show();
    FastLED.delay(random(25,350));
    leds[ledNum] = CRGB::Black;
    FastLED.show();
    FastLED.delay(random(25,75));
  }

  FastLED.delay(75);
  FastLED.setBrightness(255);
  leds[ledNum] = color;
  FastLED.show();
  FastLED.delay(1000);
  leds[ledNum] = CRGB::Black;
  FastLED.show();
  FastLED.delay(750);
}

// Blink all LEDs
void blinkAll() {
  FastLED.setBrightness(255);
  for (int i = 0; i <= NUM_LEDS; i++) {
    leds[i] = setColor();
  }
  FastLED.show();
  FastLED.delay(500);
  for (int i = 0; i <= NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
  FastLED.delay(500);
}

void lightItUp(String message) {
  message.toUpperCase();
  message.replace(" ","");

  for (int j = 0; j < message.length(); j++) {
    int ledNum = letterToNumber(message[j]);
    flicker(ledNum);
  }
}

void setup() {
  // Connect to WiFi
  Serial.begin(115200);
  delay(10);
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  int wifi_ctr = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: " + WiFi.localIP());

  // Set up FastLED
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
}

void loop() {
  // Connect to Heroku
  Serial.print("connecting to ");
  Serial.println(host);
  WiFiClient client;
  const int httpPort = 80;

  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  // Send a GET request for the JSON file
  client.print(String("GET ") + path + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: keep-alive\r\n\r\n");

  delay(500);

  String section="header";

  while(client.available()) {
    String line = client.readStringUntil('\r');

    if (section == "header") {
      Serial.print(".");
      if (line == "\n") {
        section = "json";
      }
    } else if (section == "json") {
      section = "ignore";
      String result = line.substring(1);

      // Parse the JSON
      int size = result.length() + 1;
      char json[size];
      result.toCharArray(json, size);
      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& json_parsed = jsonBuffer.parseObject(json);

      if (!json_parsed.success()) {
        Serial.println("parseObject() failed");
        return;
      }

      String message = json_parsed["message"];

      // No time for novels
      if (message.length() > 25) {
        message = "tldr";
      }

      lightItUp(message);
    }
  }
}