#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define DISABLE_CODE_FOR_RECEIVER 
#define sendPin 3
const int b1 = 4;
const int b2 = 5;
const int b3 = 6;
const int b4 = 7;

int timer;
int temp=23;
unsigned long aMicroseconds;
LiquidCrystal_I2C lcd(0x27,16,2);

void setup() {   
  //Cấu hình Timer/Counter 2 trong chế độ PWM, tạo tín hiệu PWM trên chân đầu ra OC2B với tần số 38kHz và cấu hình chân 3 (sendPin)(cũng là chân OC2B) là chân ra
    TCNT2 = 0;
    TCCR2A |= _BV(COM2B1); 
    const uint16_t tPWMWrapValue = (F_CPU / 2000) / (38000); 
    TCCR2A = _BV(WGM20); 
    TCCR2B = _BV(WGM22) | _BV(CS20); 
    OCR2A = tPWMWrapValue - 1; 
    OCR2B = ((tPWMWrapValue * 30) / 100) - 1;
    TCNT2 = 0;
  //Khởi tạo input, output và lcd
    pinMode(sendPin, OUTPUT);
    pinMode(b1, INPUT);
    pinMode(b2, INPUT);
    pinMode(b3, INPUT);
    pinMode(b4, INPUT);
    lcd.begin(16,2);
    lcd.init();
    lcd.backlight();
}

void wakeUp() {
  timer = 0;
}

