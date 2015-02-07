#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "args.h"
#include "options.h"
#include "types.h"

/* Vérifie que l'argument arg_num pour param existe */
static void check_index(int arg_num, int argc, char *param)
{
	if (arg_num >= argc) {
		printf("\nParamètre manquant pour \"%s\"\n", param);
		exit(EXIT_FAILURE);
	}
}

/* Lit une string dans l'argument arg_num pour le param param_num*/
static char *read_string(int param_num, int arg_num, int argc, char* argv[])
{
	check_index(arg_num, argc, argv[param_num]);
	return argv[arg_num];
}

/* Lit un réel dans l'argument arg_num pour le param param_num*/
static double read_double(int param_num, int arg_num, int argc, char* argv[])
{
	check_index(arg_num, argc, argv[param_num]);
	double val = (double) atof(argv[arg_num]);
	return val;
}

/* Lit un entier dans l'argument arg_num pour le param param_num*/
static int read_integer(int param_num, int arg_num, int argc, char* argv[])
{
	check_index(arg_num, argc, argv[param_num]);
	int val = atoi(argv[arg_num]);
	return val;
}

/* Lit la dimension */
static void read_dimension(int param_num, int argc, char* argv[])
{
	int arg_num = param_num+1;
	struct dimension d;
	d.width = read_integer(param_num, arg_num, argc, argv);
	arg_num++;
	d.height = read_integer(param_num, arg_num, argc, argv);
	options_setDimension(d);
}

/* Lit l'initialisateur d'ensemble */
static void read_init(int param_num, int argc, char* argv[])
{
	int arg_num = param_num+1;
	struct complex init;
	init.real = read_double(param_num, arg_num, argc, argv);
	arg_num++;
	init.im = read_double(param_num, arg_num, argc, argv);
	options_setInit(init);
}

/* Lit les bornes de l'ensemble */
static void read_bounds(int param_num, int argc, char* argv[])
{
	int arg_num = param_num+1;
	struct bounds b;
	b.xmin = read_double(param_num, arg_num, argc, argv);
	arg_num++;
	b.xmax = read_double(param_num, arg_num, argc, argv);
	arg_num++;
	b.ymin = read_double(param_num, arg_num, argc, argv);
	arg_num++;
	b.ymax = read_double(param_num, arg_num, argc, argv);
	options_setBounds(b);
}

void args_read(int argc, char *argv[])
{
	int i = 1;

	while (i < argc) {
		if (strcmp(argv[i], "-d") == 0) {
			read_dimension(i, argc, argv);
			i+=2;
		} else if (strcmp(argv[i], "-b") == 0) {
			read_bounds(i, argc, argv);
			i+=4;
		} else if (strcmp(argv[i], "-i") == 0) {
			read_init(i, argc, argv);
			i+=2;
		} else if (strcmp(argv[i], "-f") == 0) {
			options_setFullscreen(1);	
		} else if (strcmp(argv[i], "-t") == 0) {
			options_setNbThreads(read_integer(i, i+1, argc, argv));
			++i;
		} else if (strcmp(argv[i], "-n") == 0) {
			options_setNbMaxIt(read_integer(i, i+1, argc, argv));
			++i;
		} else if (strcmp(argv[i], "-j") == 0) {
			options_setJulia(1);
		} else if (strcmp(argv[i], "-p") == 0) {
			options_setPhotoMode(1);
		} else if (strcmp(argv[i], "--picture-name") == 0) {
			options_setPictureName(read_string(i, i+1, argc, argv));
			++i;
		} else if (strcmp(argv[i], "-c") == 0) {
			options_setCaptureMode(1);
			options_setCaptureZoomSpeed(read_double(i, i+1, argc, argv));
			++i;
			options_setCaptureNbFrames(read_integer(i, i+1, argc, argv));
			++i;
		} else {
			printf("\nParamètre inconnu : \"%s\"\n", argv[i]);
			exit(EXIT_FAILURE);
		}
		++i;
	}
	options_check();
}
