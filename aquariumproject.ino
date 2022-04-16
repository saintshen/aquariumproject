#include <AxisJoystick.h>
#include <Joystick.h>
#include <LiquidCrystal.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define LCD_CONTRAST_PIN 6
#define LCD_BACKLIGHT_PIN 10
#define DHTPIN 8
#define DHTTYPE DHT11
#define ONE_WIRE_BUS A3
#define SW_PIN 13
#define VRX_PIN A0
#define VRY_PIN A1

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
Joystick* joystick;

DHT dht(DHTPIN, DHTTYPE);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

int lcdDisplay = 1;


int xPosition = 0;
int yPosition = 0;
int SW_state = 0;
int SW_before = 0;

int mapX = 0;
int mapY = 0;

float RawHigh = 99.6;
float RawLow = 1.133;
float ReferenceHigh = 99.9;
float ReferenceLow = -0.5;
float RawRange = RawHigh - RawLow;
float ReferenceRange = ReferenceHigh - ReferenceLow;



void setup() {

  Serial.begin(9600);
  dht.begin();
  lcd.begin(16, 2);
  analogWrite(LCD_BACKLIGHT_PIN, 100);
  analogWrite(LCD_CONTRAST_PIN, 50);

  joystick = new AxisJoystick(SW_PIN, VRX_PIN, VRY_PIN);

  sensors.begin();
}

void loop() {

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  sensors.requestTemperatures();

  float waterT = sensors.getTempCByIndex(0);

  SW_before = SW_state;
  SW_state = joystick->isPress();
  toggleDisplay();

  lcd.setCursor(0, 0);
  lcd.print("Humidity: " + String(h, 2));
  lcd.setCursor(0, 1);
  lcd.print("Temp: " + String(waterT, 2));


  /*
  Serial.print("X: ");
  Serial.print(mapX);
  Serial.print(" | Y: ");
  Serial.print(mapY);
  Serial.println(" | Press: " + String(joystick->isPress()));
  Serial.println(SW_state);
  Serial.println(SW_before); 
  */


}

void toggleDisplay() {
  if (SW_before == 1 && SW_state == 0) {
    if (lcdDisplay == 0) {
      lcd.display();
      analogWrite(LCD_BACKLIGHT_PIN, 100);
      lcdDisplay = 1;
    } else {
      lcd.noDisplay();
      analogWrite(LCD_BACKLIGHT_PIN, 0);
      lcdDisplay = 0;
    }
  }
}