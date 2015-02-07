#include <stdio.h>
#include <stdlib.h>

#include "options.h"

static struct dimension options_dimension = DIMENSION_DEFAULT;
static struct bounds options_bounds = BOUNDS_DEFAULT;
static struct complex options_init = INIT_DEFAULT;
static int options_fullscreen = FULLSCREEN_DEFAULT;
static int options_nbThreads = NBTHREADS_DEFAULT;
static int options_nbMaxIt = NBMAXIT_DEFAULT;
static int options_julia = JULIA_DEFAULT;
static int options_photoMode = PHOTOMODE_DEFAULT;
static const char *options_pictureName = PICTURENAME_DEFAULT;
static int options_captureMode = CAPTUREMODE_DEFAULT;
static double options_captureZoomSpeed = CAPTUREZOOMSPEED_DEFAULT;
static int options_captureNbFrames = CAPTURENBFRAMES_DEFAULT;

void options_check()
{
	struct dimension dim_min = DIMENSION_MIN;
	if (options_dimension.width < dim_min.width) {
		printf("\nLargeur de la dimension trop petite\n"); exit(EXIT_FAILURE);
	}
	if (options_dimension.height < dim_min.height) {
		printf("\nHauteur de la dimension trop petite\n"); exit(EXIT_FAILURE);
	}
	if (options_bounds.xmin >= options_bounds.xmax) {
		printf("\nBornes de l'espace incorrectes (xmin >= xmax)\n"); exit(EXIT_FAILURE);
	}
	if (options_bounds.ymin >= options_bounds.ymax) {
		printf("\nBornes de l'espace incorrectes (ymin >= ymax)\n"); exit(EXIT_FAILURE);
	}
	if (options_nbThreads < NBTHREADS_MIN) {
		printf("\nNombre de threads minimal non respecté\n"); exit(EXIT_FAILURE);
	}
	if (options_nbMaxIt < NBMAXIT_MIN) {
		printf("\nNombre minimal d'itérations non respecté\n"); exit(EXIT_FAILURE);
	}
	if (options_captureZoomSpeed <= 0.0) {
		printf("\nVitesse de zoom incorrecte\n"); exit(EXIT_FAILURE);
	}
	if (options_captureNbFrames < 1) {
		printf("\nNombre d'images incorrect\n"); exit(EXIT_FAILURE);
	}
	if (options_photoMode && options_captureMode) {
		printf("\nLes modes photos et capture sont incompatibles\n"); exit(EXIT_FAILURE);
	}
}

/*********************************************/
/*******           MUTATEURS       ***********/
/*********************************************/
void options_setDimension(struct dimension d)
{
	options_dimension = d;
}

void options_setBounds(struct bounds b)
{
	options_bounds = b;
}

void options_setInit(struct complex init)
{
	options_init = init;
}

void options_setFullscreen(int boolean)
{
	options_fullscreen = boolean;
}

void options_setNbThreads(int n)
{
	options_nbThreads = n;
}

void options_setNbMaxIt(int n)
{
	options_nbMaxIt = n;
}

void options_setJulia(int boolean)
{
	options_julia = boolean;
}

void options_setPhotoMode(int boolean)
{
	options_photoMode = boolean;
}

void options_setPictureName(const char *name)
{
	options_pictureName = name;
}

void options_setCaptureMode(int boolean)
{
	options_captureMode = boolean;
}

void options_setCaptureZoomSpeed(double s)
{
	options_captureZoomSpeed = s;
}

void options_setCaptureNbFrames(int n)
{
	options_captureNbFrames = n;
}

/*********************************************/
/*******         ACCESSEURS        ***********/
/*********************************************/
struct dimension options_getDimension()
{
	return options_dimension;
}

struct bounds options_getBounds()
{
	return options_bounds;
}

struct complex options_getInit()
{
	return options_init;
}

int options_getFullscreen()
{
	return options_fullscreen;
}

int options_getNbThreads()
{
	return options_nbThreads;
}

int options_getNbMaxIt()
{
	return options_nbMaxIt;
}

int options_getJulia()
{
	return options_julia;
}

int options_getPhotoMode()
{
	return options_photoMode;
}

const char *options_getPictureName()
{
	return options_pictureName;
}

int options_getCaptureMode()
{
	return options_captureMode;
}

double options_getCaptureZoomSpeed()
{
	return options_captureZoomSpeed;
}

int options_getCaptureNbFrames()
{
	return options_captureNbFrames;
}
