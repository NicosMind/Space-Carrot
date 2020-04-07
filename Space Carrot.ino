/*
 Name:		Space_Carrot.ino
 Created:	06/04/2020 11:32:45
 Author:	nico_
*/


// Activation d'un check si le teta est pas en buté sur un des deux switch
// création d'une fonction pour checker les states des enswitchs
// création coordonnées cartésien en polaire 
// création d'une fonction de coordonnées polaire


int PUMP_ONOFF, VACUUM_PUMP_ONOFF;


int MAX_Z_STEPS, MAX_R_STEPS, MAX_T_STEPS;

int Z_ENABLE, Z_DIR, Z_STEP, Z_MIN, Z_MAX, Z_COUNT, Z_SPEED;
int R_ENABLE, R_DIR, R_STEP, R_MIN, R_COUNT, R_SPEED;
int TETA_ENABLE, TETA_DIR, TETA_STEP, TETA_MIN, TETA_MAX, TETA_MAXSTEP, TETA_SPEED;

const int CW = HIGH;
const int CCW = LOW;

bool enableStateTETA = false;
bool enableStateR = false;
bool enableStateZ = false;

int countingStepsTETA = 0;
int countingStepsR = 0;
int countingStepsZ = 0;

float R_STEPDIST = 3.14 * 10 / 200;
float Z_STEPDIST = 3.14 * 10 / 200;                 //distance pacourue par 1 pas V
float TETA_STEPDIST;
int WORK_ANGLE_TETA = 360;                          //Anlge de travail a modifé suivant l'installation de la machine

int FULL_REVOLUTION_STEPS = 200;

#define STEPS 400 // va avec la fonction STEPPER_ACCEL_MOV


//******* POSITION CHARIOT HORIZONTAL**************************
int R_POS(int compt, int NEW_POS_R)
{

  float  numSTEPS = (compt - NEW_POS_R) / R_STEPDIST;
    // Serial.println(numSTEPS);

    // on verifie que l'on ne depasse pas la longueur du chariot
    if (NEW_POS_R > MAX_R_STEPS)
    {
        NEW_POS_R = MAX_R_STEPS;
    }

    digitalWrite(R_ENABLE, LOW);

    // on regarde dans quel sens on part
    if (numSTEPS < 0)                           // est ce que ça serait pas inférieur ou égale ici ex : count 50 cm new distance = 50 arriere on est dans le cas =0 donc on est bloqué !
    {
        digitalWrite(R_DIR, LOW); // verifier sens !
    }
    else
    {
        digitalWrite(R_DIR, HIGH); // verifier sens !
    }

    // on avance ou recule du nombre de pas

    for (int i = 0; i < abs(numSTEPS); i++)
    {
        digitalWrite(R_STEP, HIGH);
        delayMicroseconds(R_SPEED);
        digitalWrite(R_STEP, LOW);
        delayMicroseconds(R_SPEED);
    }
    digitalWrite(R_ENABLE, HIGH);
    R_COUNT = NEW_POS_R;
    //return (NEW_POS_R);
}

//************** HOME HORIZONTAL*******************************
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
        //Serial.println(digitalRead(R_MIN));

    }
    digitalWrite(R_ENABLE, HIGH);
    R_COUNT = 0;
    //return (0);

}

//***************** POSITION CHARIOT VERTICAL******************
int Z_POS(int compt, int NEW_POS_Z)

