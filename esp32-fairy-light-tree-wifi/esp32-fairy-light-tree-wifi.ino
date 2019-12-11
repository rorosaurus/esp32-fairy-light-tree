// control some colorful fairy lights directly from ESP32 pins
// connects to home wifi and pings devices to determine whether to turn on strips

#include <WiFi.h>
#include <ESP32Ping.h>

const char* ssid     = "";
const char* password = "";

const IPAddress strip_1_ip(192, 168, 1, 0);
const IPAddress strip_2_ip(192, 168, 1, 1);

#define WIFI_CHECK_INTERVAL_MILLIS 300000 // every 5 min
int lastWifiCheck = -WIFI_CHECK_INTERVAL_MILLIS;

TaskHandle_t wifiTask;

#define STRIP_1_PIN 4
#define STRIP_1_CHANNEL 0

#define STRIP_2_PIN 14
#define STRIP_2_CHANNEL 1

#define FREQUENCY 5000
#define RESOLUTION 12 // 4096 different duty cycles possible

#define MAX_DUTY 900 // roughly here is where the LEDs appear to max out, any more duty cycle is unneccesary
#define STALL_MILLIS 500 // how long to wait after we reach 0 or MAX_DUTY

#define MIN_SPEED 5
#define MAX_SPEED 50

int strip1duty = 0;
int strip1speed = MIN_SPEED;
int strip1millis = 0;
bool strip1paused = false;
bool strip1enabled = true;

int strip2duty = MAX_DUTY;
int strip2speed = -MIN_SPEED;
int strip2millis = 0;
bool strip2paused = false;
bool strip2enabled = true;

void checkForDevices() {
  Serial.println("Checking for devices...");
  // time for a new check
  if(Ping.ping(strip_1_ip)) {
    strip1enabled = true;
    Serial.println("strip1 enabled");
  } else {
    strip1enabled = false;
    Serial.println("strip1 disabled");
  }

  if(Ping.ping(strip_2_ip)) {
    strip2enabled = true;
    Serial.println("strip2 enabled");
  } else {
    strip2enabled = false;
    Serial.println("strip2 disabled");
  }

  lastWifiCheck = millis();
}

void wifiTaskcode (void * parameter) {
  for(;;) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Reconnecting to WiFi");
      WiFi.disconnect();
      WiFi.begin(ssid, password);
      
      while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.print(".");
      }

      Serial.println();
      Serial.print("WiFi reconnected with ip ");  
      Serial.println(WiFi.localIP());
    }
    
    if (lastWifiCheck + WIFI_CHECK_INTERVAL_MILLIS < millis()) {
      checkForDevices();
    }
  }
}

void setup() {
  Serial.begin(115200);

  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println("Connecting to WiFi");
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("WiFi connected with ip ");  
  Serial.println(WiFi.localIP());

  xTaskCreatePinnedToCore(
      wifiTaskcode, /* Function to implement the task */
      "wifiTask", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &wifiTask,  /* Task handle. */
      0); /* Core where the task should run */

  randomSeed(analogRead(36)); // seed rand
  
  ledcSetup(STRIP_1_CHANNEL, FREQUENCY, RESOLUTION); // configure LED PWM functionalitites
  ledcAttachPin(STRIP_1_PIN, STRIP_1_CHANNEL); // attach the channel to the GPIO to be controlled

  ledcSetup(STRIP_2_CHANNEL, FREQUENCY, RESOLUTION); // configure LED PWM functionalitites
  ledcAttachPin(STRIP_2_PIN, STRIP_2_CHANNEL); // attach the channel to the GPIO to be controlled
}

void loop() {
  delay(20);
  
//  Serial.print("strip 1 duty: ");
//  Serial.println(strip1duty);
  
  // write new PWM
  if (strip1enabled) {
    ledcWrite(STRIP_1_CHANNEL, strip1duty); // changing the LED brightness with PWM
  } else {
    ledcWrite(STRIP_1_CHANNEL, 0);
  }
  if (strip2enabled) {
    ledcWrite(STRIP_2_CHANNEL, strip2duty); // changing the LED brightness with PWM
  } else {
    ledcWrite(STRIP_2_CHANNEL, 0);
  }

  // unpause
  if (strip1millis + STALL_MILLIS <= millis()) strip1paused = false;
  if (strip2millis + STALL_MILLIS <= millis()) strip2paused = false;
  
  // update duty cycles
  if(!strip1paused) strip1duty += strip1speed;
  if(!strip2paused) strip2duty += strip2speed;
  
  if (strip1duty < 0) {
    strip1duty = 0;
    strip1speed = random(MIN_SPEED, MAX_SPEED);// new positive int
    strip1paused = true;
    strip1millis = millis();
  }
  if (strip2duty < 0) {
    strip2duty = 0;
    strip2speed = random(MIN_SPEED, MAX_SPEED);// new positive int
    strip2paused = true;
    strip2millis = millis();
  }

  if (strip1duty > MAX_DUTY) {
    strip1duty = MAX_DUTY;
    strip1speed = random(-MIN_SPEED, -MAX_SPEED);// new negative int
    strip1paused = true;
    strip1millis = millis();
  }
  if (strip2duty > MAX_DUTY) {
    strip2duty = MAX_DUTY;
    strip2speed = random(-MIN_SPEED, -MAX_SPEED);// new negative int
    strip2paused = true;
    strip2millis = millis();
  }
}
