
int alerte_nbr_jours;
float ppm_interval_inferieur;
float ppm_interval_superieur;
float niveau_chlore;
float chlore_necessaire;


//*****************************************************************
int fillWarn = 15;  //variable pour définir le pourcentage dont la lumière du niveau réservoir s'allume
int dispenseDelay = 3600000; //1hr delay
int poolVolume = 14366; //volume de la piscine en US gallon
int pumpRate = 0; //débit de la pompe
//*****************************************************************


#include <NewPing.h>
#define TRIGGER_PIN  13  // Arduino pin tied to trigger pin on the ultrasonic senso
#define ECHO_PIN     2  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 100 // Maximum distance we want to ping for (in centimeters).
#define RELAY_PIN A5 // the Arduino pin, which connects to the IN pin of relay, for the pump

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.


int hauteur=15; //hauteur du reservoir
float distance = sonar.ping_cm();      // Send ping, get distance in cm and print result (0 = outside set distance range), distance entre detecteur et chlore
float niveau = hauteur-distance;//cm de chlore dans le reservoir
float pourcentage = (niveau/hauteur)*100; //pourcentage de chlore dans reservoir

pinMode(RELAY_PIN, OUTPUT);// initialize digital pin A5 as an output.

// Lis les valeurs du Analog pin specifie - Lis les valeurs de l'utilisateur 
  alerte_nbr_jours = analogRead(A1) // # de jour pour alerter l'utilisateur avant que le reservoir ne soit vide
  ppm_interval_inferieur = analogRead(A2) // interval du ppm de preference
  ppm_interval_superieur = analogRead(A3) // interval du ppm de preference


// fonction pour lire le capteur de ppm
float getCapteurPpm() //maybe PPM, maybe PH, TBD************
{
  // Cette fonction va permettre de lire la valeur du capteur ppm. Elle devra donc retourner un resultat en float.
    
    return ph;
}

// fonction pour calculer le chlore necessaire pour augmenter le ppm de le montant necessaire
float getChloreNecessaire()
{
  // il faudra convertir en float
  //faire le calcule en prennant la valuer de PH/PPM donnée par getCapteurPpm()
  //retourne une valeur en OZ
  
  int PH = getCapteurPpm();
  return (7.5 - PH)*(poolVolume)*0.002;
}

// fonction pour allumer la lumiere sur le reservoir
void allumerLumiere(int pourcentage)
{
 if(pourcentage<fillWarn) digitalWrite(7, HIGH);
 else digitalWrite(7, LOW);
 //Allume la lumiere pour combien de secondes ?
 //Moyen de definir combien de seconde on veut que la lumiere reste allume
}


// fonction pour allumer la pompe pour un temps calcule
//allume aussi une lumiere en meme temps que la pompe est active
void allumerPompe(int x)
{
  digitalWrite(8, HIGH); //turn on light indicating pump is active
    
    
  digitalWrite(RELAY_PIN, HIGH); // turn on pump for x milliseconds
  delay(x);
  digitalWrite(RELAY_PIN, LOW);  // turn off pump 
  
  digitalWrite(8, LOW); //turn off light indicating pump is off
}

// fonction pour detecter le niveau de chlore dans le reservoir
int getNiveauChlore()
{
  pinMode(7,OUTPUT);
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
  delay(50);                     // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  Serial.print("Niveau de chlore: ");
  Serial.print(pourcentage);
  Serial.println("%");
  // il faudra convertir en float ou int ou garder
}


//Nouveau code pour la fonction loop()
bool ensemble = false;

void loop() {
  
  while (true) {
    
    chlore_necessaire = getChloreNecessaire()*0.0295735;//conversion from Oz to L
    
    if (chlore_necessaire < 0.2) {

      delay(dispenseDelay);// delai de une heure si le montant à ajouter est moins de 200mL

      ensemble = true;
      break;

    }
    
    if (chlore_necessaire > 0.2){

      niveau_chlore = getNiveauChlore();
      
      if (niveau_chlore > chlore_necessaire){
        
        int time = chlore_necessaire*1000/pumpRate;//calculate how long pump turns on (ms)
        allumerPompe(time);
        
        
        
        // est ce qu'on mentionne le processus pour le nombre de litres restants
        if(>alerte_nbr_jours){
          delay(dispenseDelay);
          ensemble = true;
          break;
        }
        
        if(<alerte_nbr_jours){
          allumerLumiere();
           //allumer la lumiere et apres il faut arreter le programmes (break) ou recommencer toute la boucle ?
        }
      }
      if (niveau_chlore < chlore_necessaire){
        allumerLumiere();
        //allumer la lumiere et apres il faut arreter le programme (break) ou recommencer toute la boucle ?
        // ou ca peut etre inclus dans la fonction allumerLumiere()
      }
    }

    
    // le break est important ici
      break;
    }
  }
}

int main() {
  do {
    chlore_necessaire = getChloreNecessaire();
    ensemble = false;
    loop();
  } while (ensemble);
}
