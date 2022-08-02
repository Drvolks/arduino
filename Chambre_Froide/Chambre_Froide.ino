#include <DHT.h>
#include <LiquidCrystal.h>

#define DHT11_INT_PIN 2
#define DHT11_EXT_PIN 3
#define DHTTYPE_INT DHT11
#define DHTTYPE_EXT DHT22
#define FAN_PIN 5
#define ECART_TEMP 2

DHT dhtext(DHT11_EXT_PIN, DHTTYPE_EXT);
DHT dhtint(DHT11_INT_PIN, DHTTYPE_INT);

int statutFan = 0;
float minutesEtat = 0;
float TEMPERATURE_FAN_INACTIVE = 8.0;
float TEMPERATURE_FAN_ACTIVE = 10.0;
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
  
  int humidite_int = dhtint.readHumidity();
  float temperature_int = dhtint.readTemperature();
  int humidite_ext = dhtext.readHumidity();
  float temperature_ext = dhtext.readTemperature();
  
  lcd.begin(16, 2);
  if(humidite_int == 0 && temperature_int == 0) {
   ecrire_lcd_erreur("INT");
   pinMode(DHT11_INT_PIN,INPUT);
  } else {
    ecrire_lcd("INT ", temperature_int, humidite_int);
  }
  lcd.setCursor(0, 1);
  ecrire_lcd("", temperature_ext, humidite_ext);
  
  // On arrete si la temperature interne est inferieure au minimum
  // ou que la temperature externe depasse la temperature interne
  if(temperature_int <= TEMPERATURE_FAN_INACTIVE || temperature_ext >= temperature_int) {
    if(statutFan == 1 ) {
       statutFan = 0; 
       minutesEtat = 0;
       digitalWrite(FAN_PIN, LOW); 
    } else {
       minutesEtat = minutesEtat + LECTURE_INTERVAL_MINUTE; 
    }
  } else {
    // On demarre si la temperature interne est supperieure a la limite supperieure
    // et que la temperature interne est supperieure a la temperature exterieure
    if(temperature_int >= TEMPERATURE_FAN_ACTIVE &&
       temperature_ext < temperature_int) {
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
  
  lcd.print(" ");
  lcd.print(minutesEtat, 0);
  if(statutFan == 1) {
    lcd.print("*");  
  }
  
  delay(pause);
}

void ecrire_lcd(String sensor, float temperature, int humidite) {
  lcd.print(sensor);
  lcd.print(temperature,1);
  lcd.print("c/");
  lcd.print(humidite);
  lcd.print("%");
}

void ecrire_lcd_erreur(String sensor) {
  lcd.print(sensor);
  lcd.print(" ERREUR");
}
