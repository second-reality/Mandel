# !bin/sh
# Exemple de creation de film de l'espace de Mandelbrot
# creation d'une video à partir d'images avec ffmpeg

xmin=-2.8183691   #bornes initiales
xmax=2.79216309    
ymin=-2.15654297
ymax=0.84345703
initRe=0.0        #initialisateur ensemble
initIm=0.0
width=1280
height=720
nbImages=900     # nombre d'image total
zoomFactor=200    #degre de zoom, plus il est grand, plus le zoom est lent
nbThreads=4

bornes="$xmin $xmax $ymin $ymax"
init="$initRe $initIm"
dimension="$width $height"
movie_param="$zoomFactor $nbImages"

mkdir movie
./mandel -c $movie_param --picture-name "movie/movie" -b $bornes -i $init -d $dimension -t $nbThreads
ffmpeg -r 30 -b 10000k -an -i movie/movie%d.bmp fractal_video.avi 
rm -rf movie
