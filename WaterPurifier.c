#include <Servo.h>
#include <Wire.h> //i2c 통신을 위한 라이브러리
#include <LiquidCrystal_I2C.h> //LCD 1602 I2C용 라이브러리
#include <IRremote.h>

LiquidCrystal_I2C lcd(0x27,16,2); //접근주소

Servo servo_b;

//사용할 변수 목록
int cds = A1;
int water_sensor = A2;

int servo_b_pin = 2;
int led_g = 3;
int datapin = 5;
int latchpin = 4;
int clockpin = 6;
int switch_2 = 7;
int switch_1 = 8;
int RECV_PIN= 9;
int led_r = 10;
int led_b = 11;
int switch_3 = 12;
int buzzer = 13;

int filter_life = 2;
int degree = 60;

IRrecv irrecv(RECV_PIN);
decode_results results;

//처음 실행 단계
void setup() {
  
  servo_b.attach(servo_b_pin);

  irrecv.enableIRIn();

  pinMode(buzzer, OUTPUT);
  pinMode(led_g, OUTPUT);
  pinMode(led_r, OUTPUT);
  pinMode(led_b, OUTPUT);
  pinMode(latchpin, OUTPUT);
  pinMode(datapin, OUTPUT);
  pinMode(clockpin, OUTPUT);
  pinMode(switch_1, INPUT_PULLUP);
  pinMode(switch_2, INPUT_PULLUP);
  pinMode(switch_3, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  analogWrite(led_r,0), analogWrite(led_g,255), analogWrite(led_b,0);
  lcd.setCursor(0,1);
  lcd.print("Water");
}

//반복 단계
void loop() {
  lcd.setCursor(0,0);
  lcd.print("F_life:");
  lcd.setCursor(8,0); //필터수명 지정
  lcd.print("  ");
  lcd.setCursor(8,0);
  lcd.print(filter_life);

//택트 스위치, RGB센서, LCD 
  if(digitalRead(switch_1)==LOW){ //1번째 스위치(온수)를 눌렀을 때
    analogWrite(led_r,255), analogWrite(led_g,0), analogWrite(led_b,0);
    lcd.backlight(); //백라이트 켜기
    lcd.setCursor(0,1);
    lcd.print("                ");
    lcd.setCursor(0,1);
    lcd.print("Hot Water");
    lcd.setCursor(11,1);
    lcd.print(degree);
    lcd.print("'");
  } 
  else if(digitalRead(switch_2)==LOW){ //2번째 스위치(정수)를 눌렀을 때
    analogWrite(led_r,0), analogWrite(led_g,255), analogWrite(led_b,0);
    lcd.backlight();
    lcd.setCursor(0,1);
    lcd.print("                ");
    lcd.setCursor(0,1);
    lcd.print("Water");
  }
  else if(digitalRead(switch_3)==LOW){ //3번째 스위치(냉수)를 눌렀을 때
    analogWrite(led_r,0), analogWrite(led_g,0), analogWrite(led_b,255);
    lcd.backlight();
    lcd.setCursor(0,1);
    lcd.print("                ");
    lcd.setCursor(0,1);
    lcd.print("Cold Water");
  }

//적외선 리모컨, 버튼 측정 필요
  if(irrecv.decode(&results)==true) {
    switch(results.value){
      case 0xFF30CF:  // 1번 버튼 - 온수
        analogWrite(led_r,255),analogWrite(led_g,0),analogWrite(led_b,0);
        lcd.backlight();
        lcd.setCursor(0,1);
        lcd.print("                ");
        lcd.setCursor(0,1);
        lcd.print("Hot Water");
        lcd.setCursor(11,1);
        lcd.print(degree);
        lcd.print("'");
        break;
      case 0xFF18E7:  // 2번 버튼 - 정수
        analogWrite(led_r,0),analogWrite(led_g,255),analogWrite(led_b,0);
        lcd.backlight();
        lcd.setCursor(0,1);
        lcd.print("                ");
        lcd.setCursor(0,1);
        lcd.print("Water");
        break;
      case 0xFF7A85:  // 3번 버튼 - 냉수
        analogWrite(led_r,0),analogWrite(led_g,0),analogWrite(led_b,255);
        lcd.backlight();
        lcd.setCursor(0,1);
        lcd.print("                ");
        lcd.setCursor(0,1);
        lcd.print("Cold Water");
        break;
      case 0xFFA857: // + 입력
        degree += 10;
        if (degree>=100){
          degree=100;
        }
        lcd.setCursor(0,1);
        lcd.print("                ");
        lcd.setCursor(6,1);
        lcd.print("set");
        lcd.setCursor(11,1);
        lcd.print(degree);
        lcd.print("'");
        break;
      case 0xFFE01F: // - 입력
        degree -= 10;
        if (degree<=50){
          degree=50;
        }
        lcd.setCursor(0,1);
        lcd.print("                ");
        lcd.setCursor(6,1);
        lcd.print("set");
        lcd.setCursor(11,1);
        lcd.print(degree);
        lcd.print("'");
        break;
    }
    irrecv.resume();
  }

//조도센서, 피에조 부저, 서보모터
  if (analogRead(cds) > 820){
    tone(buzzer, 300); // 부저 알림음
    delay(300);
    tone(buzzer, 400);
    delay(300);
    tone(buzzer, 500);
    delay(300);
    noTone(buzzer);
    servo_b.write(0); // 개폐장치 서보모터 동작
    delay(100);
    servo_b.write(90); //개폐장치 열기
    delay(100);
   for (int i = 0; i < 8; i++) {
        digitalWrite(latchpin, LOW);
        shiftOut(datapin, clockpin, LSBFIRST, 1 << i); // 시프트 레지스터 동작
        digitalWrite(latchpin, HIGH);
        delay(200);
        if(i==7){i=0;}
        if(analogRead(water_sensor) < 200) { //수분수위센서의 값이 기준값을 넘으면 for문 탈출
        break;
        }
      }  
    servo_b.write(0); //개폐장치 닫기
    tone(buzzer, 500); // 부저 알림음
    delay(300);
    tone(buzzer, 400);
    delay(300);
    tone(buzzer, 300);
    delay(300);
    noTone(buzzer);
    filter_life -= 1;
    delay(3000);

  irrecv.enableIRIn();
  }

       if (filter_life==1){
      lcd.setCursor(10,0);
      lcd.print("Change");
    }
    else if (filter_life==0){
      lcd.setCursor(10,0);
      lcd.print("Danger");
  }
    else if (filter_life==-1){
      filter_life+=11;
      lcd.setCursor(7,0);
      lcd.print("          ");
   }
}
