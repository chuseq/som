#!/bin/bash
########################################################
# Archivo: plot.sh
# Descripcion:
#  Toma el archivo de salida del programa som y genera un
#  script para gnuplot
# Uso:
#  bash plot.sh SOMFILE
# Autor: Alfredo Jose Hernandez Alvarez
# Fecha de ultima modificacion: Marzo 2016
#########################################################

if [ $# == 0 ]
then
    echo Uso: $0 SOMFILE
    exit 1
fi

# SOMFILE es el archivo de salida de som
# es un archivo con columnas separadas por espacio, la 4a columna 
# es la etiqueta
SOMFILE=$1
if [ ! -f ${SOMFILE} ]
then
    echo $0: archivo ${SOMFILE} no existe
    exit 2
fi

# numero de clusters a mostrar, menor que el numero de colores
NUMCLUST=8
GPCOLORS=(red green blue orangered yellowgreen cyan brown black magenta coral darkred sienna)
# si PAUSE=0 entonces se genera automaticamente un png con la grafica
# si PAUSE=1, se abre la ventana de gnuplot (requiere ambiente X)
PAUSE=1
# nombres de archivos
FILENAME=$(basename $SOMFILE)
SOMGPDIR=${FILENAME}_gp
#TMPFILE=${SOMGPDIR}/${FILENAME}.tot
SOMGPFILE=${SOMGPDIR}/${FILENAME}.gp
SOMGPOUT=${SOMGPDIR}/${FILENAME}.png

mkdir -p ${SOMGPDIR}

# ordenar por etiqueta y acumulado de etiquetas, mandar a archivo temporal
#cat $SOMFILE | awk '{a[$4]++ } END {for (i in a) print i " " a[i]}'| sort -n -k2 -r  > ${TMPFILE}
# luego leer las etiquetas de tempo
#for l in $(cat ${TMPFILE} |cut -d' ' -f1| head -${NUMCLUST}); do grep $l ${SOMFILE} > ${SOMGPDIR}/${FILENAME}-$l; done
# se hace lo anterior tomando el tamano del archivo, se asume que los archivos mas grandes tienen mas puntos
for l in $(cat ${SOMFILE} | cut -d' ' -f4 | sort | uniq); do grep $l ${SOMFILE} > ${SOMGPDIR}/${FILENAME}-$l; done

i=0
echo > ${SOMGPFILE}
if [ $PAUSE == 0 ]
then
  echo set terminal png >> ${SOMGPFILE}
  echo set output \'${SOMGPOUT}\' >> ${SOMGPFILE}
fi
echo unset key >> ${SOMGPFILE}
echo -n "splot " >> ${SOMGPFILE}
for f in $(ls -l ${SOMGPDIR}/${FILENAME}-* |sort -k 5 -n -r | head -${NUMCLUST} | awk '{print $9}')
do
  c=${GPCOLORS[$i]}
  echo \'$f\' with labels point offset 1 tc rgb \'$c\', \\
  i=$(($i+1)) 
done >> ${SOMGPFILE}

if [ $PAUSE == 0 ]
then
  gnuplot ${SOMGPFILE}
else
  gnuplot -p ${SOMGPFILE}
fi

