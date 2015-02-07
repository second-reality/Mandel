#include <SDL/SDL.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "gfx.h"
#include "mandelbrot.h"
#include "options.h"
#include "types.h"

#define COLOR_DEPTH 32            // couleurs 32 bits

/*********************************************/
/*******    VARIABLES DU MODULE    ***********/
/*********************************************/

/* Données utilisées pour le rendu */
static SDL_Surface *surface;
static struct dimension dim; 

/* Paramètres du rendu */
static struct bounds bounds;      // bornes espace
static struct complex init;       // z0 (Mandelbrot) ou c (Julia)
static int nbMaxIt;               // nombre iterations max par point
static int julia;                 // Julia (1) ou Mandelbrot (0) ?

/*********************************************/
/*******        INITIALISATIONS    ***********/
/*********************************************/

/* Crée une fenêtre d'affichage et sa surface associée */
static void init_window() 
{
	int flags = SDL_DOUBLEBUF;
	if (options_getFullscreen()) 
		flags = SDL_FULLSCREEN;
	surface = SDL_SetVideoMode(dim.width, dim.height, COLOR_DEPTH, flags);
	if (surface == NULL) {
		printf("\nImpossible de créer une fenêtre\n"); exit(EXIT_FAILURE);
	}
}

/* Crée une surface (sans fenêtre) pour le rendu de l'ensemble */
static void init_noWindow() 
{
	surface = SDL_CreateRGBSurface(0, dim.width, dim.height, COLOR_DEPTH, 0, 0, 0, 0); 
	if (surface == NULL) {
		printf("\nImpossible d'obtenir une surface graphique\n"); exit(EXIT_FAILURE);
	}
}

/**********************************************/
/*******  MISE A JOUR DE LA SURFACE  **********/
/**********************************************/

/* Met à jour la surface via l'appel au moteur */
static void render() 
{
	mandelbrot_render(bounds, init, julia, nbMaxIt, surface);
}

/* Met a jour l'ecran en recalculant l'ensemble voulu */
static void refresh() 
{
	render();
	SDL_Flip(surface);
}

/* Sauvegarde la surface dans un fichier nom%num.bmp */
static void saveBMP(int num)
{
	char name[1024];
	sprintf(name, "%s%i.bmp", options_getPictureName(), num);
	SDL_SaveBMP(surface, name); 
}

/**********************************************/
/*******   TRAITEMENT DES EVENEMENTS **********/
/**********************************************/

/* Reinitialise la vue de l'ensemble */
static void resetView()
{
	bounds = options_getBounds();
	init = options_getInit();
	nbMaxIt = options_getNbMaxIt();
	julia = options_getJulia();
}

/* Deplace la vue d'1/20 de la distance entre les deux bornes d'un axe */
static void upView()
{
	const double depl = (bounds.ymax - bounds.ymin)/20;
	bounds.ymin -= depl;
	bounds.ymax -= depl;
}
static void downView()
{
	const double depl = (bounds.ymax - bounds.ymin)/20;
	bounds.ymin += depl;
	bounds.ymax += depl;
}
static void leftView()
{
	const double depl = (bounds.xmax - bounds.xmin)/20;
	bounds.xmin -= depl;
	bounds.xmax -= depl;
}
static void rightView()
{
	const double depl = (bounds.xmax - bounds.xmin)/20;
	bounds.xmin += depl;
	bounds.xmax += depl;
}

/* Zoom / Dezoom (facteur donné) sur (depuis) le centre de l'image */
static void zoomView(double factor)
{
	// pas de limite, exploitation de la précision maximale flottante
	factor *= 2;
	const double deplX = (bounds.xmax - bounds.xmin)/factor;
	const double deplY = (bounds.ymax - bounds.ymin)/factor;
	bounds.xmin += deplX; 
	bounds.xmax -= deplX; 
	bounds.ymin += deplY; 
	bounds.ymax -= deplY; 
}
static void unzoomView(double factor)
{
	const double deplX = (bounds.xmax - bounds.xmin)/factor;
	const double deplY = (bounds.ymax - bounds.ymin)/factor;
	bounds.xmin -= deplX; 
	bounds.xmax += deplX; 
	bounds.ymin -= deplY; 
	bounds.ymax += deplY; 
}

/* Modifie le nombre d'iterations d'un facteur donné */
static void incrIt(double factor) 
{
	nbMaxIt *= factor;
}
static void reduceIt(double factor)
{
	int newVal = nbMaxIt/factor;
	if (newVal < NBMAXIT_MIN)
		return;
	nbMaxIt = newVal;
}

/* Affiche les paramètres de l'image courante et une ligne pour
   générer la photo correspondante */
