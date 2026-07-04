
#include <NewPing.h>

#define TRIGGER_PIN  13        // Arduino pin tied to trigger pin on the ultrasonic senso
#define ECHO_PIN     2         // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 100       // Maximum distance we want to ping for (in centimeters).

#define RELAY_PIN 5            // relay IN, pump
#define ANALOGIN  A0           // pH sensor pin
#define LED_LOW   7            // blue:   reservoir low  (< fillWarn)
#define LED_PUMP  8            // red:    dispensing active
#define LED_HIGH  4            // orange: pool chlorine above target

float chlore_necessaire;

//******************************CONSTANTES***********************************
int fillWarn = 25;                                  //variable pour définir le pourcentage dont la lumière du niveau réservoir s'allume , un pourcentage (25%)
unsigned long dispenseDelay = 3600000UL;            //1 hr in ms **CORRECTED - int is 16-bit signed (max 32767). 360000 overflows and wraps to 32320, so "1 hr delay" is actually ~32 seconds
int poolVolume = 14366;                             //volume de la piscine en US gallon
float pumpRate = 0.160;                             //débit de la pompe (L/sec)
int reservoirFull = 19;                             //volume du reservoir quand il est remplit, (L)
int hauteur=20;                                     //hauteur du reservoir (cm)
float calibration_value = 0.0;                      //to calibrate PH sensor offset, placeholder
float highMargin = 0.5;                             //litres over-target before orange "high" alert (placeholder value)
unsigned long lastCycle = 0;
//***************************************************************************

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

// fonction pour lire le capteur de ppm
//*************************INCOMPLETE/TO BE DETERMINED********************
float getCapteurPpm()                               
{   
    float temp = 0;
    for(int i=0;i<10;i++){                 //take 10 sample readings
        temp += analogRead(ANALOGIN);      // was analogRead(A0)
        delay(30);
    }
    float avg = temp/10;                   //take the average of these 10 samples
    float volt = avg * 5.0/1024;           //ADC counts to volts
    float ph = -5.70 * volt + calibration_value;
    return ph;
}

// fonction pour calculer le chlore necessaire pour augmenter le ppm de le montant necessaire
// faire le calcule en prennant la valuer de PH donnée par getCapteurPpm(), retourne une valeur en Oz  **********on assume sensor PH, et non PPM
float getChloreNecessaire()
{
    //float PH = 6; //for test
    float PH = getCapteurPpm();
    return (7.5 - PH)*(poolVolume)*0.002;      
}

// fonction pour allumer la lumiere sur le reservoir, quand il est moins qu'un certain niveau : pourcentage<fillWarn
void allumerLumiere(float pourcentage)
{
    //Serial.print("allumerLumiere");//test
    if(pourcentage<fillWarn){
        digitalWrite(LED_LOW, HIGH);     //turn light on if reservoir below certain level
    }else{
        digitalWrite(LED_LOW, LOW);      //turn light off if reservoir above certain level
    }
}

//fonction pour allumer la pompe pour un temps calcule, allume aussi une lumiere en meme temps que la pompe est active
void allumerPompe(float timeX)
{
    //Serial.print("PUMP ON"); //test
    digitalWrite(LED_PUMP, HIGH);          // was digitalWrite(8, HIGH), turn on light indicating pump is active
    digitalWrite(RELAY_PIN, HIGH);         // turn on pump for x milliseconds
    delay(timeX);
    digitalWrite(RELAY_PIN, LOW);          // turn off pump 
    digitalWrite(LED_PUMP, LOW);           // was digitalWrite(8, LOW), turn off light indicating pump is off
    //Serial.print("PUMP OFF"); //test
}

//***************************************************************************
//********************************    SETUP   ********************************
//***************************************************************************
void setup() 
{
    pinMode(LED_LOW,  OUTPUT);              //sets pin 7 as an ouput
    pinMode(LED_PUMP, OUTPUT);              //sets pin 8 as an ouput
    pinMode(LED_HIGH, OUTPUT);              //sets pin 4 as an ouput
    pinMode(RELAY_PIN, OUTPUT);             // initialize pump pin (5) as an output.
    Serial.begin(115200);                   // Open serial monitor at 115200 baud to see ping results.
    lastCycle = millis() - dispenseDelay;   // pretend last cycle was an hour ago, so first real cycle fires immediately
}

// ping le reservoir, retourne le niveau en pourcentage (0-100), ou -1 si fault capteur
// distance = entre le couvercle et le fluide
float pingRes()
{
    //Serial.print("pingRes"); //test
    //Serial.println("%");//test
    float distance = sonar.ping_median(5);              // median of 5 pings, rejects outliers. distance entre detecteur et chlore
    distance = sonar.convert_cm(distance);              // ping_median returns uS, convert to cm
    if (distance == 0) return -1;                       // 0 = no echo/fault, not "full"
    float niveau = hauteur-distance;                    //cm de chlore dans le reservoir
    if (niveau < 0) niveau = 0;                         // clamp if sensor sits above the 20cm mark
    return (niveau/hauteur)*100;
}

//***************************************************************************
//*************************** MAIN LOOP *************************************
//***************************************************************************
void loop() 
{  
    float pourcentage = pingRes();                      //check current reservoir level and save to variable
    allumerLumiere(pourcentage);                        // checks reservoir level to see if the fill warning light should be turned on
    //Serial.print("Niveau de chlore: ");//test
    //Serial.println(pourcentage);//test
    //Serial.println("%");//test
    
    // gate: if we're still in the cooldown hour, bail out now
    if (millis() - lastCycle < dispenseDelay) return;
    lastCycle = millis();
    if(pourcentage > 15){  //satefy check to make sure there is chlorine in the system before running, to prevent running the pump dry
        chlore_necessaire = getChloreNecessaire()*0.0295735;     //calculate how much chlorine to pump, and conversion from Oz to L
        // B3: orange LED when pool is over target (dose is negative = "too much chlorine")
        if (chlore_necessaire < -highMargin) digitalWrite(LED_HIGH, HIGH);
        else                                 digitalWrite(LED_HIGH, LOW);
  //****************************************** if the necessary injection amount is above 200mL, proceed **************************************   
        if (chlore_necessaire >= 0.2) {
            float chloreRestant = (pourcentage*reservoirFull)/100; //calculates the reservoir level in Litres
            //Serial.print("chloreRestant: ");//test
            //Serial.print(chloreRestant);//test
            //Serial.println("%");//test
            //Serial.print("chlore_necessaire: ");//test
            //Serial.print(chlore_necessaire);//test
            //Serial.println("%");//test
            if (chloreRestant > chlore_necessaire) {             //checks if there is enough chlorine left in reservoir for current instance of injection
                float timeT = chlore_necessaire*1000/pumpRate;   //calculate how long pump turns on (ms) 
                allumerPompe(timeT);                             //active la pompe for the calculated amount of time
            }
        } 
    }
}
