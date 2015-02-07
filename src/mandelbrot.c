#include <math.h>
#include <pthread.h>
#include <SDL/SDL.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#include "mandelbrot.h"
#include "options.h"

#define MICROSEC_IN_A_SEC 1000000
#define LOG_2 0.693147181 
#define NBCOLOR 4096
#define GET_RANDOM_DOUBLE_BETWEEN(inf, sup) ((((double)rand()/(RAND_MAX))*(sup-inf))+inf)

// Compilation conditionnelle car windows ne connait pas sleep...
#ifdef WIN32
#include <windows.h>
#define WAIT_A_SEC Sleep(1000)
#elif defined (linux)
#define WAIT_A_SEC sleep(1)
#endif

/*********************************************/
/***   VARIABLES DE GESTION MULTITHREAD   ****/
/*********************************************/

/* Synchronisation */
static pthread_mutex_t mutex;
static sem_t working;              // permet de controler les threads
static sem_t waiting;              // permet d'attendre la fin du calcul
static int finished_jobs;          // nombre de threads ayant fini le calcul
static int nbThreads; 
static pthread_t *threads_id;     
static pthread_t watcher;          // surveille la progression du calcul
static int nextLine;               // prochaine ligne à calculer 

/*********************************************/
/***            PARAMETRES DU MOTEUR      ****/
/*********************************************/

static struct bounds bounds;       // bornes de l'espace
static struct complex init;        // c (Julia) ou z0 (Mandelbrot)
static int nbMaxIt;                // Nombre max d'itérations par point
static int julia;                  // Julia (1) ou Mandelbrot (0) ?
static SDL_Surface *surface;       // surface dans laquelle rendre l'ensemble
static double xIncr, yIncr;        // Distance entre deux points de l'espace
static int display = 1;            // Affichage avancement et temps calcul ?
static Uint32 color_table[NBCOLOR];// table des couleurs

/*********************************************/
/***             COULEURS                 ****/
/*********************************************/

static void HSV_to_RGB(double h, double s, double v, int *_r, int *_g, int *_b)
{
	double f, l, m, n, r = 0, g = 0, b = 0;
	int hi = (int) (h/60)%6;
	f = h/60 - hi;
	l = v*(1-s);
	m = v*(1-f*s);
	n = v*(1-(1-f)*s);
	switch (hi) {
		case 0:
			r = v; g = n; b = l; break;
		case 1:
			r = m; g = v; b = l; break;
		case 2:
			r = l; g = v; b = n; break;
		case 3:
			r = l; g = m; b = v; break;
		case 4:
			r = n; g = l; b = v; break;
		case 5:
			r = v; g = l; b = m; break;
	}
	r *= 255; g *= 255; b *= 255;
	*_r = r; *_g = g; *_b = b;
}

static void init_color_table(double h, double hAngleDegree, double s, double v) 
{
	int i, r, g, b;
	for (i = 0; i < NBCOLOR; ++i) {
		h += hAngleDegree/NBCOLOR;
                if (h > 360)
                        h = 0;
		HSV_to_RGB(h, s, v, &r, &g, &b);
		color_table[i] = SDL_MapRGB(surface->format, (Uint8)r, (Uint8)g, (Uint8)b);
	}
}

/*********************************************/
/***                 CALCUL               ****/
/*********************************************/

/* Calcule l'itération pour la ligne y */
static void calc(int y) 
{
	int x, it;
	double square_module, newReal, newIm, val;
	struct complex z, c;
	struct complex point = {bounds.xmin, bounds.ymin + y*yIncr};
	Uint32 *pixel = (Uint32*) surface->pixels + y*(surface->pitch/4);
	for (x = 0; x < surface->w; ++x) {
		if (julia) {
			// Initialisation Julia 
			c = init; z = point;
		} else {
			// Initialisation Mandelbrot
			z = init; c = point;
		}

		it = 0;
		do {
			newReal = z.real*z.real - z.im*z.im + c.real;
			newIm = 2*z.real*z.im + c.im;
			z.real = newReal;
			z.im = newIm;
			square_module = z.real*z.real + z.im*z.im;
		} while (square_module <= 4 && ++it < nbMaxIt); 
		point.real += xIncr;
		if (it == nbMaxIt) {
			*pixel = 0;
		} else {
			val = (it - (log(0.5*log(square_module))/LOG_2))/nbMaxIt;
			val = (val<0.0)?0.0:val;
			val = (val>1.0)?1.0:val;
			*pixel = color_table[(int) (val*NBCOLOR) % NBCOLOR];
		}
		++pixel;
	}
}

