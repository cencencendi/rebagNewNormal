#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>

SoftwareSerial mySerial(2, 3);

const int sensorIR1 = 4;
const int sensorIR2 = 5;
const int sensorIR3 = 6;
const int sensorIR4 = 8;
const int sensorIR = 7;
unsigned long seconds, minutes, hours;
unsigned long prev_time;

void setup() {
  pinMode(sensorIR1, INPUT_PULLUP);
  pinMode(sensorIR2, INPUT_PULLUP);
  pinMode(sensorIR3, INPUT_PULLUP);
  pinMode(sensorIR4, INPUT_PULLUP);
  pinMode(sensorIR, INPUT_PULLUP);

  Serial.begin(9600);
  mySerial.begin (9600);
  mp3_set_serial (mySerial);
  delay(10);
  
  mp3_set_volume (100);
  delay(10);

  // Setting register arduino untuk membuat timer
  noInterrupts();
  TCCR1A = 0;             //initialize timer1
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 62500;          //frequency target: 1 Hz
  TCCR1B |= 0b00001100;   //256 prescaler
  TIMSK1 |= 0b00000010;   //enable timer compare interrupt
  interrupts();

}

ISR(TIMER1_COMPA_vect)  //timer compare ISR: update counter
{
  seconds++;
  clockdisplay();
}

void loop() {
  // put your main code here, to run repeatedly:  
  bool bacaSensorIR = digitalRead(sensorIR);
  bool state = checking();
  
    if (hours % 4 == 0 &&minutes == 0 && seconds == 0){
        mp3_play(5);
        delay(5000);
    }
    
    // Sensor tangan hanya akan membaca ketika semua barang sudah masuk
    if(bacaSensorIR == LOW && state){
      mp3_play(2);
      Serial.println("Tidak ada halangan");
      delay(5000);
    }
}

void clockdisplay(){
  if (seconds==60){
    minutes++;
    seconds = 0;
  }
  if (minutes==60){
    hours++;
    minutes = 0;
  }
  if (hours==24){
    hours = 0;
  }
  Serial.print(hours);
  Serial.print(":");
  Serial.print(minutes);
  Serial.print(":");
  Serial.println(seconds);
}

bool checking(){
  bool bacaSensorIR1 = digitalRead(sensorIR1);
  bool bacaSensorIR2 = digitalRead(sensorIR2);
  bool bacaSensorIR3 = digitalRead(sensorIR3);
  bool bacaSensorIR4 = digitalRead(sensorIR4);
  
  if(millis() - prev_time > 4000 && (bacaSensorIR1 || bacaSensorIR2 || bacaSensorIR3 || bacaSensorIR4)){
      Serial.println("Barang belum masuk");
      mp3_play (1);
      prev_time = millis();
    }

  if (!bacaSensorIR1 && !bacaSensorIR2 && !bacaSensorIR3 && !bacaSensorIR4)return true;
  else return false;
}
