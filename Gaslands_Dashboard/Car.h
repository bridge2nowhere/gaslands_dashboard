/*
  CAr.h - Library for Gaslands cars.
  Created by Andrew Woodbridge, December 29, 2019.
  Released into the public domain.
*/

#ifndef Car_h
#define Car_h

#include "Arduino.h"



class Car{

  public:
    Car(char* cname, uint8_t m_gear, uint8_t hand, uint8_t am0,uint8_t am1,uint8_t am2, uint8_t am3,uint8_t m_hull, char* txt);
    char* car_name;
    uint8_t max_gear;
    uint8_t curr_gear;
    uint8_t handling;
    uint8_t ammo[4];
    uint8_t max_hull;
    uint8_t curr_hull;
    uint8_t hazards;
    bool canActivate;
    char* textBlock;
    
};

#endif