/*********************************************/
/***           THREAD LIVES               ****/
/*********************************************/

/* La vie du thread surveillant l'avancée - pas de synchro, thread read-only */
static void *life_Of_Watcher(void *noargs) 
{
	double avancee;
	while(1) {
		if (surface == NULL)
			avancee = 100;
		else 
			avancee = (double) (nextLine* 100) / surface->h;
		if (display && (int) avancee < 100) {
			printf("\rCalcul en cours... %2.1f %%             ", avancee);
			fflush(stdout); // force affichage
		}
		WAIT_A_SEC; 
	}
	return NULL;
}

/* La vie d'un thread de calcul... */
static void *life_Of_Thread (void *noargs) 
{
	int task;
	sem_wait(&working);
	while(1) {
		pthread_mutex_lock(&mutex);
		task = nextLine;
		if (task == surface->h) {
			// fin du calcul
			++finished_jobs;
			if (finished_jobs == nbThreads) // dernier thread
				sem_post(&waiting);
			pthread_mutex_unlock(&mutex);
			sem_wait(&working);
			continue;
		} else {
			// rendre prochaine tache disponible
			++nextLine;
			pthread_mutex_unlock(&mutex);
		}
		calc(task);
	}
	return NULL;
}

/*********************************************/
/***       PUBLIC FUNCTIONS               ****/
/*********************************************/

void mandelbrot_init(int _nbThreads) 
{
	nbThreads = _nbThreads;

	threads_id = (pthread_t*) malloc(nbThreads * sizeof(pthread_t));
	pthread_mutex_init(&mutex, NULL);
	sem_init(&working, 0, 0);
	sem_init(&waiting, 0, 0);

	int i;
	if (pthread_create(&watcher, NULL, life_Of_Watcher, NULL)) {
		printf("\nImpossible de créer un thread\n"); exit(EXIT_FAILURE);
	}
	for (i = 0; i < nbThreads; ++i) 
		if (pthread_create(&threads_id[i], NULL, life_Of_Thread, NULL)) {
			printf("\nImpossible de créer un thread\n"); exit(EXIT_FAILURE);
		}
}

void mandelbrot_setDisplay(int boolean)
{
	display = boolean;
}

void mandelbrot_changeColors()
{
        static int randInit = 0;
        if (!randInit) {
                srand(time(NULL));
                randInit = 1;
        }
        init_color_table(GET_RANDOM_DOUBLE_BETWEEN(60, 360), GET_RANDOM_DOUBLE_BETWEEN(60, 360), 0.9, 0.9);
}

void mandelbrot_render(struct bounds _bounds, struct complex _init, 
		int _julia, int _nbMaxIt, SDL_Surface *_surface) 
{
	struct timeval start, end;
	int i;

	// Paramétrage moteur
	gettimeofday(&start, NULL);
	bounds = _bounds;
	init = _init;
	nbMaxIt = _nbMaxIt;
	julia = _julia;
	if (surface != _surface) {
		surface = _surface;
		init_color_table(0, 360, 0.9, 0.9);
	}
	xIncr = (bounds.xmax - bounds.xmin) / surface->w;
	yIncr = (bounds.ymax - bounds.ymin) / surface->h;
	nextLine = 0;
	finished_jobs = 0;

	// Lancement des threads
	for (i = 0; i < nbThreads; ++i)
		sem_post(&working);  // reveille tous les threads travailleurs

	// Attendre threads
	sem_wait(&waiting);    // le dernier thread postera sur waiting

	// Affichage de fin
	gettimeofday(&end, NULL);
	double elapsed_time = (double) (end.tv_sec - start.tv_sec);
	elapsed_time += (double) (end.tv_usec - start.tv_usec)/ MICROSEC_IN_A_SEC;
	if (display) {
		printf("\rCalcul terminé en %2.3f secondes! ", elapsed_time);
		fflush(stdout); 
	}
}

void mandelbrot_close()
{
	pthread_mutex_destroy(&mutex);
	sem_destroy(&working);
	sem_destroy(&waiting);

	// annulation des threads
	pthread_cancel(watcher);
	int i;
	for (i = 0; i < nbThreads; ++i) 
		pthread_cancel(threads_id[i]);
	free(threads_id); 
}
