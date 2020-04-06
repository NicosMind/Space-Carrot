/*
 Name:		Space_Carrot.ino
 Created:	06/04/2020 11:59:45
 Author:	nico_
*/



// define some values used by the panel and buttons
int Z_ENABLE, Z_DIR, Z_STEP, R_ENABLE, R_DIR, R_STEP, R_MIN, Z_MAX, Z_MIN, R_COUNT, Z_COUNT, Z_SPEED;
int R_SPEED, TETA_SPEED, TETA_MAXSTEP, tour, TETA_ENABLE, TETA_DIR, TETA_STEP, TETA_MAX, TETA_MIN, PUMP_ONOFF, PAV_ONOFF;
int compteur, MAXH, MAXV, nbpas, distH, distV;
float R_STEPDIST, Z_STEPDIST, TETA_STEPDIST;

void setup()
// open the serial port at 9600 bps:

{
    Serial.begin(9600);



    //defintion pins chariot radial
    R_ENABLE = 15;//enable
    R_DIR = 17;//direction
    R_STEP = 16; //step
    R_MIN = 27; // fin de course

    // définition pins chariot vertical

    Z_ENABLE = 26; //enable
    Z_DIR = 12; //direction
    Z_STEP = 4; //step
    Z_MAX = 39; // fin de course en haut
    Z_MIN = 36; // fin course contact sol


    // définition pins rotation bras

    TETA_ENABLE = 2; //enable
    TETA_DIR = 14; //direction
    TETA_STEP = 13; //step
    TETA_MAX = 34; // fin de course en haut
    TETA_MIN = 35; // fin course contact sol

    // definition pin auxilaire

    PUMP_ONOFF = 25; // pompe a eau
    PAV_ONOFF = 33; //  pompe a vide



    // delai en micros entre HIGH et LOW 
    Z_SPEED = 500;
    R_SPEED = 500;
    TETA_SPEED = 500;



    R_STEPDIST = 3.14 * 10 / 200;
    Z_STEPDIST = 3.14 * 10 / 200;


    // Z_STEPDIST = 3.14 * 10 / 200; //  distance pacourue par 1 pas V

    tour = 200;



    MAXH = 1000;
    MAXV = 5000;
    //definition type pin
    pinMode(Z_ENABLE, OUTPUT);
    pinMode(Z_DIR, OUTPUT);
    pinMode(Z_STEP, OUTPUT);
    pinMode(Z_MAX, INPUT);
    pinMode(Z_MIN, INPUT);

    pinMode(R_ENABLE, OUTPUT);
    pinMode(R_DIR, OUTPUT);
    pinMode(R_STEP, OUTPUT);
    pinMode(R_MIN, INPUT);

    pinMode(TETA_ENABLE, OUTPUT);
    pinMode(TETA_DIR, OUTPUT);
    pinMode(TETA_STEP, OUTPUT);
    pinMode(TETA_MAX, INPUT);
    pinMode(TETA_MIN, INPUT);

    pinMode(PUMP_ONOFF, OUTPUT);
    pinMode(PAV_ONOFF, OUTPUT);
    digitalWrite(PAV_ONOFF, LOW);

}
void loop()
{
    Serial.println(digitalRead(R_MIN));
    R_COUNT = R_HOME();
    Serial.println(R_COUNT);
    Serial.println(digitalRead(R_MIN));
    R_COUNT = R_POS(R_COUNT, 500, R_STEPDIST);
    delay(500);
    Serial.println(R_COUNT);
    R_COUNT = R_POS(R_COUNT, 300, R_STEPDIST);

    Serial.println(R_COUNT);

    Z_COUNT = Z_HOME();
    Serial.println(Z_COUNT);
    delay(1000);
    Z_COUNT = Z_POS(Z_COUNT, 1500, Z_STEPDIST);
    Serial.println(Z_COUNT);

    digitalWrite(PAV_ONOFF, HIGH);
    delay(10000);
    digitalWrite(PAV_ONOFF, LOW);




    while (1);
}

//******* POSITION CHARIOT HORIZONTAL**************************
int R_POS(int compt, int distH, float convR)

