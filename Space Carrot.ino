/*
 Name:		Space_Carrot.ino
 Created:	06/04/2020 11:32:45
 Author:	nico_
*/



// cr�ation coordonn�es cart�sien en polaire 
// cr�ation d'une fonction de coordonn�es polaire


#include "geometry.h"
int PUMP_ONOFF, VACUUM_PUMP_ONOFF;


int MAX_Z_STEPS, MAX_R_STEPS, MAX_T_STEPS;

int Z_ENABLE, Z_DIR, Z_STEP, Z_MIN, Z_MAX, Z_COUNT, Z_SPEED;
int R_ENABLE, R_DIR, R_STEP, R_MIN, R_MAX, R_COUNT, R_SPEED, R_MAXSTEP;
int TETA_ENABLE, TETA_DIR, TETA_STEP, TETA_MIN, TETA_MAX, TETA_MAXSTEP, TETA_SPEED, T_COUNT;

const int CW = HIGH;
const int CCW = LOW;

bool enableStateTETA = false;
bool enableStateR = false;
bool enableStateZ = false;

int countingStepsTETA = 0;
int countingStepsR = 0;
int countingStepsZ = 0;

int MICRO_STEP_CONFIG = 1 / 2;
int FULL_REVOLUTION_STEPS = 400;

float R_STEPDIST = 3.14 * 10 / 400;
float Z_STEPDIST = 3.14 * 10 / 400;                 //distance pacourue par 1 pas V
float T_STEP_ANGLE;
int WORK_ANGLE_TETA = 360;                          //Angle de travail a modif� suivant l'installation de la machine


int T_OFFSET = 10;     //Degree
int R_OFFSET = 150;    //mm
int Z_MIN_OFFSET = 50; //mm
int Z_MAX_OFFSET = 50; //mm



//TIMER VARIABLE                // valeur a changer ce sont juste des tests !

long R_HOMMING_INTERVAL = 30000; //milliseconds
long Z_HOMMING_INTERVAL = 30000;
long T_HOMMING_INTERVAL = 30000;






                                                                                //***************** POSITION TROLLEY **************************
int R_POS(int compt, int NEW_POS_R, int minSpeed, int maxSpeed)
{
    float  numSTEPS = (compt - NEW_POS_R) / R_STEPDIST;
    if (NEW_POS_R > MAX_R_STEPS) // on verifie que l'on ne depasse pas la longueur du chariot
    {
        NEW_POS_R = MAX_R_STEPS;
    }
    int lowSpeed = minSpeed;
    int highSpeed = maxSpeed;
    int change = 2;
    int rampUpEnding = (lowSpeed - highSpeed) / change;
    if (rampUpEnding > abs(numSTEPS) / 2)
    {
        rampUpEnding = abs(numSTEPS) / 2;
    }
    int rampDownBegin = abs(numSTEPS) - rampUpEnding;
    int speedDelay = lowSpeed;
    digitalWrite(R_ENABLE, LOW);
    // on regarde dans quel sens on part
    if (numSTEPS < 0)
    {
        digitalWrite(R_DIR, HIGH); // verifier sens !
    }
    else
    {
        digitalWrite(R_DIR, LOW); // verifier sens !
    }
    for (int i = 0; i < abs(numSTEPS); i++)   // on avance ou recule du nombre de pas
    {
        digitalWrite(R_STEP, HIGH);
        digitalWrite(R_STEP, LOW);
        delayMicroseconds(speedDelay);
        if (i < rampUpEnding)
        {
            speedDelay -= change;
        }
        else if (i > rampDownBegin)
        {
            speedDelay += change;
        }
        //Serial.println(speedDelay);
    }
    digitalWrite(R_ENABLE, HIGH);
    R_COUNT = NEW_POS_R;
    CHECK_ENDSTOP_R();
}

                                                                                //********************* HOME TROLLEY **************************
int R_HOME()
// retour chariot horizontal s'arrete quand stop passe de 0 a 1
{
    R_COUNT = 0;
    unsigned long startMillis = millis();
    digitalWrite(R_ENABLE, LOW);
    digitalWrite(R_DIR, LOW); // verifier sens !
    while (digitalRead(R_MIN) == LOW)
    {
        if (timer(startMillis, R_HOMMING_INTERVAL) == true)
        {
            Serial.println("Time out sur R");
            break;
        }
        digitalWrite(R_STEP, HIGH);
        delayMicroseconds(R_SPEED);
        digitalWrite(R_STEP, LOW);
        delayMicroseconds(R_SPEED);
    }
    digitalWrite(R_ENABLE, HIGH);
    R_POS(R_COUNT, R_OFFSET,2000,500);
    R_COUNT = R_COUNT - R_OFFSET;
}

                                                                                //****************** TROLLEY CALIBRATION **********************
