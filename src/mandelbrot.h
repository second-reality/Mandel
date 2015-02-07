#ifndef MANDELBROT_H
#define MANDELBROT_H

#include <SDL/SDL.h>

#include "types.h"

/* Moteur multithreadé de calcul de l'espace de Mandelbrot (et de Julia)*/

/* Initialise les données du moteur 
   - _nbThreads : nombre de threads à utiliser */
void mandelbrot_init(int _nbThreads);  

/* Active/Desactive l'affichage de l'avancée et du temps de calcul 
   Par defaut, affichage activé (1) */
void mandelbrot_setDisplay(int boolean);

/* Change les couleurs de la fractale au hasard */
void mandelbrot_changeColors();

/* Réalise le rendu de l'ensemble de Mandelbrot ou de Julia dans la surface s
   - _bounds : bornes de l'espace
   - _init : complexe initialisateur - c (Julia) ou z0 (Mandelbrot)
   - _julia : vaut 1 si on veut l'ensemble de Julia, 0 pour Mandelbrot
   - _nbMaxIt : nombre d'iterations max pour chaque point de l'espace 
   - _surface : surface SDL dans laquelle rendre l'ensemble */ 
void mandelbrot_render(struct bounds _bounds, struct complex _init, 
		int _julia, int _nbMaxIt, SDL_Surface *_surface); 

/* Libère les données du moteur */
void mandelbrot_close();

#endif