{
    int buffer = 0;
    float numSTEPS = (compt - NEW_POS_Z) / Z_STEPDIST;

    // Serial.println(numSTEPS);

    // on verifie que l'on ne depasse pas la longueur du chariot
    if (NEW_POS_Z > MAX_Z_STEPS)
    {
        NEW_POS_Z = MAX_Z_STEPS;
    }

    digitalWrite(Z_ENABLE, LOW);

    // on regarde dans quel sens on part
    if (numSTEPS < 0)
    {
        digitalWrite(Z_DIR, LOW); // verifier sens !
    }
    else
    {
        digitalWrite(Z_DIR, HIGH); // verifier sens !
    }

    // on avance ou recule du nombre de pas

    for (int i = 0; i < abs(numSTEPS); i++)
    {

        digitalWrite(Z_STEP, HIGH);
        delayMicroseconds(Z_SPEED);
        digitalWrite(Z_STEP, LOW);
        delayMicroseconds(Z_SPEED);
        if (digitalRead(Z_MIN) == HIGH)         // a modifié : coder pour que le bras descende jusqu'à ce que le Z_MAX s'active pour palper le terrain
        {
            break;
        }
        buffer = i;
    }
    digitalWrite(Z_ENABLE, HIGH);
    //return( NEW_POS_Z);
    Z_COUNT = NEW_POS_Z;
    //Z_COUNT = (compt + buffer+1 * Z_STEPDIST);
    //return (compt + buffer * Z_STEPDIST);
}

//************** HOME VERTICAL*********************************
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

    Z_COUNT = 0;
    //return (0);

}

//************** POSITION ANGULAIRE****************************
int TETA_POS()
{
    if (digitalRead(TETA_MIN)==HIGH || digitalRead(TETA_MAX)==HIGH)
    {
        //send notification error
    }
    else
    {
        //continue program as normal
    }
}
//************** HOME ANGULAIRE********************************
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

    TETA_STEPDIST = WORK_ANGLE_TETA / TETA_MAXSTEP;             //360 deviendra une variable pour ajuster le type d'installation type 360 degree ou 180 ou autre....

    return (TETA_STEPDIST);

}


//************** Mooving one stepper at the time***************
void STEPPER_MOV(int enablePin, int dirPin, int stepPin, float steps, int dirMotor, int speedMotor)
{

    digitalWrite(enablePin, LOW);
    digitalWrite(dirPin, dirMotor);
    for (int x = 0; x < steps; x++)
    {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(speedMotor);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(speedMotor);
    }
    
}

//*********** Mooving multiple stepper at the time*************
void MULTI_STEPPER(float posR, float posTETA)
{
    //Variable a modifer une fois la formule trouvés pour transformer les coordonnes en steps pour les moteurs
    float stepRPos = posR;              
    float stepTETAPos = posTETA;

    float numSteps = 0.0;
    float stepRPosBuffer = 0.0;
    float stepTETAPosBuffer = 0.0;
    if (stepRPos > stepTETAPos)
    {
        numSteps = stepRPos;
    }
    else
    {
        numSteps = stepTETAPos;
    }
    if (numSteps > 0)
    {
        for (int x = 0; x < numSteps; x++)
        {

            stepRPosBuffer += (stepRPos / numSteps);
            stepTETAPosBuffer += (stepTETAPos / numSteps);
            STEPPER_MOV(R_ENABLE, R_DIR, R_STEP, stepRPosBuffer, CW, 1000);
            STEPPER_MOV(TETA_ENABLE, TETA_DIR, TETA_STEP, stepTETAPosBuffer, CW, 1000);
            Serial.print(stepRPosBuffer);
            Serial.print("   ");
            Serial.print(stepTETAPosBuffer);
            Serial.println(" ");
        }
    }
    else if (numSteps < 0)
    {
        numSteps = abs(numSteps);
        stepRPos = abs(stepRPos);
        stepTETAPos = abs(stepTETAPos);

        for (int x = 0; x < numSteps; x++)
        {
            stepRPosBuffer += (stepRPos / numSteps);
            stepTETAPosBuffer += (stepTETAPos / numSteps);
            STEPPER_MOV(R_ENABLE, R_DIR, R_STEP, stepRPosBuffer, CCW, 600);
            STEPPER_MOV(TETA_ENABLE, TETA_DIR, TETA_STEP, stepTETAPosBuffer, CCW, 600);
            Serial.print(stepRPosBuffer);
            Serial.print("   ");
            Serial.print(stepTETAPosBuffer);
            Serial.println("  ");
        }
    }
}


