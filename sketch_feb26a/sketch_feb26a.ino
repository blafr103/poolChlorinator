

#include <NewPing.h>

#define TRIGGER_PIN  13                             // Arduino pin tied to trigger pin on the ultrasonic senso
#define ECHO_PIN     2                              // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 100                            // Maximum distance we want to ping for (in centimeters).
#define RELAY_PIN 5                                // the Arduino pin, which connects to the IN pin of relay, for the pump

float ppm_interval_inferieur;
float ppm_interval_superieur;
float chlore_necessaire;

//*****************************************************************
int fillWarn = 25;                                  //variable pour définir le pourcentage dont la lumière du niveau réservoir s'allume , un pourcentage (15%)
int dispenseDelay = 360000;                           //1hr delay
int poolVolume = 14366;                             //volume de la piscine en US gallon
float pumpRate = 0.160;                               //débit de la pompe (L/sec)
int reservoirFull = 19;                             //volume du reservoir quand il est remplit, (L)
int hauteur=20;                                     //hauteur du reservoir (cm)
//*****************************************************************

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

// fonction pour lire le capteur de ppm
//*************************TO DO********************
float getCapteurPpm()                               //maybe PPM, maybe PH, TBD************
{   
    float ph = 0;
    return ph;
}


// fonction pour calculer le chlore necessaire pour augmenter le ppm de le montant necessaire
float getChloreNecessaire()
{
  //faire le calcule en prennant la valuer de PH donnée par getCapteurPpm(), retourne une valeur en Oz  **********on assume sensor PH, et non PPM
    //float PH = getCapteurPpm();
    float PH = 6; //for test
    return (7.5 - PH)*(poolVolume)*0.002;      
}


// fonction pour allumer la lumiere sur le reservoir, quand il est moins qu'un certain niveau : pourcentage<fillWarn
void allumerLumiere(float pourcentage)
{
 float pourc = pourcentage;
    
 Serial.print("allumerLumiere");//test
 Serial.println("%");//test
    
    
 if(pourc<fillWarn) digitalWrite(7, HIGH); //turn light on if reservoir below certain level
 else digitalWrite(7, LOW);                      //turn light off if reservoir above certain level
}


// fonction pour allumer la pompe pour un temps calcule
//allume aussi une lumiere en meme temps que la pompe est active
void allumerPompe(float timeX)
{
  float timeP = timeX;
    
  Serial.print("PUMP ON"); //test
  Serial.println("%");//test
  
    
  digitalWrite(8, HIGH);                      //turn on light indicating pump is active
    
  digitalWrite(RELAY_PIN, HIGH);              // turn on pump for x milliseconds
  delay(timeP);
  digitalWrite(RELAY_PIN, LOW);               // turn off pump 
  
    
  digitalWrite(8, LOW);                       //turn off light indicating pump is off
  Serial.print("PUMP OFF"); //test
  Serial.println("%");//test
}



void setup() {
  pinMode(7,OUTPUT);                            //sets pin 7 as an ouput
  pinMode(8,OUTPUT);                            //sets pin 8 as an ouput
  pinMode(RELAY_PIN, OUTPUT);                  // initialize pump pin (5) as an output.
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
  
}


float pingRes(){
    
    
    Serial.print("pingRes"); //test
    Serial.println("%");//test
    
    float distance = sonar.ping_cm();                   // Send ping, get distance in cm and print result (0 = outside set distance range), distance entre detecteur et chlore
    float niveau = hauteur-distance;                    //cm de chlore dans le reservoir
    return (niveau/hauteur)*100;
}

void loop() {
    
    delay(50);                                          // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
    float pourcentage = pingRes();                        //check current reservoir level and save to variable
    delay(50); 
    
    Serial.print("Niveau de chlore: ");//test
    Serial.println(pourcentage);//test
    Serial.println("%");//test
    
    allumerLumiere(pourcentage);                        // checks reservoir level to see if the fill warning light should be turned on
    delay(50); 
    
    if(pourcentage > 10){  //satefy check to make sure there is chlorine in the system before running, to prevent running the pump dry
      
      chlore_necessaire = getChloreNecessaire()*0.0295735;     //calculate how much chlorine to pump, and conversion from Oz to L
      delay(50); 
        
  //******************************************if LESS than 200mL is needed for current injection, wait*****************************************      
      if (chlore_necessaire < 0.2) { //if the necessary injection amount is below 200mL, wait
        
        Serial.print("delay(1hr)");//test
        Serial.println("%");//test
          
          
        delay(dispenseDelay);                             // delai de une heure si le montant à ajouter est moins de 200mL
      }   
        
  //******************************************if MORE than 200mL is needed for current injection, proceed**************************************   
      if (chlore_necessaire > 0.2){ //if the necessary injection amount is above 200mL

        float chloreRestant =pourcentage*(reservoirFull)/100; //calculates the reservoir level in Litres
        delay(50); 
          
        Serial.print("chloreRestant: ");//test
        Serial.print(chloreRestant);//test
        Serial.println("%");//test
        Serial.print("chlore_necessaire: ");//test
        Serial.print(chlore_necessaire);//test
        Serial.println("%");//test

        if (chloreRestant > chlore_necessaire){           //checks if there is enough chlorine left in reservoir for current instance of injection
          float time = chlore_necessaire*1000/pumpRate;   //calculate how long pump turns on (ms)
          delay(50); 
          allumerPompe(time);                           //active la pompe for the calculated amount of time
          delay(50); 
        } 
      } 
    } 
//******************************************************************************************************************************************
}
