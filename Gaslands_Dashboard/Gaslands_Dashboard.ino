#include <Adafruit_LEDBackpack.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Car.h"
#include "DFRobot_ST7687S_Latch.h"
#include "DFRobot_Display_Clock.h"
#include "SPI.h"
#include <RotaryEncoder.h>


DFRobot_ST7687S_Latch tft(3,5, 6, 7);
Adafruit_24bargraph bar = Adafruit_24bargraph();
Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();

RotaryEncoder encoder(A2, A3);

enum class knobMode : uint8_t
{
  Gear,Hull,Ammo
};
knobMode knob = knobMode::Gear;

//Car(char* cname, uint8_t m_gear, uint8_t hand, uint8_t am0,uint8_t am1,uint8_t am2, uint8_t am3,uint8_t m_hull);
Car train("  Perf Car  ", 6, 4,1,2,3,4, 8);


void setup()
{
  Serial.begin(115200);
  tft.begin();
  bar.begin(0x70);
  alpha4.begin(0x71);
  alpha4.clear();
  alpha4.writeDisplay();
  bar.clear();
  bar.writeDisplay();
  bar.setBrightness(8);
  alpha4.setBrightness(1);
  pinMode(A1, INPUT_PULLUP);
//  tft.fillScreen(DISPLAY_BLUE);
//  tft.setTextColor(DISPLAY_WHITE);
//  tft.setTextBackground(DISPLAY_BLUE);
//  tft.setTextSize(2);
//  tft.setCursor(18, 40);
//  tft.println("Gaslands");
//  tft.setCursor(12, 58);
//  tft.println("Dashboard");
//  tft.setCursor(40,76);
//  tft.println("v1.0");
//  delay(3000);
  updateRound(train);
  updateAmmoQuad(train.ammo[0],train.ammo[1],train.ammo[2],train.ammo[3],5);
  updateHullGraph(train.curr_hull,train.max_hull);
}

void loop()
{
  
  static uint8_t activeAmmo = 0;
  if (digitalRead(A1) == LOW) {
    Serial.println("click");
    if(knob == knobMode::Gear) {
      knob = knobMode::Hull;
      bar.blinkRate(1);
    }
    else if(knob == knobMode::Hull) {
      bar.blinkRate(0);
      knob = knobMode::Ammo;
    }
    else if(knob == knobMode::Ammo) knob = knobMode::Gear;
    delay(500);
  }
  
  //read encoder
  static int pos = 0;
  encoder.tick();
  int newPos=encoder.getPosition();

  ///////////////////gear & hazards
  if(knob == knobMode::Gear) {
    if (pos != newPos) {
      if (newPos < pos) {
        train.hazards++;
        if (train.hazards > 6) train.hazards = 0;
        updateHazardOnly(train.hazards);
      }
      if (newPos > pos) {
        train.curr_gear++;
        if (train.curr_gear > train.max_gear) train.curr_gear = 1;
        updateGearOnly(train.curr_gear);
      }
      pos = newPos;
    }
  }
  ///////////////////hull
  else if(knob == knobMode::Hull) {
    Serial.println("Hull");
    if (pos != newPos) {
      if (newPos < pos) {
        train.curr_hull--;
        if (train.curr_hull < 0) train.curr_hull = 0;

      }
      if (newPos > pos) {
        train.curr_hull++;
        if (train.curr_hull > train.max_hull) train.curr_hull = train.max_hull;
        
      }
      updateHullGraph(train.curr_hull,train.max_hull);
      pos = newPos;
    }  
  }
  
  ///////////////////ammo
  else if(knob == knobMode::Ammo) {
    updateAmmoQuad(train.ammo[0],train.ammo[1],train.ammo[2],train.ammo[3],activeAmmo);
    if (pos != newPos) {
      if (newPos < pos) {
        activeAmmo++;
        if(activeAmmo >3) activeAmmo = 0 ;
      }
      if (newPos > pos) {
        train.ammo[activeAmmo]--;
        if(train.ammo[activeAmmo] > 9) train.ammo[activeAmmo] = 5;
      }
      pos = newPos;
    }
    updateAmmoQuad(train.ammo[0],train.ammo[1],train.ammo[2],train.ammo[3],activeAmmo);
  }

}


//update only the Hazard Character ont he screen
void updateHazardOnly(uint8_t haz) {
  tft.setTextBackground(0xFF60);
  tft.setTextColor(DISPLAY_BLACK);
  tft.setTextSize(2);
  tft.setCursor(59,92);
  tft.print(haz);
}


