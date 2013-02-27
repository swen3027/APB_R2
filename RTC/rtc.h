 typedef struct{
         uint8_t seconds;
         uint8_t minutes;
         uint8_t hours;
         uint8_t DOM;
         uint8_t DOW;
         uint16_t DOY;
         uint8_t month;
         uint16_t year;
 
} RTC_time;

int setRTC(uint8_t, uint8_t, uint8_t, uint8_t,uint8_t, uint16_t, uint8_t, uint16_t);

RTC_time readRTC(); 

