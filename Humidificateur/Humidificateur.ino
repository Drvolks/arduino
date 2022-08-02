#include <DHT.h>
#include <LiquidCrystal.h>

#define DHT11_PIN 3
#define DHTTYPE DHT22
#define FAN_PIN 5

DHT dhtext(DHT11_PIN, DHTTYPE);

int statutFan = 0;
float minutesEtat = 0;
float HUMIDITE_FAN_INACTIVE = 32;
float HUMIDITE_FAN_ACTIVE = 27;
float LECTURE_INTERVAL_MINUTE = 0.1666;

// 7 - 11
// 8 - 13
// 9 - 18
// 10 - 19
// 11 - 20
// 12 - 21
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

void setup() {
  Serial.begin(9600);
  pinMode(FAN_PIN, OUTPUT); 
}

void loop() {
  int pause = LECTURE_INTERVAL_MINUTE * 60;
  pause = pause * 1000;
  
  int humidite = dhtext.readHumidity();
  float temperature = dhtext.readTemperature();

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print("c  ");
  Serial.print("Humidite: ");
  Serial.print(humidite);
  Serial.print("%");
  Serial.print("   Statut ");

  lcd.begin(16, 2);
  ecrire_lcd("Temp: ", temperature, "c");
  lcd.setCursor(0, 1);
  ecrire_lcd("Humidite: ", humidite, "%");
  
  if(statutFan == 0) {
      Serial.print("Inactif");
  } else {
      Serial.print("Actif");
  }
  Serial.print(" depuis ");
  Serial.print(minutesEtat,0);
  Serial.print(" min\n");
  
  // On arrete si l'humidité est suppérieure au max
  if(humidite >= HUMIDITE_FAN_INACTIVE) {
    if(statutFan == 1 ) {
       statutFan = 0; 
       minutesEtat = 0;
       digitalWrite(FAN_PIN, LOW); 
    } else {
       minutesEtat = minutesEtat + LECTURE_INTERVAL_MINUTE; 
    }
  } else {
    // On demarre si l'humidité est inférieure au min
    if(humidite <= HUMIDITE_FAN_ACTIVE) {
      if(statutFan == 0) {
         statutFan = 1;
         minutesEtat = 0;
        digitalWrite(FAN_PIN, HIGH); 
      } else {
        // deja actif
        minutesEtat = minutesEtat + LECTURE_INTERVAL_MINUTE;
      }
    }
  }
  
  delay(pause);
}

void ecrire_lcd(String sensor, float value, String unit) {
  lcd.print(sensor);
  lcd.print(value,1);
  lcd.print(unit);
}