//************** HOMMING ALL MOTOR (Z,R,TETA) *****************
int HOMMING_ALL()
{
    Z_HOME();
    R_HOME();
    TETA_HOME();
}

//******************** LINEAR ACCELERATION ********************
void STEPPER_ACCEL_MOV(int STEP_PIN) {
    // juste un test d'une potentielle fonction pour gérer l'accélération du moteur (fonctionne seulement en simple moteur pas multi)

    int delays[STEPS];
    float angle = 1;
    float accel = 0.01;
    float c0 = 2000 * sqrt(2 * angle / accel) * 0.67703;
    float lastDelay = 0;
    int highSpeed = 100;
    for (int i = 0; i < STEPS; i++) {
        float d = c0;
        if (i > 0)
            d = lastDelay - (2 * lastDelay) / (4 * i + 1);
        if (d < highSpeed)
            d = highSpeed;
        delays[i] = d;
        lastDelay = d;
    }

    // use delays from the array, forward
    for (int i = 0; i < STEPS; i++) {
        digitalWrite(STEP_PIN, HIGH);
        delayMicroseconds(delays[i]);
        digitalWrite(STEP_PIN, LOW);
    }

    // use delays from the array, backward
    for (int i = 0; i < STEPS; i++) {
        digitalWrite(STEP_PIN, HIGH);
        delayMicroseconds(delays[STEPS - i - 1]);
        digitalWrite(STEP_PIN, LOW);
    }
}



//****************** ENABLE/DISABLE MOTOR *********************
void ENABLE_TETA()
{
    if (enableStateTETA == true)
    {
        digitalWrite(TETA_ENABLE, !enableStateTETA);
        enableStateTETA = !enableStateTETA;
    }
    else
    {
        digitalWrite(TETA_ENABLE, !enableStateTETA);
        enableStateTETA = !enableStateTETA;
    }
}

void ENABLE_R()
{
    if (enableStateR == true)
    {
        digitalWrite(R_ENABLE, !enableStateR);
        enableStateR = !enableStateR;
    }
    else
    {
        digitalWrite(R_ENABLE, !enableStateR);
        enableStateR = !enableStateR;
    }
}

void ENABLE_Z()
{
    if (enableStateZ == true)
    {
        digitalWrite(Z_ENABLE, !enableStateZ);
        enableStateZ = !enableStateZ;
    }
    else
    {
        digitalWrite(Z_ENABLE, !enableStateZ);
        enableStateZ = !enableStateZ;
    }
}

//**************** ENABLE/DISABLE ALL MOTOR *******************
void ENABLE_ALL()
{
    ENABLE_TETA();
    ENABLE_R();
    ENABLE_Z();
}


//************* CONTROL TROUGH SERIAL MONITOR *****************
void SERIAL_COMMAND()
{
    if (Serial.available() > 0)
    {
        if (Serial.peek() == 'H')
        {
            int clrBuffer;
            Serial.read();
            clrBuffer = Serial.parseInt();
            HOMMING_ALL();
        }
        else if (Serial.peek() == 'T')
        {
            int numSteps = 0;
            Serial.read();
            numSteps = Serial.parseInt();
            Serial.println(numSteps);
            if (numSteps > 0)
            {
                //Serial.println("tourne CW");
                STEPPER_MOV(TETA_ENABLE, TETA_DIR, TETA_STEP, numSteps, CW, 1000);
                countingStepsTETA = (countingStepsTETA + numSteps);
            }
            else if (numSteps < 0)
            {
                //Serial.println("tourne CCW");
                numSteps = abs(numSteps);
                STEPPER_MOV(TETA_ENABLE, TETA_DIR, TETA_STEP, numSteps, CCW, 1000);
                countingStepsTETA = (countingStepsTETA - numSteps);
            }
        }
        else if (Serial.peek() == 'R')
        {
            int numSteps = 0;
            Serial.read();
            numSteps = Serial.parseInt();
            Serial.println(numSteps);
            R_POS(R_COUNT, numSteps);

        }
        else if (Serial.peek() == 'Z')
        {
            int numSteps = 0;
            Serial.read();
            numSteps = Serial.parseInt();
            Serial.println(numSteps);
            Z_POS(Z_COUNT, numSteps);
        }
        else if (Serial.peek() == 'P')
        {
            int state = 0;
            Serial.read();
            state = Serial.parseInt();
            Serial.println(state);
            digitalWrite(VACUUM_PUMP_ONOFF, state);
        }
        else if (Serial.peek() == 'E')
        {
            int clrBuffer;
            ENABLE_ALL();
            Serial.read();
            clrBuffer = Serial.parseInt();
        }
        else
        {
            int clrBuffer;
            Serial.read();
            clrBuffer = Serial.parseInt();
        }
        Serial.print("StepsTETA : ");
        Serial.println(countingStepsTETA);
        Serial.print("StepsR : ");
        Serial.println(R_COUNT);
        Serial.print("StepsZ : ");
        Serial.println(Z_COUNT);
    }
    else
    {

    }
}

