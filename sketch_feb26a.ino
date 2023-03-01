##############################################################################################################################################
// Le code Arduino ci-dessous calcul la quantité de chlore en pourcentage à l’aide du capteur ultrason.
// #include <NewPing.h>


// #define TRIGGER_PIN  13  // Arduino pin tied to trigger pin on the ultrasonic senso
// #define ECHO_PIN     2  // Arduino pin tied to echo pin on the ultrasonic sensor.
// #define MAX_DISTANCE 100 // Maximum distance we want to ping for (in centimeters). 


// float distance;  //distance entre detecteur et chlore
// float niveau;   //cm de chlore dans le reservoir
// int hauteur=15; //hauteur du reservoir
// int pourcentage; //pourcentage de chlore dans reservoir


// NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.


// void setup() {
// //  pinMode(7,OUTPUT);
// //  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
// }
// void loop() {
//  delay(50);                     // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
//  Serial.print("Niveau de chlore: ");
//  distance = sonar.ping_cm();      // Send ping, get distance in cm and print result (0 = outside set distance range)
//  niveau = hauteur-distance;
//  pourcentage = (niveau/hauteur)*100;
//  Serial.print(pourcentage);
//  Serial.println("%");


//allumerLumiere(pourcentage);

/*
 if(pourcentage<20) digitalWrite(7, HIGH);
 else digitalWrite(7, LOW);
*/
##############################################################################################################################################
int alerte_nbr_jours;
float ppm_interval_inferieur;
float ppm_interval_superieur;
float niveau_chlore;
float moyenne_chlore_par_jour = xxx; //Il faut trouver la valeur predefinie
float chlore_necessaire;


int pourcentage; //pourcentage de chlore dans reservoir
float distance;  //distance entre detecteur et chlore
float niveau;   //cm de chlore dans le reservoir
int hauteur=15; //hauteur du reservoir
pourcentage = (niveau/hauteur)*100;
distance = sonar.ping_cm();      // Send ping, get distance in cm and print result (0 = outside set distance range)
niveau = hauteur-distance;

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
#include <NewPing.h>
#define TRIGGER_PIN  13  // Arduino pin tied to trigger pin on the ultrasonic senso
#define ECHO_PIN     2  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 100 // Maximum distance we want to ping for (in centimeters).

// Lis les valeurs du Analog pin specifie - Lis les valeurs de l'utilisateur 
  alerte_nbr_jours = analogRead(A1) // # de jour pour alerter l'utilisateur avant que le reservoir ne soit vide
  ppm_interval_inferieur = analogRead(A2) // interval du ppm de preference
  ppm_interval_superieur = analogRead(A3) // interval du ppm de preference


// fonction pour lire le capteur de ppm
int getCapteurPpm()
{
  // Cette fonction va permettre de lire la valeur du capteur ppm. Elle devra donc retourner un resultat en float.
}

// fonction pour calculer le chlore necessaire pour equilibrer le ppm
int getChloreNecessaire()
{
  // il faudra convertir en float
  // est ce qu'il vaut mieux pas utiliser le getNiveauChlore et voire le niveau necessaire en fonctin de getNiveauChlore ?
}

// fonction pour allumer la lumiere sur le reservoir
void allumerLumiere(int pourcentage)
{
 if(pourcentage<20) digitalWrite(7, HIGH);
 else digitalWrite(7, LOW);
 //Allume la lumiere pour combien de secondes ?
 //Moyen de definir combien de seconde on veut que la lumiere reste allume
}

// fonction pour allumer la pompe pour un temps calcule
void allumerPompe()
{

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


void loop() {

  getCapteurPpm();
  chlore_necessaire = getChloreNecessaire();
  boolean ensemble = true;
  while (ensemble == true){
  // peut etre il va falloir faire des variables pour les 200ml;ou garder 0.2 litres donc garder tout le programme en litre
  if (chlore_necessaire < 0.2)
  {
    delay(60UL * 60UL * 1000UL);  //delai de une heure : 60 minutes each of 60 seconds each of 1000 milliseconds all unsigned longs
    ensemble = false;
    break;
  }
  else{
    niveau_chlore = getNiveauChlore();
    if (niveau_chlore > chlore_necessaire){
      allumerPompe();
      // est ce qu'on mentionne le processus pour le nombre de litres restants
      if(>alerte_nbr_jours){
        delay(60UL * 60UL * 1000UL);
        ensemble = false;
        break
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
  }
}