//update only the Gear line on the screen
void updateGearOnly(uint8_t gear) {
  tft.setLineWidth(3);
  if(gear == 1) {
    tft.drawLine(0,-8,44,-8,DISPLAY_BLACK);
    tft.drawLine(0,-8,-44,-8,DISPLAY_RED);
  }
  else if(gear == 2) {
    tft.drawLine(0,-8,-44,-8,DISPLAY_BLACK);
    tft.drawLine(0,-8,-30,-31,DISPLAY_RED);
  }
  else if(gear == 3) {
    tft.drawLine(0,-8,-30,-31,DISPLAY_BLACK);
    tft.drawLine(0,-8,-10,-42,DISPLAY_RED);
  }
  else if(gear == 4) {
    tft.drawLine(0,-8,-10,-42,DISPLAY_BLACK);
    tft.drawLine(0,-8,10,-42,DISPLAY_RED);
  }
  else if(gear == 5) {
    tft.drawLine(0,-8,10,-42,DISPLAY_BLACK);
    tft.drawLine(0,-8,30,-31,DISPLAY_RED);
  }
  else if(gear == 6) {
    tft.drawLine(0,-8,30,-31,DISPLAY_BLACK);
    tft.drawLine(0,-8,44,-8,DISPLAY_RED);
  }
  tft.fillCircle(0,-8,4,DISPLAY_RED);
}

//update the bargraph display with current hull
void updateHullGraph(uint8_t hull_left, uint8_t max_hull) {
  bar.clear();
  if( hull_left == 0);
  else if (max_hull == hull_left) {
     for (uint8_t i = 0; i < hull_left; i++) {
      bar.setBar(23-i, LED_GREEN);
    }
  }
  else if (hull_left > 3) {
     for (uint8_t i = 0; i < hull_left; i++) {
      bar.setBar(23-i, LED_GREEN);
    }
    bar.setBar(24-max_hull,LED_YELLOW);
  }
  else {
    for (uint8_t i = 0; i < hull_left; i++) {
      bar.setBar(23-i, LED_RED);
    }
    bar.setBar(24-max_hull, LED_YELLOW);
  
  }
  bar.writeDisplay();
}

//update the ammo display, mark active ammo with decimal
void updateAmmoQuad(uint8_t a0, uint8_t a1,uint8_t a2,uint8_t a3, uint8_t active) {
  uint8_t displayBuffer[4] = {0,0,0,0};
  uint8_t ammoIn[4] = {a0,a1,a2,a3};
  for(uint8_t i = 0; i < 4; i++) {
    if(active == i) alpha4.writeDigitAscii(i,ammoIn[i]+48,true);
    else alpha4.writeDigitAscii(i,ammoIn[i]+48);
    }
  alpha4.writeDisplay();
  //delay(1000);
}
  

//full update on the round display
void updateRound(Car car_pull) {
  tft.fillScreen(DISPLAY_BLACK);
  
  //current gear ring
  tft.setLineWidth(3);
  tft.drawCircle(0,0,62,DISPLAY_WHITE);
  tft.setLineWidth(1);
  tft.fillRect(-64,-8,140,72,DISPLAY_BLACK);
  //current gear hashes
  //tft.drawLine(0,-61,0,-54, DISPLAY_WHITE);
  tft.drawLine(-61,-8,-54 ,-8, DISPLAY_WHITE);
  tft.drawLine(61,-8,54 ,-8, DISPLAY_WHITE);
  tft.drawLine(-41,-38,-46,-41,DISPLAY_WHITE);
  tft.drawLine(41,-38,46,-41,DISPLAY_WHITE);
  tft.drawLine(-15,-54,-17,-60,DISPLAY_WHITE);
  tft.drawLine(15,-54,17,-60,DISPLAY_WHITE);
  tft.setTextColor(DISPLAY_WHITE);
  tft.setTextSize(1);
  tft.setTextBackground(DISPLAY_BLACK);
  tft.setCursor(12,53);
  tft.print("1");
  tft.setCursor(26,26);
  tft.print("2");
  tft.setCursor(50,13);
  tft.print("3");
  tft.setCursor(111,53);
  tft.print("6");
  tft.setCursor(98,26);
  tft.print("5");
  tft.setCursor(74,13);
  tft.print("4");


  updateGearOnly(car_pull.curr_gear);
  tft.setLineWidth(1);
  
  //max values
  tft.setTextColor(DISPLAY_LIGHTGREY);
  tft.setTextBackground(DISPLAY_BLACK);
  tft.setCursor(20,78);
  tft.setTextSize(1);

  tft.print("G:");
  tft.print(car_pull.max_gear);
  tft.setCursor(92,78);
  tft.print("H:");
  tft.print(car_pull.handling);

  
  //car name
  String nameForPrint;
  tft.setTextColor(DISPLAY_GREEN);
  tft.setTextBackground(DISPLAY_BLACK);
  tft.setTextSize(1);
  tft.setCursor(tft.width / 2, tft.height / 2);
  
  for (uint8_t i = 0;i < 12; i++) {
    nameForPrint += String(car_pull.car_name[i]);
  }
  tft.setCursor(28,64);
  tft.print(nameForPrint);


  //hazards
  tft.fillTriangle(0, 16, -18, 43, 18, 43, 0XFF60);
  //tft.fillCircle(0,34,18,0xFF60); 
  tft.setTextBackground(0xFF60);
  tft.setCursor(62,84);
  tft.setTextSize(1);
  tft.setTextColor(DISPLAY_BLACK);
  tft.print("!");
  tft.setTextSize(2);
  tft.setCursor(59,92);
  tft.print(car_pull.hazards);
}