void loop() {
  timer = 0;
  attachInterrupt(0, wakeUp, HIGH); //khi các chân Digital ở mức HIGH thì hàm wakeUp gọi liên tục thì timer = 0
  const uint16_t rawDataON[] =
{3200, 9900,  480, 1480,  480, 480,  480, 480,  480,  480, 480, 1480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 1480, 480, 480,  480, 480,  480, 480,  480, 480,  480, 1480,  480, 480,  480, 480,  480, 1480,  480, 1480,  480, 480,  480, 480,  480};
  const uint16_t rawDataOFF[] = 
{3200, 9900,  480, 1480,  480, 480,  480, 480,  480,  480,  480, 1480, 480,480,  480, 480,  480,480,  480, 1480,  480, 1480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480,480, 480, 480, 480, 1480,  480, 480,  480, 1480,  480, 480,  480, 480,  480, 480,  480, 1480,  480};
  const uint16_t rawData23[] =
{3200, 9900,  480, 1480,  480, 480, 480, 480,  480, 480, 480, 1480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 1480,  480, 480,  480, 480,  480, 480,  480, 1480,  480,480, 480, 480,  480, 480,  480, 480, 480, 1480,  480, 480,  480, 480, 480, 480,  480, 1480,  480, 480,  480, 480, 480};
  const uint16_t rawData24[] = 
{3144, 9654,  496, 1564,  530, 512,  506, 510,  454, 562,  452, 1582,  538, 478,  512, 530,  450, 542,  474, 542,  502, 514,  500, 544,  502, 520,  522, 1556,  446, 548,  468, 548,  524, 514,  502, 1532,  472, 542,  502, 514,  502, 1536,  466, 548,  524, 1532,  478, 538,  502, 514,  502, 514,  502, 1530,  476, 540,  502, 1532,  472};
  const uint16_t rawData25[] =
{3200, 9900,  480, 1480,  480, 480,  480, 480,  480, 480,  480, 1480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 1480,  480, 480,  480, 480,  480, 480,  480, 1480,  480, 480,  480, 1480,  480, 480,  480, 480,  480, 1480,  480, 480,  480, 480,  480, 480,  480, 1480,  480, 1480,  480, 480,  480};
  const uint16_t rawData26[] =
{3200, 9900,  480, 1480,  480, 480,  480, 480,  480, 480,  480, 1480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 1480,  480, 480,  480, 480,  480, 480,  480, 1480, 480, 480,  480, 1480,  480, 1480,  480, 480,  480, 1480,  480, 480,  480, 480,  480, 480,  480, 1480,  480, 1480,  480, 1480,  480}; 
  while (timer < 10000) {
    if (digitalRead(b1) == HIGH) {
      timer = 0;
      delay(500);
      for (uint_fast16_t i = 0; i < 59; i++) {
        aMicroseconds = rawDataON[i];
        if (i & 1) {
          unsigned long start = micros() - (64 / clockCyclesPerMicrosecond()); //gán start cho thời điểm hiện tại
          while (micros() - start < aMicroseconds) {    }  // khi thời điểm sau - thời điểm hiện tại < mảng[i] thì không làm gì, khi lớn hơn thì dừng. Tóm lại để nghỉ
        }
        else {
          analogWrite(sendPin, (256L*40)/100);
          unsigned long start = micros() - (64 / clockCyclesPerMicrosecond());
          while (micros() - start < aMicroseconds) {  }
          analogWrite(sendPin, 0);
          /*
          i lẻ ứng với xung thấp - chỉ nghỉ
          i chẵn ứng với xung cao - gửi xung cao (bật led) xong nghỉ rồi tắt led 
          */
        }
      }
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Da bat dieu hoa."); //mảng là mảng bật ở 23 độ nhé
      delay(100);
    }
    if (digitalRead(b2) == HIGH) {
      timer = 0;
      delay(500);
      for (uint_fast16_t i = 0; i < 59; i++) {
        aMicroseconds = rawDataOFF[i];
        if (i & 1) {
          unsigned long start = micros() - (64 / clockCyclesPerMicrosecond()); 
          while (micros() - start < aMicroseconds) {    }
        }
        else {
          analogWrite(sendPin, (256L*40)/100);
          unsigned long start = micros() - (64 / clockCyclesPerMicrosecond());
          while (micros() - start < aMicroseconds) {  }
          analogWrite(sendPin, 0);
        }
      }
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Da tat dieu hoa."); //mảng là mảng tắt ở 23 độ nhé
      delay(100);
    }
    if(digitalRead(b3) == HIGH) {
      temp++;
      timer = 0;
      delay(500);
      if(temp==24){
        for (uint_fast16_t i = 0; i < 59; i++) {
          aMicroseconds = rawData24[i];
          if (i & 1) {
            unsigned long start = micros() - (64 / clockCyclesPerMicrosecond()); 
            while (micros() - start < aMicroseconds) {    }
          }   
          else {
            analogWrite(sendPin, (256L*40)/100);
            unsigned long start = micros() - (64 / clockCyclesPerMicrosecond());
            while (micros() - start < aMicroseconds) {  }
            analogWrite(sendPin, 0);
          }
        }
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Nhiet do dang la");
        lcd.setCursor(11,1);
        lcd.print("24 do");
        delay(100);
      }
      if(temp==25){
        for (uint_fast16_t i = 0; i < 59; i++) {
          aMicroseconds = rawData25[i];
          if (i & 1) {
            unsigned long start = micros() - (64 / clockCyclesPerMicrosecond()); 
            while (micros() - start < aMicroseconds) {    }
          }   
          else {
            analogWrite(sendPin, (256L*40)/100);
            unsigned long start = micros() - (64 / clockCyclesPerMicrosecond());
            while (micros() - start < aMicroseconds) {  }
            analogWrite(sendPin, 0);
          }
        }
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Nhiet do dang la");
        lcd.setCursor(11,1);
        lcd.print("25 do");
        delay(100);
      }
      if(temp==26){
        for (uint_fast16_t i = 0; i < 59; i++) {
          aMicroseconds = rawData26[i];
          if (i & 1) {
            unsigned long start = micros() - (64 / clockCyclesPerMicrosecond()); 
            while (micros() - start < aMicroseconds) {    }
          }   
          else {
            analogWrite(sendPin, (256L*40)/100);
            unsigned long start = micros() - (64 / clockCyclesPerMicrosecond());
            while (micros() - start < aMicroseconds) {  }
            analogWrite(sendPin, 0);
          }
        }
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Nhiet do dang la");
        lcd.setCursor(11,1);
        lcd.print("26 do");
        delay(100);
      }
      if(temp>26){
        temp=26;
      }
    }
    if(digitalRead(b4) == HIGH) {
      temp--;
      timer = 0;
      delay(500);
      if(temp==25){
        for (uint_fast16_t i = 0; i < 59; i++) {
          aMicroseconds = rawData25[i];
          if (i & 1) {
            unsigned long start = micros() - (64 / clockCyclesPerMicrosecond()); 
            while (micros() - start < aMicroseconds) {    }
          } 
          else {
            analogWrite(sendPin, (256L*40)/100);
            unsigned long start = micros() - (64 / clockCyclesPerMicrosecond());
            while (micros() - start < aMicroseconds) {  }
            analogWrite(sendPin, 0);
          }
        }
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Nhiet do dang la");
        lcd.setCursor(11,1);
        lcd.print("25 do");
        delay(100);
      }
      if(temp==24){
        for (uint_fast16_t i = 0; i < 59; i++) {
          aMicroseconds = rawData24[i];
          if (i & 1) {
            unsigned long start = micros() - (64 / clockCyclesPerMicrosecond()); 
            while (micros() - start < aMicroseconds) {    }
          } 
          else {
            analogWrite(sendPin, (256L*40)/100);
            unsigned long start = micros() - (64 / clockCyclesPerMicrosecond());
            while (micros() - start < aMicroseconds) {  }
            analogWrite(sendPin, 0);
          }
        }
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Nhiet do dang la");
        lcd.setCursor(11,1);
        lcd.print("24 do");
        delay(100);
      }
      if(temp==23){
        for (uint_fast16_t i = 0; i < 59; i++) {
          aMicroseconds = rawData23[i];
          if (i & 1) {
            unsigned long start = micros() - (64 / clockCyclesPerMicrosecond()); 
            while (micros() - start < aMicroseconds) {    }
          } 
          else {
            analogWrite(sendPin, (256L*40)/100);
            unsigned long start = micros() - (64 / clockCyclesPerMicrosecond());
            while (micros() - start < aMicroseconds) {  }
            analogWrite(sendPin, 0);
          }
        }
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Nhiet do dang la");
        lcd.setCursor(11,1);
        lcd.print("23 do");
        delay(100);
      }
      if(temp<23){
        temp = 23;
      }
    }
    delay(1);
    timer = timer + 1;
  }
}
// Sửa là 3200 9900 480 1480
/*
- Nếu thích chỉnh ở 27 đến 30 thì để thành mảng như này:
(Bật và tắt ở 27 độ)
  const uint16_t rawDataON[] = {
3200, 9900,  480, 1480,  480, 480,  480, 480,  480, 480,  480, 1480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480, 480, 480,  480, 480,  480, 1480,  480, 1480,  480, 480,  480, 480,  480, 480,  480, 1480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480, 480, 480,  480}; 
//  const uint16_t rawDataON[] = {
//3174, 9652,  542, 1518,  540, 502,  514, 502,  514, 502,  514, 1518,  514, 502,  534, 480,  536, 480,  542, 474,  542, 500,  530, 514,  514, 502,  540, 502,  516, 500,  514, 502,  432, 584,  540, 1490,  542, 1518,  464, 552,  454, 562,  454, 562,  540, 1518,  516, 500,  516, 500,  482, 532,  542, 500,  516, 500,  540, 476,  542};
  const uint16_t rawDataOFF[] = {
3200, 9900,  480, 1480,  480, 480,  480, 480,  480, 480,  480, 1480,  480, 480,  480, 480,  480, 480,  480, 1480,  480, 1480,  480, 480,  480, 480, 480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 1480,  480, 480,  480, 1480,  480, 480,  480, 480, 480, 480,  480, 1480,  480};  
//  const uint16_t rawDataOFF[] = {
//3142, 9684,  538, 1522,  538, 478,  508, 508,  532, 486,  532, 1500,  534, 482,  536, 508,  506, 536,  510, 1548,  512, 1520,  498, 518,  536, 506,  510, 504,  512, 506,  510, 506,  454, 562,  456, 560,  458, 558,  494, 522,  538, 504,  488, 526,  538, 1548,  512, 504,  496, 1536,  538, 478,  502, 512,  530, 486,  532, 1526,  514};
  const uint16_t rawData27[] = {
3150, 9628,  564, 1518,  540, 500,  516, 502,  514, 502,  514, 1518,  542, 474,  542, 502,  514, 502,  514, 502,  514, 502,  540, 502,  514, 500,  540, 1518,  502, 514,  528, 488,  542, 500,  516, 1516,  518, 1514,  544, 498,  516, 500,  494, 524,  542, 1516,  544, 498,  516, 500,  544, 1516,  516, 500,  518, 498,  432, 584,  540};
  const uint16_t rawData28[] ={
3200, 9900,  480, 1480,  480, 480,  480, 480,  480, 480,  480, 1480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 1480,  480, 480,  480, 480,  480, 480,  480, 1480,  480, 1480,  480, 480,  480, 1480,  480, 480,  480, 1480,  480, 480,  480, 480,  480, 1480,  480, 480,  480, 480,  480, 1480,  480};
//  const uint16_t rawData28[] ={
//3088, 9740,  564, 1520,  434, 582,  480, 534,  540, 504,  534, 1524,  514, 502,  500, 544,  514, 502,  540, 502,  512, 502,  514, 502,  512, 502,  514, 1520,  540, 476,  540, 504,  460, 554,  540, 1494,  538, 1546,  514, 502,  514, 1520,  538, 476,  540, 1520,  540, 502,  500, 514,  540, 1522,  512, 504,  512, 504,  538, 1522,  512};
  const uint16_t rawData29[] ={
3200, 9900,  480, 1480,  480, 480,  480, 480,  480, 480,  480, 1480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 1480,  480, 480,  480, 480,  480, 480,  480, 1480,  480, 1480,  480, 1480,  480, 480,  480, 480,  480, 1480,  480, 480,  480, 480,  480, 1480,  480, 480,  480, 1480,  480, 480,  480};
//  const uint16_t rawData29[] ={
//3150, 9628,  522, 1564,  514, 502,  490, 524,  494, 522,  498, 1560,  514, 502,  514, 502,  464, 552,  530, 486,  492, 550,  514, 502,  540, 502,  462, 1596,  512, 504,  514, 502,  456, 560,  512, 1522,  536, 1522,  512, 1520,  514, 504,  456, 560,  536, 1522,  514, 504,  512, 504,  512, 1520,  528, 488,  538, 1494,  536, 506,  514};
  const uint16_t rawData30[] ={
3200, 9900,  480, 1480,  480, 480,  480, 480,  480, 480,  480, 1480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 480,  480, 1480,  480, 480,  480, 480,  480, 480,  480, 1480,  480, 1480,  480, 1480,  480, 1480,  480, 480,  480, 1480,  480, 480,  480, 480,  480, 1480,  480, 480,  536, 1480,  480, 1480,  480};
//  const uint16_t rawData30[] ={
//3144, 9680,  540, 1548,  514, 502,  512, 504,  466, 576,  512, 1520,  540, 476,  540, 504,  538, 504,  456, 560,  532, 484,  534, 482,  534, 482,  538, 1520,  512, 504,  502, 514,  538, 504,  512, 1520,  496, 1562,  460, 1572,  540, 1518,  514, 502,  538, 1522,  536, 480,  536, 480,  538, 1522,  432, 582,  536, 1498,  536, 1524,  512};

/*
- Các mảng thu được khi bật tắt ở 23 độ: 
//  const uint16_t rawDataON[] = 
//{3104, 9744,  476, 1564,  522, 516,  502, 514,  472, 570,  502, 1556,  504, 514,  502, 514,  484, 534,  472, 570,  502, 512,  504, 514,  502, 514,  502, 514,  546, 476,  522, 514,  472, 570,  502, 1558,  502, 512,  504, 512,  504, 514,  502, 512,  504, 1530,  472, 544,  472, 546,  526, 1532,  478, 1554,  474, 544,  498, 544,  500};
//  const uint16_t rawDataOFF[] = 
//{3144, 9656,  564, 1522,  512, 504,  464, 578,  512, 504,  538, 1520,  538, 478,  538, 504,  512, 504,  540, 1520,  512, 1520,  538, 504,  514, 502,  464, 578,  514, 502,  538, 504,  514, 502,  514, 502,  538, 504,  540, 502,  514, 502,  512, 504,  492, 1540,  540, 502,  462, 1596,  490, 526,  538, 504,  514, 502,  538, 1522,  528};
//  const uint16_t rawData23[] =
//{3080, 9696,  526, 1534,  524, 492,  524, 514,  502, 514,  472, 1586,  498, 518,  472, 568,  504, 512,  500, 542,  504, 512,  504, 512,  504, 512,  474, 1584,  504, 514,  504, 512,  500, 542,  504, 1530,  504, 512,  502, 514,  502, 512,  504, 514,  472, 1586,  504, 514,  470, 548,  526, 512,  474, 1586,  504, 512,  476, 568,  504};
//  const uint16_t rawData25[] =
//{3146, 9656,  574, 1482,  550, 492,  520, 496,  520, 496,  504, 1554,  520, 496,  520, 496,  520, 496,  536, 480,  500, 516,  550, 466,  550, 466,  550, 1508,  520, 494,  522, 494,  520, 496,  440, 1594,  498, 518,  550, 1484,  550, 468,  548, 466,  550, 1510,  548, 468,  550, 492,  496, 520,  544, 1488,  550, 1536,  520, 496,  520};
//  const uint16_t rawData26[] =
//{3076, 9778,  482, 1574,  480, 538,  498, 544,  502, 516,  524, 1556,  504, 512,  500, 542,  486, 556,  504, 514,  504, 512,  504, 512,  504, 514,  500, 1560,  502, 512,  504, 518,  524, 512,  504, 1528,  506, 514,  496, 1538,  468, 1588,  502, 514,  504, 1556,  504, 512,  504, 512,  504, 514,  504, 1530,  472, 1562,  470, 1588,  504};
*/