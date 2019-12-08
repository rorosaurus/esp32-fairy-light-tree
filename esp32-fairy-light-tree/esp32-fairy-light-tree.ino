// control some colorful fairy lights directly from ESP32 pins

#define STRIP_1_PIN 4
#define STRIP_1_CHANNEL 0

#define STRIP_2_PIN 14
#define STRIP_2_CHANNEL 1

#define FREQUENCY 5000
#define RESOLUTION 12 // 4096 different duty cycles possible

#define MAX_DUTY 1200 // roughly here is where the LEDs appear to max out, any more duty cycle is unneccesary

#define MIN_SPEED 10
#define MAX_SPEED 50

int strip1duty = 0;
int strip1speed = MIN_SPEED;

int strip2duty = MAX_DUTY;
int strip2speed = -MIN_SPEED;

void setup() {
  Serial.begin(9600);

  randomSeed(analogRead(36)); // seed rand
  
  ledcSetup(STRIP_1_CHANNEL, FREQUENCY, RESOLUTION); // configure LED PWM functionalitites
  ledcAttachPin(STRIP_1_PIN, STRIP_1_CHANNEL); // attach the channel to the GPIO to be controlled

  ledcSetup(STRIP_2_CHANNEL, FREQUENCY, RESOLUTION); // configure LED PWM functionalitites
  ledcAttachPin(STRIP_2_PIN, STRIP_2_CHANNEL); // attach the channel to the GPIO to be controlled
}

void loop() {
//  Serial.print("strip 1 duty: ");
//  Serial.println(strip1duty);
  
  // write new PWM
  ledcWrite(STRIP_1_CHANNEL, strip1duty); // changing the LED brightness with PWM
  ledcWrite(STRIP_2_CHANNEL, strip2duty); // changing the LED brightness with PWM
  
  // update duty cycles
  strip1duty += strip1speed;
  strip2duty += strip2speed;
  
  if (strip1duty <= 0) {
    strip1duty = 0;
    strip1speed = random(MIN_SPEED, MAX_SPEED);// new positive int
  }
  if (strip2duty <= 0) {
    strip2duty = 0;
    strip2speed = random(MIN_SPEED, MAX_SPEED);// new positive int
  }

  if (strip1duty >= MAX_DUTY) {
    strip1duty = MAX_DUTY;
    strip1speed = random(-MIN_SPEED, -MAX_SPEED);// new negative int
  }
  if (strip2duty >= MAX_DUTY) {
    strip2duty = MAX_DUTY;
    strip2speed = random(-MIN_SPEED, -MAX_SPEED);// new negative int
  }
}
