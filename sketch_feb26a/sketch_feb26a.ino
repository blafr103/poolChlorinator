

#include <NewPing.h>

#define TRIGGER_PIN  13                             // Arduino pin tied to trigger pin on the ultrasonic senso
#define ECHO_PIN     2                              // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 100                            // Maximum distance we want to ping for (in centimeters).
#define RELAY_PIN 5                                // the Arduino pin, which connects to the IN pin of relay, for the pump

float ppm_interval_inferieur;
float ppm_interval_superieur;
float chlore_necessaire;

//*****************************************************************
int fillWarn = 15;                                  //variable pour définir le pourcentage dont la lumière du niveau réservoir s'allume , un pourcentage (15%)
int dispenseDelay = 3600000;                        //1hr delay
int poolVolume = 14366;                             //volume de la piscine en US gallon
int pumpRate = 0.160;                               //débit de la pompe
int alerte_nbr_jours = 5;                           //nombre de jours que la cliente veut etre alerté avant que le réservoir est vide
int reservoirFull = 19;                             //volume du reservoir quand il est remplit, (L)
float dailyAV = 0.66666666666;                      //calcul aprox. de L de chlore utilisé par jours
int hauteur=44;                                     //hauteur du reservoir (cm)
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
  //faire le calcule en prennant la valuer de PH donnée par getCapteurPpm(), retourne une valeur en Oz  
  //**********on assume sensor PH, et non PPM
  int PH = getCapteurPpm();
  return (7.5 - PH)*(poolVolume)*0.002;
}


// fonction pour allumer la lumiere sur le reservoir, quand il est moins qu'un certain niveau : pourcentage<fillWarn
void allumerLumiere(int pourcentage)
{
 if(pourcentage<fillWarn) digitalWrite(7, HIGH);
 else digitalWrite(7, LOW);
}


// fonction pour allumer la pompe pour un temps calcule
//allume aussi une lumiere en meme temps que la pompe est active
void allumerPompe(int timeX)
{
  pinMode(RELAY_PIN, OUTPUT);                  // initialize digital pin as an output.

  digitalWrite(8, HIGH);                      //turn on light indicating pump is active
    
  digitalWrite(RELAY_PIN, HIGH);              // turn on pump for x milliseconds
  delay(timeX);
  digitalWrite(RELAY_PIN, LOW);               // turn off pump 
  
  digitalWrite(8, LOW);                       //turn off light indicating pump is off
}


//Nouveau code pour la fonction loop()
bool ensemble = false;

void loop(int pourcentage) {
  
  int pourcentage1 = pourcentage ; 
  
  while (true) {
    
    chlore_necessaire = getChloreNecessaire()*0.0295735;//conversion from Oz to L
    
      
      
      
    if (chlore_necessaire < 0.2) {

      delay(dispenseDelay);                             // delai de une heure si le montant à ajouter est moins de 200mL

      ensemble = true;
      break;

    }   
      
      
      
    
    if (chlore_necessaire > 0.2){

      float chloreRestant =pourcentage1*(reservoirFull)/100;
      
        
        
      if (chloreRestant > chlore_necessaire){ 
               
        int daysLeft = (chloreRestant/dailyAV) - 1;   //dailyAV to be more accurately defined, algo to calculate how many days worth of chlorine left
          
        if(daysLeft<=alerte_nbr_jours){
          allumerLumiere(pourcentage1);
        }
        
        int time = chlore_necessaire*1000/pumpRate;   //calculate how long pump turns on (ms)
         
        allumerPompe(time);                           //active la pompe
      } 
      
    }
       
                          // le break est important ici
      break;
  }
}


int main() {
  do {
    
    //*************code pour niveau de reservoir******************
    pinMode(7,OUTPUT);
    Serial.begin(115200);                       // Open serial monitor at 115200 baud to see ping results.
    delay(50);                                  // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
    float distance = sonar.ping_cm();                   // Send ping, get distance in cm and print result (0 = outside set distance range), distance entre detecteur et chlore
    float niveau = hauteur-distance;                    //cm de chlore dans le reservoir
    float pourcentage = (niveau/hauteur)*100;           //pourcentage de chlore dans reservoir
    //**************************************************************
   
    allumerLumiere(pourcentage); // checks reservoir level to see if the fill warning light should be turned on 
      
    if(pourcentage > 5){                                //if there is enough chlorine left in reservoir for current pump injection
      chlore_necessaire = getChloreNecessaire();
      ensemble = false;
      loop(pourcentage);
    } //if there is NOT enough chlorine left in reservoir for current pump injection, system will keep looping in the Do...While and continuously check reservoir until it is filled again
    
  } while (true);
}