int R_CALIBRATION()
{
    R_MAXSTEP = 0;
    digitalWrite(TETA_ENABLE, LOW);
    digitalWrite(TETA_DIR, LOW); // verifier sens !
    while (digitalRead(R_MIN) == LOW)
    {
        digitalWrite(R_STEP, HIGH);
        delayMicroseconds(R_SPEED);
        digitalWrite(R_STEP, LOW);
        delayMicroseconds(R_SPEED);
    }
    digitalWrite(R_DIR, HIGH); // verifier sens !
    while (digitalRead(R_MAX) == LOW)
    {
        digitalWrite(R_STEP, HIGH);
        delayMicroseconds(R_SPEED);
        digitalWrite(R_STEP, LOW);
        delayMicroseconds(R_SPEED);
        R_MAXSTEP = R_MAXSTEP + 1;
    }
    digitalWrite(R_ENABLE, HIGH);
    Serial.print("Maximum de pas: ");
    Serial.println(R_MAXSTEP);
    return(R_MAXSTEP);
}




                                                                                //********************** ARM POSITION *************************
int Z_POS(int compt, int NEW_POS_Z)
{
    int buffer = 0;
    float numSTEPS = (compt - NEW_POS_Z) / Z_STEPDIST;
    if (NEW_POS_Z > MAX_Z_STEPS)    // on verifie que l'on ne depasse pas la longueur du chariot
    {
        NEW_POS_Z = MAX_Z_STEPS;
    }
    digitalWrite(Z_ENABLE, LOW);
    if (numSTEPS < 0)     // on regarde dans quel sens on part
    {
        digitalWrite(Z_DIR, HIGH); // verifier sens !
    }
    else
    {
        digitalWrite(Z_DIR, LOW); // verifier sens !
    }
    for (int i = 0; i < abs(numSTEPS); i++)    // on avance ou recule du nombre de pas
    {
        digitalWrite(Z_STEP, HIGH);
        delayMicroseconds(Z_SPEED);
        digitalWrite(Z_STEP, LOW);
        delayMicroseconds(Z_SPEED);
        if (digitalRead(Z_MAX) == HIGH)         // a modifi� : coder pour que le bras descende jusqu'� ce que le Z_MAX s'active pour palper le terrain
        {
            digitalWrite(Z_DIR, LOW);                        // � v�rifi� le sens
            for (int j = 0; j < Z_MAX_OFFSET; j++)
            {
                digitalWrite(Z_STEP, HIGH);
                delayMicroseconds(Z_SPEED);
                digitalWrite(Z_STEP, LOW);
                delayMicroseconds(Z_SPEED);
                buffer = -j;
            }
            break;
        }
        buffer = buffer + i;
    }
    digitalWrite(Z_ENABLE, HIGH);
    Z_COUNT = NEW_POS_Z;
    CHECK_ENDSTOP_Z();
}

                                                                                //********************** HOME ARM *****************************
int Z_HOME()
// retour chariot horizontal s'arrete quand stop passe de 0 a 1
{
    Z_COUNT = 0;
    unsigned long startMillis = millis();
    digitalWrite(Z_ENABLE, LOW);
    digitalWrite(Z_DIR, LOW); // verifier sens !
    while (digitalRead(Z_MIN) == LOW)
    {
        if (timer(startMillis, Z_HOMMING_INTERVAL) == true)
        {
            Serial.println("Time out sur Z");
            break;
        }
        digitalWrite(Z_STEP, HIGH);
        delayMicroseconds(Z_SPEED);
        digitalWrite(Z_STEP, LOW);
        delayMicroseconds(Z_SPEED);
    }
    digitalWrite(Z_ENABLE, HIGH);
    Z_POS(Z_COUNT, Z_MIN_OFFSET);
    Z_COUNT = Z_COUNT - Z_MIN_OFFSET;
}


                                                                                //******************** ANGLE POSITON **************************
