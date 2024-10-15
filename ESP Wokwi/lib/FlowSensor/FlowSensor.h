#ifndef FLOWSENSOR_H
#define FLOWSENSOR_H

#include <Firebase_ESP_Client.h>

// Declarar FirebaseData, FirebaseAuth y FirebaseConfig como externos
extern FirebaseData fbdo;
extern FirebaseAuth auth;
extern FirebaseConfig configF;
extern const char *USER_EMAIL;
extern const char *SENSOR_ID;

void initFlowSensor();
void measureFlow(unsigned long currentTime);
void pulseGen(int pulseTime);

#endif
