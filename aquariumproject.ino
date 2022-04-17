#include <LiquidCrystal.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define LCD_CONTRAST_PIN 6
#define LCD_BACKLIGHT_PIN 10
#define DHTPIN 8
#define DHTTYPE DHT11
#define ONE_WIRE_BUS A3
#define VRX_PIN A0
#define VRY_PIN A1
#define SW_PIN 18

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

DHT dht(DHTPIN, DHTTYPE);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

int xPosition = 0;
int yPosition = 0;
int screen = 0;
volatile byte switchClick = 0;
volatile bool lcdDisplay = true;

float humidity = 0;
float temperature = 0;
float waterTemperature = 0;
float waterPH = 0;

int mapX = 0;
int mapY = 0;

float RawHigh = 99.6;
float RawLow = 1.133;
float ReferenceHigh = 99.9;
float ReferenceLow = -0.5;
float RawRange = RawHigh - RawLow;
float ReferenceRange = ReferenceHigh - ReferenceLow;



void setup() {
  pinMode(VRX_PIN, INPUT);
  pinMode(VRY_PIN, INPUT);
  pinMode(SW_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(SW_PIN), toggleDisplay, RISING);

  lcd.begin(16, 2);
  analogWrite(LCD_BACKLIGHT_PIN, 100);
  analogWrite(LCD_CONTRAST_PIN, 50);
  dht.begin();
  sensors.begin();
  Serial.begin(9600);
}

void loop() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  sensors.requestTemperatures();

  waterTemperature = sensors.getTempCByIndex(0);
  
  if (lcdDisplay) {
    lcd.display();
    analogWrite(LCD_BACKLIGHT_PIN, 100);
  } else {
    lcd.noDisplay();
    analogWrite(LCD_BACKLIGHT_PIN, 0);
  }
  

  xPosition = analogRead(VRX_PIN);
  yPosition = analogRead(VRY_PIN);

  mapX = map(xPosition, 0, 1023, -512, 512);
  mapY = map(yPosition, 0, 1023, -512, 512);

  if (mapY>0){
    screen++;
  }

  if (screen % 2) {
    lcd.setCursor(0, 0);
    lcd.print("Humidity: " + String(humidity, 2));
    lcd.setCursor(0, 1);
    lcd.print("TEMP    : " + String(temperature, 2));
  }
  else {
    lcd.setCursor(0, 0);
    lcd.print("WaterTemp:" + String(waterTemperature, 2));
    lcd.setCursor(0, 1);
    lcd.print("Water PH :" + String(waterPH, 2));
  }
  
  
  Serial.print("swtch clicked: " + String(switchClick));  
  Serial.println(" | Display: " + String(lcdDisplay));
  /*
    Serial.print("X: ");
    Serial.print(mapX);
    Serial.print(" | Y: ");
    Serial.print(mapY);
    Serial.println(SW_state);
    Serial.println(SW_before);
  */


}

void toggleDisplay() {
  switchClick++;
  if (switchClick % 2) {
    lcdDisplay = !lcdDisplay;
  }
}