int TETA_POS(int compt, int NEW_POS_T, int minSpeed, int maxSpeed)
{
    float  numSTEPS = ((compt - NEW_POS_T) * T_STEP_ANGLE);
    if (NEW_POS_T > TETA_MAXSTEP / T_STEP_ANGLE)                        // on verifie que l'on ne depasse pas la longueur du chariot
    {
        (NEW_POS_T) = TETA_MAXSTEP / T_STEP_ANGLE;
    }
    int lowSpeed = minSpeed;
    int highSpeed = maxSpeed;
    int change = 2;
    int rampUpEnding = (lowSpeed - highSpeed) / change;
    if (rampUpEnding > abs(numSTEPS) / 2)
    {
        rampUpEnding = abs(numSTEPS) / 2;
    }
    int rampDownBegin = abs(numSTEPS) - rampUpEnding;
    int speedDelay = lowSpeed;
    digitalWrite(TETA_ENABLE, LOW);
    if (numSTEPS < 0)    // on regarde dans quel sens on part
    {
        digitalWrite(TETA_DIR, HIGH); // verifier sens !
    }
    else
    {
        digitalWrite(TETA_DIR, LOW); // verifier sens !
    }
    for (int i = 0; i < abs(numSTEPS); i++)    // on avance ou recule du nombre de pas
    {
        digitalWrite(TETA_STEP, HIGH);
        digitalWrite(TETA_STEP, LOW);
        delayMicroseconds(speedDelay);
        if (i < rampUpEnding)
        {
            speedDelay -= change;
        }
        else if (i > rampDownBegin)
        {
            speedDelay += change;
        }
    }
    digitalWrite(TETA_ENABLE, HIGH);
    T_COUNT = NEW_POS_T;
    CHECK_ENDSTOP_T();
}


                                                                                //********************** HOME ANGLE ***************************
int TETA_HOME()
{
    T_COUNT = 0;
    unsigned long startMillis = millis();
    digitalWrite(TETA_ENABLE, LOW);
    digitalWrite(TETA_DIR, LOW); // verifier sens !
    while (digitalRead(TETA_MIN) == LOW)
    {
        if (timer(startMillis, T_HOMMING_INTERVAL) == true)
        {
            Serial.println("Time out sur T");
            break;
        }
        digitalWrite(TETA_STEP, HIGH);
        delayMicroseconds(TETA_SPEED);
        digitalWrite(TETA_STEP, LOW);
        delayMicroseconds(TETA_SPEED);
    }
    digitalWrite(TETA_ENABLE, HIGH);
    TETA_POS(T_COUNT, T_OFFSET, 2000, 500);
    T_COUNT = (T_COUNT - T_OFFSET);
}

                                                                                //****************** ANGLE CALIBRATION ************************
int TETA_CALIBRATION()
{
    TETA_MAXSTEP = 0;
    digitalWrite(TETA_ENABLE, LOW);
    digitalWrite(TETA_DIR, LOW); // verifier sens !
    while (digitalRead(TETA_MIN) == LOW)
    {
        digitalWrite(TETA_STEP, HIGH);
        delayMicroseconds(TETA_SPEED);
        digitalWrite(TETA_STEP, LOW);
        delayMicroseconds(TETA_SPEED);
    }
    digitalWrite(TETA_DIR, HIGH); // verifier sens !
    while (digitalRead(TETA_MAX) == LOW)
    {
        digitalWrite(TETA_STEP, HIGH);
        delayMicroseconds(TETA_SPEED);
        digitalWrite(TETA_STEP, LOW);
        delayMicroseconds(TETA_SPEED);
        TETA_MAXSTEP=TETA_MAXSTEP+1;
    }
    digitalWrite(TETA_ENABLE, HIGH);
    T_STEP_ANGLE = ((TETA_MAXSTEP -T_OFFSET) / WORK_ANGLE_TETA);
    Serial.print("NB pas pour 1 degree: ");
    Serial.println(T_STEP_ANGLE);
    Serial.print("Maximum de pas: ");
    Serial.println(TETA_MAXSTEP);
}




                                                                                //************** HOMMING ALL MOTOR (Z,R,TETA) *****************
int HOMMING_ALL()
{
    Z_HOME();
    R_HOME();
    TETA_HOME();
}



                                                                                //************** MOOVING ON STEPPER AT THE TIME ***************

//void STEPPER_MOOV(int enablePin, int dirPin, int stepPin, float steps, int dirMotor, int speedMotor)
//{
//
//    digitalWrite(enablePin, LOW);
//    digitalWrite(dirPin, dirMotor);
//    for (int x = 0; x < steps; x++)
//    {
//        digitalWrite(stepPin, HIGH);
//        delayMicroseconds(speedMotor);
//        digitalWrite(stepPin, LOW);
//        delayMicroseconds(speedMotor);
//    }
//    
//}
                                                                                //********** MOOVING MUTLTIPLE STEPPER AT THE TIME ************
