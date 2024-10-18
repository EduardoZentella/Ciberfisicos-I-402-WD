#ifndef PIVISION_H
#define PIVISION_H

extern const char *USER_EMAIL;
extern const char *SENSOR_ID;

void iniciarPiVisionAccount();
void verificarUsuario(const char *email);
void verificarUnidad(const char *email, const char *sensor_id);
void verificarPoint(const char *email, const char *sensor_id);
void crearUsuario(const char *email);
void crearUnidad(const char *sensor_id);
void crearPoint(const char *email, const char *sensor_id);

#endif