static void afficheParams()
{
	printf("\nBornes : %2.8f %2.8f %2.8f %2.8f\n", 
			bounds.xmin, bounds.xmax, bounds.ymin, bounds.ymax); 
	printf("Init : %2.2f %2.2f\n", init.real, init.im);
	printf("nbMaxIt : %d\n", nbMaxIt);
	if (julia) printf("Ensemble : Julia\n");
	else printf("Ensemble : Mandelbrot\n");
	printf("mandel -p");
	printf(" -b %2.8f %2.8f %2.8f %2.8f",
			bounds.xmin, bounds.xmax, bounds.ymin, bounds.ymax); 
	printf(" -i %2.2f %2.2f", init.real, init.im);
	printf(" -n %d", nbMaxIt);
	if (julia) printf(" -j");
	printf("\n");
}

/* Traite un événement clavier */
static void treatKeyDown(SDL_Event *event) 
{
	switch((*event).key.keysym.sym) {
		case SDLK_r:
			resetView(); break;
		case SDLK_f:
			mandelbrot_changeColors(); break;
		case SDLK_UP:
			upView(); break; 
		case SDLK_DOWN:
			downView(); break; 
		case SDLK_LEFT:
			leftView(); break; 
		case SDLK_RIGHT:
			rightView(); break; 
		case SDLK_t:
			zoomView(2.0); break; 
		case SDLK_g:
			unzoomView(2.0); break; 
		case SDLK_y:
			incrIt(1.5); break; 
		case SDLK_h:
			reduceIt(1.5); break; 
		case SDLK_u:
			init.real += 0.02; break; 
		case SDLK_j: 
			init.real -= 0.02; break; 
		case SDLK_i:
			init.im += 0.02; break; 
		case SDLK_k:
			init.im -= 0.02; break; 
		case SDLK_SPACE:
			julia = (julia+1) % 2; break; 
		case SDLK_p:
			afficheParams();
			return; 
		default:
			return; 
	}
	refresh(); 
}

/* Boucle de traitement des evenements */
static void gfxMainLoop() 
{
	printf("\r                                            ");
	printf("\n---- Mandelbrot - Controles ----\n");
	printf("Haut, Bas, Gauche, Droite : déplacer la vue\n");
	printf("r   : reinitialiser la vue\n");
	printf("f   : changer couleurs courantes de la fractale\n");
	printf("t/g : zoomer/dezoomer\n");
	printf("y/h : augmenter/réduire(*/1.5) le nombre d'itérations par point\n");
	printf("u/j : augmenter/diminuer (+-0.02) la partie réelle de init (c ou z0)\n");
	printf("i/k : augmenter/diminuer (+-0.02) la partie imaginaire de init (c ou z0)\n");
	printf("p : afficher les paramètres courants de l'image : \n");
	printf("    * Bornes de l'espace affiché : xmin xmax ymin ymax \n");
	printf("    * Init - c (Julia) ou z0 (Mandelbrot) : partie reelle, imaginaire\n");
	printf("    * Nombre d'itérations max par point - nbMaxIt\n");
	printf("    * Nom de l'ensemble : Julia ou Mandelbrot\n");
	printf("    La ligne supplémentaire affichée permet de generer une photo\n");
	printf("    Il est possible d'ajouter une largeur / hauteur\n");
	printf("Espace : Alterner entre l'espace de Mandelbrot et de Julia\n");
	printf("Echap : quitter le programme\n");
	printf("\n");
	printf("Utilisation de %d threads\n", options_getNbThreads());
	printf("\n");

	refresh();

	SDL_Event event;
	SDL_EnableKeyRepeat(100, 50);
	while (1) {
		SDL_WaitEvent(&event);
		switch (event.type) {
			case SDL_QUIT:
				return;
			case SDL_KEYDOWN:
				if(event.key.keysym.sym == SDLK_ESCAPE) 
					return;
				treatKeyDown(&event);
				break;
			default:
				break;
		}
	}
}

void gfx_start() 
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) < 0) {
		printf("\nSDL Initialization failed\n"); exit(EXIT_FAILURE);
	}
	signal(SIGINT, SIG_DFL);  // Empeche SDL d'intercepter CTRL-C

	mandelbrot_init(options_getNbThreads());
	dim = options_getDimension();
	resetView();
	if (options_getPhotoMode()) {
		init_noWindow();
		render();
		saveBMP(0);
	} else if (options_getCaptureMode()) {
		init_noWindow();
		mandelbrot_setDisplay(0);
		int i;
		double avancee;
		for (i = 0; i < options_getCaptureNbFrames(); ++i) {
			avancee = (double) (i*100)/options_getCaptureNbFrames();
			printf("\rGénération des images en cours... %2.1f %%    ", avancee);
			fflush(stdout); 
			render();
			saveBMP(i);
			zoomView(options_getCaptureZoomSpeed());
		}
	} else {
		init_window();
		gfxMainLoop();
	}

	mandelbrot_close();
	SDL_FreeSurface(surface);
	SDL_Quit();
	printf("\n");
}
