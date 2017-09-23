#define EXTERNAL_FAN A3
#define INTERNAL_FAN A4

void setup() {
  pinMode(EXTERNAL_FAN, OUTPUT);
  pinMode(INTERNAL_FAN, OUTPUT);
  Serial.begin(9600);
}

boolean on = true;
void loop() {
  if (on) on=false;
  else on=true;
  Serial.println(on);
  if (on) {
    digitalWrite(EXTERNAL_FAN, HIGH);
    digitalWrite(INTERNAL_FAN, LOW);
  } else {
    digitalWrite(EXTERNAL_FAN, LOW);
    digitalWrite(INTERNAL_FAN, HIGH);
  }
  delay(5000);
}