void setup(){

    // open the serial port at 9600 bps:
    Serial.begin(9600);

    //defintion pins chariot radial
    R_ENABLE = 15;      //enable
    R_DIR = 17;         //direction
    R_STEP = 16;        //step
    R_MIN = 27;         //fin de course

    // définition pins chariot vertical
    Z_ENABLE = 26;      //enable
    Z_DIR = 12;         //direction
    Z_STEP = 4;         //step
    Z_MAX = 36;         //fin de course en haut
    Z_MIN = 39;         //fin course contact sol


    // définition pins rotation bras
    TETA_ENABLE = 2;    //enable
    TETA_DIR = 14;      //direction
    TETA_STEP = 13;     //step
    TETA_MAX = 35;      //fin de course en haut
    TETA_MIN = 34;      //fin course contact sol

    // definition pin auxilaire
    PUMP_ONOFF = 25;    //pompe a eau
    VACUUM_PUMP_ONOFF = 33;     //pompe a vide

    // delai en micros entre HIGH et LOW 
    Z_SPEED = 500;
    R_SPEED = 500;
    TETA_SPEED = 500;

    MAX_R_STEPS = 1000;
    MAX_Z_STEPS = 5000;

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
    pinMode(VACUUM_PUMP_ONOFF, OUTPUT);
    digitalWrite(VACUUM_PUMP_ONOFF, LOW);
    digitalWrite(PUMP_ONOFF, LOW);
}

void loop()
{
    SERIAL_COMMAND();


    //Serial.println(digitalRead(R_MIN));     //Affiche l'état du fin de course en postion minimum

    //R_COUNT = R_HOME();                     //Réinitialisation du compteur en R

    //Serial.println(R_COUNT);                //Affichage du compteur en R

    //Serial.println(digitalRead(R_MIN));     //Affiche l'état du fin de course en postion minimum

    //R_COUNT = R_POS(R_COUNT, 500);          // Déplacement de 500 (steps / cm / revolution) et affectation de la nouvelle valeur au compteur

    //Serial.println(R_COUNT);                //Affichage du compteur en R

    //R_COUNT = R_POS(R_COUNT, 300);          //Déplacement de 300 (steps / cm / revolution) et affectation de la nouvelle valeur au compteur

    //Serial.println(R_COUNT);                //Affichage du compteur en R



    //Z_COUNT = Z_HOME();                     //Réinitialisation du compteur en Z

    //Serial.println(Z_COUNT);                //Affichage du compteur en Z

    //Z_COUNT = Z_POS(Z_COUNT, 1500);         //Déplacement de 1500 (steps / cm / revolution) et affectation de la nouvelle valeur au compteur

    //Serial.println(Z_COUNT);                // Affichage du compteur en Z

    //TETA_HOME();

    //Serial.println("fin du test");
    //

    //while (1);                              //Effectue une seule fois le void loop et s'arrête
}