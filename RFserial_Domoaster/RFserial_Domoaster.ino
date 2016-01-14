/*
 
 
 */
#include <StandardCplusplus.h>
#include <vector>
#include <iterator>
#include <TimerOne.h>


#define TIMEOUT 5000

volatile bool flag;
volatile bool flag2;
volatile bool incoming_msg;

unsigned int time;
unsigned int flag_time;
unsigned int previousTime;
unsigned int period;
unsigned int elapsed_time;
uint8_t pin_value;

std::vector<uint8_t> periodsList;
std::vector<uint8_t> serial_in;
std::vector<uint8_t> frameToSend;
std::vector<bool> valuesList;

uint8_t period2bytes[2];
uint16_t mask = B11111111;

uint8_t byte_in = 0;
int periodBit = 0;
int frameToSendLength = 0;
static int tx_bit = 7;
static int tx_index = 0;

void setup() {
    Serial.begin(115200);
    pinMode(0, INPUT);
    pinMode(10, OUTPUT);
    attachInterrupt(0, isr, CHANGE);
}

void loop() {
    time = micros();
    elapsed_time = time - previousTime;
    if (incoming_msg) {
        incoming_msg = false;
        detachInterrupt(0);
        Timer1.initialize(periodBit);
        Timer1.attachInterrupt(timerInterrupt);
    }
    // Si changement sur le pin 
    if (flag) {
        // On relève l'horloge en µs
        flag_time = micros();

        // Calcule de la période entre 2 changements sur le pin
        period = flag_time - previousTime;
        // Selon la période 
        if (period > 50 && period < 5000) {

            // Ajout de la période décomposée
            periodsList.push_back(period >> 8);
            periodsList.push_back(period & mask);

        }
        flag2 = true;
        // On réinitialise
        previousTime = time;
        // On baisse le drapeau
        flag = false;
    }
    // Si la période est trop longue, généralement sur LOW
    if (elapsed_time > TIMEOUT && flag2 == true) {
        // Calcul des tailles des listes
        int periodsList_size = periodsList.size();
        // Si le tableau n'est pas vide
        if (periodsList_size > 0) {
            // Création des tableaux à envoyer
            uint8_t periodsData[periodsList_size];
            //uint8_t valuesData[valuesList_size];
            // Copie des données à envoyer
            for (int i = 0; i < periodsList_size; i++) {
                periodsData[i] = periodsList[i];
            }

            // Vidage des listes
            periodsList.clear();

            // Réinitialisation de l'interruption
            detachInterrupt(0);
            attachInterrupt(0, isr, CHANGE);
            // Envoi de la taille décomposée du tableau des périodes
            uint8_t halfsize_first = periodsList_size >> 8;
            uint8_t halfsize_second = periodsList_size & mask;

            Serial.write(halfsize_first);
            Serial.write(halfsize_second);
            Serial.write(periodsData, periodsList_size);


            flag2 = false;
        }
    }
}

// Interruption: on lève le drapeau

void isr() {
    flag = true;
}

void serialEvent() {
    while (Serial.available()) {
        byte_in = Serial.read();
        serial_in.push_back(byte_in);
    }
    uint16_t size_in = serial_in[0] << 8;
    size_in += serial_in[1];
    if (size_in == serial_in.size()) {
        frameToSendLength = size_in - 4;
        periodBit = serial_in[2] << 8;
        periodBit += serial_in[3];
        for (int i = 0; i < frameToSendLength; ++i) {
            frameToSend[i] = serial_in[i + 4];
        }
        tx_index = 0;
        tx_bit = 7;
        incoming_msg = true;
        serial_in.clear();
    }
}

void timerInterrupt() {
    /*if (frameToSend[tx_index]& 1 << tx_bit) {
        PORTB |= (1 << PB2); //digitalWrite(pinTx, HIGH);
    } else {
        PORTB &= ~(1 << PB2); //digitalWrite(pinTx, LOW),
    }*/
    digitalWrite(10, frameToSend[tx_index] & 1 << tx_bit);
    tx_bit--; // next bit of sample
    // if last bit (0), we restart to 8th of next sample
    if (tx_bit < 0) {
        tx_index++;
        tx_bit = 7;
    }
    // if last bit of sample sent
    if (tx_index > frameToSendLength) {
        PORTB &= ~(1 << PB2); //digitalWrite(pinTx, LOW);
        Timer1.detachInterrupt();
        attachInterrupt(0, isr, CHANGE);
    }
}

