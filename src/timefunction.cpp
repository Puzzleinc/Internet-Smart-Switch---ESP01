#include <NTPClient.h>
#include "timefunction.h"

void clockCounter(unsigned long currentMillis, unsigned long &previousMillis, uint8_t &seconds, uint8_t &minutes, uint8_t &hours, uint16_t &days) {
    seconds = currentMillis - previousMillis;

    //the number of seconds that have passed since the last time 60 seconds was reached.
    if (seconds >= 60) {
    previousMillis = currentMillis;
    seconds = 0;
    minutes = minutes + 1; 
    }

    //if one minute has passed, start counting milliseconds from zero again and add one minute to the clock.
    if (minutes >= 60){
    minutes = 0;
    hours = hours + 1; 
    }

    if (hours == 24){
    hours = 0;
    days = days + 1; 
    }
}

void showTime(unsigned long currentMillis, unsigned long &timeLast , uint8_t const intervalSec, uint8_t secIndicator) {
    if (currentMillis - timeLast >= intervalSec) {
        timeLast = currentMillis;

        // Indikator waktu tiap detik led menyala
        digitalWrite(secIndicator, HIGH);
        delay(50);
        digitalWrite(secIndicator, LOW);
        delay(50);

        // tampilkan waktu saat ini ke serial monitor
        // timeClient.update();
        // Serial.print("Waktu internal: ");
        // Serial.print(days);
        // Serial.print(":");
        // Serial.print(hours);
        // Serial.print(":");
        // Serial.print(minutes);
        // Serial.print(":");
        // Serial.println(seconds);
        // Serial.print(" -- ");
        // Serial.println("Waktu internet : " + timeClient.getFormattedTime());
    }
}