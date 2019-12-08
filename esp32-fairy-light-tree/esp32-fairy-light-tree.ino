// control some colorful fairy lights directly from ESP32 pins

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

int strip2duty = MAX_DUTY;
int strip2speed = -MIN_SPEED;
int strip2millis = 0;
bool strip2paused = false;

void setup() {
  Serial.begin(9600);

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
  ledcWrite(STRIP_1_CHANNEL, strip1duty); // changing the LED brightness with PWM
  ledcWrite(STRIP_2_CHANNEL, strip2duty); // changing the LED brightness with PWM

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