{

    nbpas = (compt - distH) / convR;

    // Serial.println(nbpas);

    // on verifie que l'on ne depasse pas la longueur du chariot
    if (distH > MAXH)
    {
        distH = MAXH;
    }

    digitalWrite(R_ENABLE, LOW);

    // on regarde dans quel sens on part
    if (nbpas < 0)
    {
        digitalWrite(R_DIR, LOW); // verifier sens !
    }
    else
    {
        digitalWrite(R_DIR, HIGH); // verifier sens !
    }

    // on avance ou recule du nombre de pas

    for (compteur = 0; compteur < abs(nbpas); compteur++)
    {

        digitalWrite(R_STEP, HIGH);
        delayMicroseconds(R_SPEED);
        digitalWrite(R_STEP, LOW);
        delayMicroseconds(R_SPEED);
    }
    digitalWrite(R_ENABLE, HIGH);
    return (distH);
}

//******* POSITION CHARIOT VERTICAL**************************
int Z_POS(int compt, int distV, float convZ)

{

    nbpas = (compt - distV) /  convZ;

    // Serial.println(nbpas);

    // on verifie que l'on ne depasse pas la longueur du chariot
    if (distV > MAXV)
    {
        distV = MAXV;
    }

    digitalWrite(Z_ENABLE, LOW);

    // on regarde dans quel sens on part
    if (nbpas < 0)
    {
        digitalWrite(Z_DIR, LOW); // verifier sens !
    }
    else
    {
        digitalWrite(Z_DIR, HIGH); // verifier sens !
    }

    // on avance ou recule du nombre de pas

    for (compteur = 0; compteur < abs(nbpas); compteur++)
    {

        digitalWrite(Z_STEP, HIGH);
        delayMicroseconds(Z_SPEED);
        digitalWrite(Z_STEP, LOW);
        delayMicroseconds(Z_SPEED);
        if (digitalRead(Z_MIN) == HIGH)
        {
            break;
        }
    }
    digitalWrite(Z_ENABLE, HIGH);
    // return( distV);

    return (compt + compteur * Z_STEPDIST);
}

//************** HOME VERTICAL*************************
int Z_HOME()

// retour chariot horizontal s'arrete quand stop passe de 0 a 1
{

    digitalWrite(Z_ENABLE, LOW);
    digitalWrite(Z_DIR, HIGH); // verifier sens !
    while (digitalRead(Z_MAX) == LOW)

    {
        digitalWrite(Z_STEP, HIGH);
        delayMicroseconds(Z_SPEED);
        digitalWrite(Z_STEP, LOW);
        delayMicroseconds(Z_SPEED);
    }
    digitalWrite(Z_ENABLE, HIGH);

    return (0);

}

//************** HOME HORIZONTAL*************************
int R_HOME()

// retour chariot horizontal s'arrete quand stop passe de 0 a 1
{

    digitalWrite(R_ENABLE, LOW);
    digitalWrite(R_DIR, HIGH); // verifier sens !
    while (digitalRead(R_MIN) == LOW)

    {
        digitalWrite(R_STEP, HIGH);
        delayMicroseconds(R_SPEED);
        digitalWrite(R_STEP, LOW);
        delayMicroseconds(R_SPEED);
        //  Serial.println(digitalRead(R_MIN));

    }
    digitalWrite(R_ENABLE, HIGH);

    return (0);

}

//************** HOME ANGULAIRE*************************
int TETA_HOME()

// retour chariot horizontal s'arrete quand stop passe de 0 a 1
{

    TETA_MAXSTEP = 0;

    digitalWrite(TETA_ENABLE, LOW);
    digitalWrite(TETA_DIR, HIGH); // verifier sens !
    while (digitalRead(TETA_MIN) == LOW)

    {
        digitalWrite(TETA_STEP, HIGH);
        delayMicroseconds(TETA_SPEED);
        digitalWrite(TETA_STEP, LOW);
        delayMicroseconds(TETA_SPEED);
    }
    digitalWrite(TETA_DIR, LOW); // verifier sens !

    while (digitalRead(TETA_MAX) == LOW)

    {
        TETA_MAXSTEP++;

        digitalWrite(TETA_STEP, HIGH);
        delayMicroseconds(TETA_SPEED);
        digitalWrite(TETA_STEP, LOW);
        delayMicroseconds(TETA_SPEED);
    }


    digitalWrite(TETA_ENABLE, HIGH);

    TETA_STEPDIST = 350 / TETA_MAXSTEP;

    return (TETA_STEPDIST);

}