//void MULTI_STEPPER(float posR, float posTETA)
//{
//    //Variable a modifer une fois la formule trouv�s pour transformer les coordonnes en steps pour les moteurs
//    float stepRPos = posR;              
//    float stepTETAPos = posTETA;
//
//    float numSteps = 0.0;
//    float stepRPosBuffer = 0.0;
//    float stepTETAPosBuffer = 0.0;
//    if (stepRPos > stepTETAPos)
//    {
//        numSteps = stepRPos;
//    }
//    else
//    {
//        numSteps = stepTETAPos;
//    }
//    if (numSteps > 0)
//    {
//        for (int x = 0; x < numSteps; x++)
//        {
//
//            stepRPosBuffer += (stepRPos / numSteps);
//            stepTETAPosBuffer += (stepTETAPos / numSteps);
//            STEPPER_MOOV(R_ENABLE, R_DIR, R_STEP, stepRPosBuffer, CW, 1000);
//            STEPPER_MOOV(TETA_ENABLE, TETA_DIR, TETA_STEP, stepTETAPosBuffer, CW, 1000);
//            Serial.print(stepRPosBuffer);
//            Serial.print("   ");
//            Serial.print(stepTETAPosBuffer);
//            Serial.println(" ");
//        }
//    }
//    else if (numSteps < 0)
//    {
//        numSteps = abs(numSteps);
//        stepRPos = abs(stepRPos);
//        stepTETAPos = abs(stepTETAPos);
//
//        for (int x = 0; x < numSteps; x++)
//        {
//            stepRPosBuffer += (stepRPos / numSteps);
//            stepTETAPosBuffer += (stepTETAPos / numSteps);
//            STEPPER_MOOV(R_ENABLE, R_DIR, R_STEP, stepRPosBuffer, CCW, 600);
//            STEPPER_MOOV(TETA_ENABLE, TETA_DIR, TETA_STEP, stepTETAPosBuffer, CCW, 600);
//            Serial.print(stepRPosBuffer);
//            Serial.print("   ");
//            Serial.print(stepTETAPosBuffer);
//            Serial.println("  ");
//        }
//    }
//}
                                                                                //******************** LINEAR ACCELERATION ********************




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



int CHECK_ENDSTOP_Z()
{
    if (digitalRead(Z_MIN) == HIGH || digitalRead(Z_MAX) == HIGH)
    {
        //send notification error
        Serial.println("error endswitch sur Z");
    }
}
int CHECK_ENDSTOP_R()
{
    if (digitalRead(R_MIN) == HIGH /*|| digitalRead(R_MAX) == HIGH*/)
    {
        //send notification error
        Serial.println("error endswitch sur R");
    }
}
int CHECK_ENDSTOP_T()
{
    if (digitalRead(TETA_MIN) == HIGH || digitalRead(TETA_MAX) == HIGH)
    {
        //send notification error
        Serial.println("error endswitch sur T");
    }
}




bool timer(long startTime , long interval)
{
    unsigned long currentMillis = millis();
    if ((currentMillis - startTime) > interval)
    {
        return (true);
    }
    else
    {
        return (false);
    }
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
        else if (Serial.peek() == 'C')
        {
            int clrBuffer;
            TETA_CALIBRATION();
            Serial.read();
            clrBuffer = Serial.parseInt();
        }
        else if (Serial.peek() == 'T')
        {
            float numSteps = 0;
            Serial.read();
            numSteps = Serial.parseInt();
            Serial.println(numSteps);
            TETA_POS(T_COUNT, numSteps, 2000, 500);
        }
        else if (Serial.peek() == 'R')
        {
            int numSteps = 0;
            Serial.read();
            numSteps = Serial.parseInt();
            Serial.println(numSteps);
            R_POS(R_COUNT, numSteps, 2000, 500);

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
        Serial.print("StepsT : ");
        Serial.print(T_COUNT);
        Serial.println(" Degree");
        Serial.print("StepsR : ");
        Serial.print(R_COUNT);
        Serial.println(" cm");
        Serial.print("StepsZ : ");
        Serial.print(Z_COUNT);
        Serial.println(" cm");
        Serial.println(" ");
    }
    else
    {

    }
}


int GO_TO_COORDINATE(int x, int y)
{
    Coordinates c = CartesianToPolar(x, y);
    R_POS(R_COUNT, c.array[0], 2000, 400); Serial.println(c.array[0]);
    TETA_POS(T_COUNT, c.array[1], 2000, 400); Serial.println(c.array[1]);

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

    MAX_R_STEPS = 100000;
    MAX_Z_STEPS = 500000;
    MAX_T_STEPS = 100000; // variable a bien d�finir 1000 est une valeur random pour le moment ! 

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

    //TETA_CALIBRATION();
    //HOMMING_ALL();
}

void loop()
{
    SERIAL_COMMAND();
    TETA_CALIBRATION();
    HOMMING_ALL();
    delay(1000);
    GO_TO_COORDINATE(1, 1);
    while (1);

    //Coordinates c = CartesianToPolar(1, 1);
    //Serial.println(c.array[0]);
    //Serial.println(c.array[1]);
    //while (true) {

    //}
}