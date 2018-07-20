#include <Wire.h>
#include <RTClib.h>                       // RTC = Real Time Clock

const int LDR_Pin = A0;                   // This will measure the Voltage at The Pin A0
const int DC_Water_Pump_Pin = 7;          // This will take the Value of the voltage and pass it to the Pump
const int Soil_Moisture_Sensor_Pin = A1;  // This will measure the value from the Soil Moisture Sensor
const int Relay = 8;

const int wateringTime = 1;              // 10 minutes of Watering Time
const int threshold_LDR_Value = 300;
const int LDR_Maximum_Value = 100;        // 100 K Ohm
const int threshold_Moisture = 20;
bool wateredToday = false;

DateTime current_Time;
RTC_DS1307 rtc;

int LDR_Value = 0;
bool Output_Pump = 0;
float moisture = 0;
float raw_moisture = 0;

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);

  Wire.begin();
  pinMode(DC_Water_Pump_Pin, OUTPUT);
  pinMode(Relay, OUTPUT);
  
  //Set the time and date on the real time clock if necessary
  if (! rtc.isrunning()) {
    // following line sets the RTC to the date & time
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  

  current_Time = rtc.now();
}

void loop() {
  digitalWrite( Relay , HIGH);
  //Reset wateredToday variable if it's a new day
  if ( ! ( current_Time.day() == rtc.now().day() ) )
    wateredToday = false;

  current_Time = rtc.now();

  // Format : "Day/Month/Year  Hours:Minutes:Seconds"
  Serial.print( current_Time.day() , DEC );
  Serial.print( "/" );
  Serial.print( current_Time.month() , DEC );
  Serial.print( "/" );
  Serial.print( current_Time.year() , DEC );
  Serial.print( "\t " );
  Serial.print( current_Time.hour() , DEC );
  Serial.print( ":" );
  Serial.print( current_Time.minute() , DEC );
  Serial.print( ":" );
  Serial.print( current_Time.second() , DEC );

  // read the analog in value from the LDR:
  LDR_Value = analogRead( LDR_Pin );
  raw_moisture = analogRead(Soil_Moisture_Sensor_Pin) * (3.3 / 1024);
  
  /*
    Soil Moisture Reference
    Air = 0%
    Really dry soil = 10%
    Probably as low as you'd want = 20%
    Well watered = 50%
    Cup of water = 100%
  */
  
  if (raw_moisture < 1.1) {
    moisture = (10 * raw_moisture) - 1;
  }
  else if (raw_moisture < 1.3) {
    moisture = (25 * raw_moisture) - 17.5;
  }
  else if (raw_moisture < 1.82) {
    moisture = (48.08 * raw_moisture) - 47.5;
  }
  else if (raw_moisture < 2.2) {
    moisture = (26.32 * raw_moisture) - 7.89;
  }
  else {
    moisture = (62.5 * raw_moisture) - 87.5;
  }

  moisture = (moisture * 100) / 50;
  // Start watering at 11 o'clock if the Light intensity is over the threshold
  if ( ( LDR_Value > threshold_LDR_Value )){ // && (moisture < 40 && moisture > 10) ) { // && (current_Time.hour() >= 15) ) {
    digitalWrite(Relay , LOW);
    wateredToday = true;

    // print the results to the serial monitor:
    Serial.print("\t LDR Value = ");
    Serial.print(LDR_Value);
    Serial.print("\t Output = ");
    Serial.print(wateredToday);
    Serial.print("\t Moisture = ");
    Serial.println(moisture);

    delay( wateringTime * 6000 );
    digitalWrite( Relay , HIGH);
  }
  // print the results to the serial monitor:
  Serial.print("\t LDR Value = ");
  Serial.print(LDR_Value);
  Serial.print("\t Output = ");
  Serial.print(wateredToday);
  Serial.print("\t Moisture = ");
  Serial.println(moisture);

  delay(2);                                  // 2 miliseconds
}
