// Include all needed library first
// This sketch has Firebase ESP8266, DHT Master, and Adafruit Unified Sensor Library dependencies
// So before implement this sketch to the ESP8266 please install all library above

#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include <DHT.h>

//Then we define Firebase data base url, Firebase API Key, Wifi SSID, and Wifi Password

#define FIREBASE_HOST "***********************************" //Insert URL without http:// for example: first-cloud-default-rtdb.firebasedatabase.app 
#define FIREBASE_AUTH "***********************************" //Insert Firebase API key
#define WIFI_SSID "************" //Insert your wifi ssid and pass, note that if your wifi ssid and pass is changed, you also need to changed this
#define WIFI_PASSWORD "********"

//Then we define the pin for sensor and actuators.
//Check your board GP/IO pin datasheet so you can adjust the code accordingly.

#define DHTPIN 2
int relay_1 = D5; //relay_1 will be temperature final control element which is 12V Peltier
int relay_2 = D7; //relay_2 will be relative humidity final control element which is 5V Humidifier

//Define DHT Type that is used
#define DHTTYPE    DHT11
DHT dht(DHTPIN, DHTTYPE);

//Define FirebaseESP8266 data object
FirebaseData firebaseData;
FirebaseData ledData;
FirebaseJson json;

void setup() {
  
  // Setup pin, wifi connection, and firebase connection.
  
  // If you use ESP8266 used 115200 serial communication rate. Adjust accordingly
  Serial.begin(115200);
  dht.begin();
  pinMode(relay_1,OUTPUT);
  pinMode(relay_2, OUTPUT);

  //Setting up the wifi connection
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  
  //Setting Up Firebase connection
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}

//Creating sensor update function.

void sensorUpdate(){
  // DHT11 has really slow sampling rate. So the reading interval can be up to 2 seconds.
  float h = dht.readHumidity();
  // Read temperature as Celsius (default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  
  //Print the readings on serial monitor.
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C  ,"));
  Serial.print(f);
  Serial.println(F("°F  "));

  //Push read data to firebase
  if (Firebase.setFloat(firebaseData, "/FirebaseIOT/temperature", t))
  {
    Serial.println("PASSED");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
    Serial.println("ETag: " + firebaseData.ETag());
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }

  if (Firebase.setFloat(firebaseData, "/FirebaseIOT/humidity", h))
  {
    Serial.println("PASSED");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
    Serial.println("ETag: " + firebaseData.ETag());
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
}
void loop() {
  //Call sensorUpdate function
  sensorUpdate();

  //Add bang-bang controller automation.
  //There will be 4 conditions in this scenarion since we're only use one controller for the system.
  //First we're going to state the Setpoint, in this case we're going to set treshold for each parameter.
  //We're not setting the lower SP because in our environment it particulary hot and dry.
  //We're also going to use hysterisis SP so actuator will not always work. It will help save up the electricity bill.
  float UpperSPtrehold_t = 32;
  float LowerSPtreshold_t = 27;
  float UpperSPtreshold_h = 0.8;
  float LowerSPtreshold_h = 0.62;
  int isActuatorON_t = 0;
  int isActuatorON_h = 0;
  
  //First condition is when temperature is too hot and relative humidity is too low
  //Then the both actuators will go on
  if (t >= UpperSPtreshold_t || h <= LowerSPtreshold_h || isActuatorON_t ==0 || isActuatorON_h == 0 )
  {
    digitalWrite(relay_1, HIGH);
    digitalWrite(relay_2, HIGH);
    isActuatorON_t = 1;
    isActuatorON_h = 1;
  }
  //Second condition is when temperature is too low and relative humidity is too high
  else if(t <= LowerSPtreshold_t || h >= UpperSPtreshold_h || isActuatorON_t == 1 || isActuatorON_h == 0)
  {
    digitalWrite(relay_1, LOW);
    digitalWrite(relay_2, LOW);
    //Updating actuator information so we can use hysterisis setpoint mode and save up energy.
    if( t >=UpperSPtreshold_t || h <= LowerSPtreshold_h)
    {
      isActuatorON_t = 1;
      isActuatorON_h = 1;
    }
    else if(t >=UpperSPtreshold_t || h >= LowerSPtreshold_h)
    {
      isActuatorON_t = 1;
      isActuatorON_h = 0;
    }
    else if(t >=UpperSPtreshold_t || h >= LowerSPtreshold_h)
    {
      isActuatorON_t = 0;
      isActuatorON_h = 1;
    }
    else if(t <=UpperSPtreshold_t || h >= LowerSPtreshold_h)
    {
      isActuatorON_t = 0;
      isActuatorON_h = 0;
    }
  }
  //Third condition is when temperature is too high, but relative humidity still in optimal range
  else if(t >= UpperSPtreshold_t || h >=LowerSPtreshold_h || h <= UpperSPtreshold_h || isActuatorON_t == 0)
  {
    digitalWrite(relay_1,HIGH);
    digitalWrite(relay_2,LOW);
    
    //We still need to update actuator condition
    if( t >=UpperSPtreshold_t || h <= LowerSPtreshold_h)
    {
      isActuatorON_t = 1;
      isActuatorON_h = 1;
    }
    else if(t >=UpperSPtreshold_t || h >= LowerSPtreshold_h)
    {
      isActuatorON_t = 1;
      isActuatorON_h = 0;
    }
    else if(t >=UpperSPtreshold_t || h >= LowerSPtreshold_h)
    {
      isActuatorON_t = 0;
      isActuatorON_h = 1;
    }
    else if(t <=UpperSPtreshold_t || h >= LowerSPtreshold_h)
    {
      isActuatorON_t = 0;
      isActuatorON_h = 0;
    }
  }
  //Last condition is when temperature is in optimal range, but relative humidity is too dry
  else if(h <= LowerSPtreshold_h || t >=LowerSPtreshold_t || t <= UpperSPtreshold_t || isActuatorON_h == 0)
  {
    digitalWrite(relay_1,LOW);
    digitalWrite(relay_2,HIGH);

    //Updating actuator condition
    if( t >=UpperSPtreshold_t || h <= LowerSPtreshold_h)
    {
      isActuatorON_t = 1;
      isActuatorON_h = 1;
    }
    else if(t >=UpperSPtreshold_t || h >= LowerSPtreshold_h)
    {
      isActuatorON_t = 1;
      isActuatorON_h = 0;
    }
    else if(t >=UpperSPtreshold_t || h >= LowerSPtreshold_h)
    {
      isActuatorON_t = 0;
      isActuatorON_h = 1;
    }
    else if(t <=UpperSPtreshold_t || h >= LowerSPtreshold_h)
    {
      isActuatorON_t = 0;
      isActuatorON_h = 0;
    }
  }
}
