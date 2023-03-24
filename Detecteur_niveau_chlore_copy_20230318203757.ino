

#include <NewPing.h>

#define TRIGGER_PIN  13  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     2  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 100 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.


float distance; //distance entre detecteur et chlore
float niveau;   //cm de chlore dans le reservoir
int hauteur=15; //hauteur du reservoir entré de l'utilisateur
int pourcentage; //pourcentage de chlore dans reservoir



NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

void setup() {
  pinMode(7,OUTPUT);
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
  
}

void loop() {
  delay(50);                     // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  Serial.print("Niveau de chlore: ");
  distance = sonar.ping_cm();      // Send ping, get distance in cm and print result (0 = outside set distance range)
  niveau = hauteur-distance;
  pourcentage = (niveau/hauteur)*100;
  Serial.print(pourcentage);
  Serial.println("%");

  if(pourcentage<20) digitalWrite(7, HIGH);
  else digitalWrite(7, LOW);
  
}