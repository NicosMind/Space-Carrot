/*
 Name:		Space_Carrot.ino
 Created:	06/04/2020 11:59:45
 Author:	nico_
*/



// define some values used by the panel and buttons

int  tour, PUMP_ONOFF, VACUUM_PUMP_ONOFF;
int compteur, MAX_R_STEPS, MAX_Z_STEPS, distH, distV;

//int numSTEPS; // a nettoyer dans le code pour l pass� en variable local !!

int Z_ENABLE, Z_DIR, Z_STEP, Z_MIN, Z_MAX, Z_COUNT, Z_SPEED;
int R_ENABLE, R_DIR, R_STEP, R_MIN, R_COUNT, R_SPEED;
int TETA_ENABLE, TETA_DIR, TETA_STEP, TETA_MIN, TETA_MAX, TETA_MAXSTEP, TETA_SPEED;

float R_STEPDIST, Z_STEPDIST, TETA_STEPDIST;


const int CW = HIGH;
const int CCW = LOW;

bool enableStateTETA = false;
bool enableStateR = false;
bool enableStateZ = false;

int countingStepsTETA = 0;
int countingStepsR = 0;
int countingStepsZ = 0;



#define STEPS 400 // va avec la fonction STEPPER_ACCEL_MOV


//******* POSITION CHARIOT HORIZONTAL**************************
int R_POS(int compt, int distH, float convR)

{

  int  numSTEPS = (compt - distH) / convR;

    // Serial.println(numSTEPS);

    // on verifie que l'on ne depasse pas la longueur du chariot
    if (distH > MAX_R_STEPS)
    {
        distH = MAX_R_STEPS;
    }

    digitalWrite(R_ENABLE, LOW);

    // on regarde dans quel sens on part
    if (numSTEPS < 0)
    {
        digitalWrite(R_DIR, LOW); // verifier sens !
    }
    else
    {
        digitalWrite(R_DIR, HIGH); // verifier sens !
    }

    // on avance ou recule du nombre de pas

    for (compteur = 0; compteur < abs(numSTEPS); compteur++)
    {

        digitalWrite(R_STEP, HIGH);
        delayMicroseconds(R_SPEED);
        digitalWrite(R_STEP, LOW);
        delayMicroseconds(R_SPEED);
    }
    digitalWrite(R_ENABLE, HIGH);
    return (distH);
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
        //  Serial.println(digitalRead(R_MIN));

    }
    digitalWrite(R_ENABLE, HIGH);

    return (0);

}

//******* POSITION CHARIOT VERTICAL****************************
int Z_POS(int compt, int distV, float convZ)

{

    int numSTEPS = (compt - distV) /  convZ;

    // Serial.println(numSTEPS);

    // on verifie que l'on ne depasse pas la longueur du chariot
    if (distV > MAX_Z_STEPS)
    {
        distV = MAX_Z_STEPS;
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

    for (compteur = 0; compteur < abs(numSTEPS); compteur++)
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

    return (0);

}

//************** POSITION ANGULAIRE********************************
int TETA_POS()
{

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

    TETA_STEPDIST = 350 / TETA_MAXSTEP; // qu'est ce que signifie ce 350 ???

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
    //Variable a modifer une fois la formule trouv�s pour transformer les coordonnes en steps pour les moteurs
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
    // juste un test d'une potentielle fonction pour g�rer l'acc�l�ration du moteur (fonctionne seulement en simple moteur pas multi)

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
            if (numSteps > 0)
            {
                //Serial.println("tourne CW");
                STEPPER_MOV(R_ENABLE, R_DIR, R_STEP, numSteps, CW, 1000);
                countingStepsR = (countingStepsR + numSteps);
            }
            else if (numSteps < 0)
            {
                //Serial.println("tourne CCW");
                numSteps = abs(numSteps);
                STEPPER_MOV(R_ENABLE, R_DIR, R_STEP, numSteps, CCW, 1000);
                countingStepsR = (countingStepsR - numSteps);
            }
        }
        else if (Serial.peek() == 'Z')
        {
            int numSteps = 0;
            Serial.read();
            numSteps = Serial.parseInt();
            Serial.println(numSteps);
            if (numSteps > 0)
            {
                //Serial.println("tourne CW");
                STEPPER_MOV(Z_ENABLE, Z_DIR, Z_STEP, numSteps, CW, 1000);
                countingStepsZ = (countingStepsZ + numSteps);
            }
            else if (numSteps < 0)
            {
                //Serial.println("tourne CCW");
                numSteps = abs(numSteps);
                STEPPER_MOV(Z_ENABLE, Z_DIR, Z_STEP, numSteps, CCW, 1000);
                countingStepsZ = (countingStepsZ - numSteps);
            }
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
        Serial.println(countingStepsR);
        Serial.print("StepsZ : ");
        Serial.println(countingStepsZ);
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

    // d�finition pins chariot vertical
    Z_ENABLE = 26;      //enable
    Z_DIR = 12;         //direction
    Z_STEP = 4;         //step
    Z_MAX = 39;         //fin de course en haut
    Z_MIN = 36;         //fin course contact sol


    // d�finition pins rotation bras
    TETA_ENABLE = 2;    //enable
    TETA_DIR = 14;      //direction
    TETA_STEP = 13;     //step
    TETA_MAX = 34;      //fin de course en haut
    TETA_MIN = 35;      //fin course contact sol

    // definition pin auxilaire
    PUMP_ONOFF = 25;    //pompe a eau
    VACUUM_PUMP_ONOFF = 33;     //pompe a vide

    // delai en micros entre HIGH et LOW 
    Z_SPEED = 500;
    R_SPEED = 500;
    TETA_SPEED = 500;

    R_STEPDIST = 3.14 * 10 / 200;
    Z_STEPDIST = 3.14 * 10 / 200;


    // Z_STEPDIST = 3.14 * 10 / 200; //  distance pacourue par 1 pas V
    tour = 200;

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

    //digitalWrite(PAV_ONOFF, HIGH);
    //delay(10000);
    //digitalWrite(PAV_ONOFF, LOW);

    while (1);
}