#include "Arduino.h"
#include "Car.h"

Car::Car(char* cname, uint8_t m_gear, uint8_t hand, uint8_t am0,uint8_t am1,uint8_t am2, uint8_t am3,uint8_t m_hull)
{
   
    car_name = new char[12];
    strcpy(car_name, cname);
    hazards = 0;
    max_gear = m_gear;
    curr_gear = 1;
    handling = hand;
    ammo[0] = am0;
    ammo[1] = am1;
    ammo[2] = am2;
    ammo[3] = am3;
    //ammo[4] = {am[0],am[1],am[2],am[3]};
    max_hull = m_hull;
    curr_hull = m_hull;
    
}
