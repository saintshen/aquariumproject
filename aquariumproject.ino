#include <GravityTDS.h>

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
#define DTS_PIN A2
#define TURBIDITY_PIN A4
#define PH_PIN A5

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;

int xPosition = 0;
int yPosition = 0;
int screen = 0;
volatile byte switchClick = 0;
volatile bool lcdDisplay = true;

float humidity = 0;
float temperature = 0;
float waterTemperature = 0;
float waterPH = 0;
float tdsValue = 0;
float turbidityValue = 0;

int mapX = 0;
int mapY = 0;

float RawHigh = 100.0;
float RawLow = 1.999;
float ReferenceHigh = 99.9;
float ReferenceLow = 0;
float RawRange = RawHigh - RawLow;
float ReferenceRange = ReferenceHigh - ReferenceLow;

float calibration_ph = 21.34;
int phval = 0;
unsigned long int avgValue;
int buffer_arr[10],temp;


LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

DHT dht(DHTPIN, DHTTYPE);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
GravityTDS gravityTds;

void setup() {
  pinMode(VRX_PIN, INPUT);
  pinMode(VRY_PIN, INPUT);
  pinMode(SW_PIN, INPUT_PULLUP);
  pinMode(TURBIDITY_PIN, INPUT);
  pinMode(PH_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(SW_PIN), toggleDisplay, RISING);

  lcd.begin(16, 2);
  analogWrite(LCD_BACKLIGHT_PIN, 100);
  analogWrite(LCD_CONTRAST_PIN, 50);
  dht.begin();
  sensors.begin();
  gravityTds.setPin(DTS_PIN);
  gravityTds.setAref(5.0);
  gravityTds.setAdcRange(1024);
  gravityTds.begin();
  Serial.begin(115200);
}

void loop() {
  // get environment humidity and temperature
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  // get water temperature
  sensors.requestTemperatures();
  waterTemperature = (((sensors.getTempCByIndex(0) - RawLow) * ReferenceRange) / RawRange) + ReferenceLow;;

  // get TDS
  gravityTds.setTemperature(waterTemperature);
  gravityTds.update();
  tdsValue = gravityTds.getTdsValue();

  // get Turbidity
  turbidityValue = analogRead(TURBIDITY_PIN) / 1024.0 * 5.0;

  // get PH value
  for(int i=0;i<10;i++)       //Get 10 sample value from the sensor for smooth the value
  { 
    buffer_arr[i]=analogRead(PH_PIN);
    delay(10);
  }
  for(int i=0;i<9;i++)        //sort the analog from small to large
  {
    for(int j=i+1;j<10;j++)
    {
      if(buffer_arr[i]>buffer_arr[j])
      {
        temp=buffer_arr[i];
        buffer_arr[i]=buffer_arr[j];
        buffer_arr[j]=temp;
      }
    }
  }
  avgValue=0;
  for(int i=2;i<8;i++)                      //take the average value of 6 center sample
    avgValue+=buffer_arr[i];
  float volt=(float)avgValue*5.0/1024/6; //convert the analog into millivolt
  waterPH=-5.70 * volt + calibration_ph;     

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

  if (mapY > 0) {
    screen++;
  }

  switch (screen % 3) {
    case 0:
      lcd.setCursor(0, 0);
      lcd.print("Humidity :" + String(humidity, 2));
      lcd.setCursor(0, 1);
      lcd.print("TEMP     :" + String(temperature, 2));
      break;
    case 1:
      lcd.setCursor(0, 0);
      lcd.print("WaterTemp:" + String(waterTemperature, 2));
      lcd.setCursor(0, 1);
      lcd.print("Water TDS:" + String(tdsValue, 2));
      break;
    case 2:
      lcd.setCursor(0, 0);
      lcd.print("Turbidity:" + String(turbidityValue, 2));
      lcd.setCursor(0, 1);
      lcd.print("Water  PH:" + String(waterPH, 2));
      break;
    default:
      lcd.setCursor(0, 0);
      lcd.print("Turbidity:" + String(humidity, 2));
      lcd.setCursor(0, 1);
      lcd.print("TEMP    : " + String(temperature, 2));
      break;
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
