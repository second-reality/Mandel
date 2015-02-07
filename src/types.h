#ifndef TYPES_H
#define TYPES_H

/* Dimension : {width, height} 
   - width : largeur pour le calcul(précision horizontale) / rendu / affichage
   - height : hauteur pour le calcul(précision verticale) / rendu / affichage */
struct dimension {
	int width, height;
};

/* Bounds : {Xmin, Xmax, Ymin, Ymax}
   - [XY]({min}|{max}) : bornes d'un espace */
struct bounds {
	double xmin, xmax, ymin, ymax;
};

/* Complex : {real, im} */
struct complex {
	double real, im;
};

#endif
