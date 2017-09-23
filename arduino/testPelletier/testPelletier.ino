#define HBRIDGE_INH  MISO
#define HBRIDGE_IN1  MOSI
#define HBRIDGE_IN2  SCK


void setup() {
  pinMode(HBRIDGE_IN1, OUTPUT);
  pinMode(HBRIDGE_IN2, OUTPUT);
  pinMode(HBRIDGE_INH, OUTPUT);
  digitalWrite(HBRIDGE_INH, HIGH); // wake up the device
  Serial.begin(9600);
}

boolean on = true;
void loop() {
  if (on) on=false;
  else on=true;
  Serial.println(on);
  if (on) {
    digitalWrite(HBRIDGE_IN1, HIGH);
    digitalWrite(HBRIDGE_IN2, LOW);
  } else {
    digitalWrite(HBRIDGE_IN1, LOW);
    digitalWrite(HBRIDGE_IN2, HIGH);
  }
  delay(5000);
}
