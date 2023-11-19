 #include <Wire.h> 
#include <Adafruit_MLX90614.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

SFE_UBLOX_GNSS myGNSS;

int DEBUG_LED_pin = 13;

#define RXD2 16
#define TXD2 17


int lat_size;
int lon_size;
int tem_size;

//---------------------------- Accelerometer----------------------------------------------
const int xPin = 25; // Analog pin for X-axis
float xScale = 0.01;  // Calibration factor for X-axis
float v_oldx = 0;
float offset = 0;
float xAccel;
int xRaw;
String message;
int isRunning = 0;
//-------------------------------------------
int countDigits(long num)
{
  int count = 0;
  while(num)
  {
    num = num / 10;
    count++;
  }
  if(count==0)
  {
    count = 1;
  }
  return count;
}


void setup() {

  pinMode(DEBUG_LED_pin, OUTPUT);

  //----------- To check if auto boot
  digitalWrite(DEBUG_LED_pin,HIGH);
  delay (2000);
  digitalWrite(DEBUG_LED_pin,LOW);
  //------------
  
  
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  delay (15000);
  Serial2.println("AT");
  delay(10);
  while (Serial2.available()) {
      Serial.print(char(Serial2.read()));
  }

  Serial2.println("AT+CMQTTSTART");
  delay(10);
  while (Serial2.available()) {
      Serial.print(char(Serial2.read()));
  }
  delay (15000);

  Serial2.println("AT+CMQTTACCQ=0,\"petpulse#pet#1234\"");
  delay(10);
  while (Serial2.available()) {
      Serial.print(char(Serial2.read()));
  }
  
  Serial2.println("AT+CMQTTCONNECT=0,\"tcp://test.mosquitto.org:1883\",90,1");
  delay(10);
  while (Serial2.available()) {
      Serial.print(char(Serial2.read()));
  }
  delay (15000);

  Wire.begin();
  
  if (myGNSS.begin() == false) 
  {
    Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }

  myGNSS.setI2COutput(COM_TYPE_UBX); 
  myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); 

  if (!mlx.begin()) {
    Serial.println("Error connecting to MLX sensor. Check wiring.");
    while (1);
  };

}


void loop() {
  long latitude = myGNSS.getLatitude();
  long longitude = myGNSS.getLongitude();
  int temperature = mlx.readObjectTempC();
  for(int i = 0; i<30; i++)
  {
    xRaw = analogRead(xPin);
  xAccel = (xRaw - 512)*xScale;
  float Dvx =( xAccel) * ( 0.1);
  float Velocityx = v_oldx + Dvx;
  v_oldx = Dvx;
  String xVelStr = String(Velocityx*10);
  if(Velocityx>=3)
  {
    Serial.println("Running");
    isRunning = 1;
  }
  message =  "Xvel:" + xVelStr +"\n";
  Serial.println(message);
  delay(10);
  }
  
 
  
     Serial.println(latitude);
     Serial.println(longitude);
     Serial.println(temperature);
    if(latitude !=0)
    {
      digitalWrite(DEBUG_LED_pin,HIGH);
      
    }
    else
    {
      digitalWrite(DEBUG_LED_pin,LOW);
    }
// ########################LATTITUDE MQTT #########################
    Serial.print("------------Lat--------\n");
    Serial2.println("AT+CMQTTTOPIC=0,10");
    delay(10);
    Serial2.println("pet123-lat");
    delay(10);
    while (Serial2.available()) {
      Serial.print(char(Serial2.read()));
      }
    lat_size = countDigits(latitude);   
   Serial2.println("AT+CMQTTPAYLOAD=0,"+ String(lat_size));
    delay(10);
    Serial2.println(String(latitude));
    delay(10);
    while (Serial2.available()) {
      Serial.print(char(Serial2.read()));
      }
      
    Serial2.println("AT+CMQTTPUB=0,1,60");
    delay(10);
    while (Serial2.available()) {
      Serial.print(char(Serial2.read()));
      }
delay(1000);

// ########################LONGITUDE MQTT #########################
    Serial.print("------------Lon--------\n");
    Serial2.println("AT+CMQTTTOPIC=0,10");
    delay(10);
    Serial2.println("pet123-lon");
    delay(10);
    while (Serial2.available()) {
      Serial.print(char(Serial2.read()));
      }

   lon_size = countDigits(longitude);
   Serial2.println("AT+CMQTTPAYLOAD=0,"+String(lon_size));
    delay(10);
    Serial2.println(String(longitude));
    delay(10);
    while (Serial2.available()) {
      Serial.print(char(Serial2.read()));
      }
      
    Serial2.println("AT+CMQTTPUB=0,1,60");
    delay(10);
    while (Serial2.available()) {
      Serial.print(char(Serial2.read()));
      }
      
delay(1000);

// ########################TEMPERATURE MQTT #########################
    Serial.print("------------TEMP--------\n");
    Serial2.println("AT+CMQTTTOPIC=0,10");
    delay(10);
    Serial2.println("pet123-tem");
    delay(10);
    while (Serial2.available()) {
      Serial.print(char(Serial2.read()));
      }

   tem_size = countDigits(temperature);
   Serial2.println("AT+CMQTTPAYLOAD=0,"+String(tem_size));
    delay(10);
    Serial2.println(String(temperature));
    delay(10);
    while (Serial2.available()) {
      Serial.print(char(Serial2.read()));
      }
      
    Serial2.println("AT+CMQTTPUB=0,1,60");
    delay(10);
    while (Serial2.available()) {
      Serial.print(char(Serial2.read()));
      }
delay(1000);
// ########################PET MOTION MQTT #########################

    Serial.print("------------MOTION--------\n");
    Serial2.println("AT+CMQTTTOPIC=0,10");
    delay(10);
    Serial2.println("pet123-mot");
    delay(10);
    while (Serial2.available()) {
      Serial.print(char(Serial2.read()));
      }

    if(isRunning == 1)
    {
      Serial2.println("AT+CMQTTPAYLOAD=0,7");
      delay(10);
      Serial2.println("Running");
      delay(10);
      while (Serial2.available()) {
        Serial.print(char(Serial2.read()));
      }
      isRunning = 0;
    }
    else
    {
      Serial2.println("AT+CMQTTPAYLOAD=0,8");
      delay(10);
      Serial2.println("Standing");
      delay(10);
      while (Serial2.available()) {
        Serial.print(char(Serial2.read()));
      }
    }
    Serial2.println("AT+CMQTTPUB=0,1,60");
    delay(10);
    while (Serial2.available()) {
      Serial.print(char(Serial2.read()));
      }

// -----------------
    delay(2000);

}
