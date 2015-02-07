#ifndef OPTIONS_H
#define OPTIONS_H

#include "types.h"

#define DIMENSION_MIN {128, 128}
#define NBTHREADS_MIN 1      
#define NBMAXIT_MIN 4           

#define DIMENSION_DEFAULT {800, 600} 
#define BOUNDS_DEFAULT {-2.0, 2.0, -1.5, 1.5}
#define INIT_DEFAULT {0.0, 0.0} 
#define FULLSCREEN_DEFAULT 0
#define NBTHREADS_DEFAULT 2
#define NBMAXIT_DEFAULT 32 
#define JULIA_DEFAULT 0
#define PHOTOMODE_DEFAULT 0
#define PICTURENAME_DEFAULT "mandel"
#define CAPTUREMODE_DEFAULT 0
#define CAPTUREZOOMSPEED_DEFAULT 100.0
#define CAPTURENBFRAMES_DEFAULT 150

/* Module de gestion des options du programme (arguments) */

/* Verifie la validite des options - A appeler une fois toutes les options lues */
void options_check();

/* Mutateurs */
void options_setDimension(struct dimension d);
void options_setBounds(struct bounds b);
void options_setInit(struct complex init);
void options_setFullscreen(int boolean);
void options_setNbThreads(int n);
void options_setNbMaxIt(int n);
void options_setJulia(int boolean);
void options_setPhotoMode(int boolean);
void options_setPictureName(const char *name);
void options_setCaptureMode(int boolean);
void options_setCaptureZoomSpeed(double s);
void options_setCaptureNbFrames(int n);

/* Accesseurs */
struct dimension options_getDimension();
struct bounds options_getBounds();
struct complex options_getInit();
int options_getFullscreen();
int options_getNbThreads();
int options_getNbMaxIt();
int options_getJulia();
int options_getPhotoMode();
const char *options_getPictureName();
int options_getCaptureMode();
double options_getCaptureZoomSpeed();
int options_getCaptureNbFrames();

#endif
