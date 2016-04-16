#!/bin/bash
######################################################################
# Archivo: matrix.sh
# Descripcion:
#  Corre el programa vector sobre cada archivo con terminacion .fna.gz 
#  en el subdirectorio $GENOMES, para construir una matriz compuesta 
#  por los vectores de probabilidades en el archivo $MATRIX.
#  Los archivos .fna.gz son "complete genome", no plasmid, no chromosome.
#  Genera un indice ordenado alfabeticamente por nombre de organismo
# Uso:
#  bash matrix2015.sh
# Autor: Alfredo Jose Hernandez Alvarez
# Fecha de ultima modificacion: Junio 2015
######################################################################

# archivo de salida
MATRIX=../data/matrix20160415ok
# directorio que contiene los archivos .fna.gz
GENOMES=../genomes/Bacteria

# primero obtener el listado, sacar la primera linea de cada archivo, 
# ordenarlas por nombre de bacteria, mandarlas a archivo .srt
echo -n "Generando lista..."
for f in ${GENOMES}/*.fna.gz 
do
    bn=$(zcat $f |grep "^>"|grep -v chromosome | grep -v plasmid |grep -v replicon|grep "complete genome"|head -1|cut -d' ' -f 2-)
    if [ -n "$bn" ] 
    then
      echo "$f|$bn"
    fi
done |sort -t '|' -k 2 > ${MATRIX}.srt
echo "Hecho."

# luego leer del archivo .srt, obtener el nombre del archivo de la secuencia, 
# calcularle el vector, mandarlo a matrix con su etiqueta
echo -n "Generando matriz..."
echo 16 > ${MATRIX}.dat
echo > ${MATRIX}.idx
#echo AA AC AG AT CA CC CG CT GA GC GG GT TA TC TG TT
i=1
while read fl
do
    f=$(echo $fl | cut -d'|' -f1)
    bn=$(echo $fl | cut -d'|' -f 2-|sed 's/^ //g;s/\[//g;s/\]//g')
    l1=$(echo "$bn"| cut -c 1-4); l2=$(echo "$bn"|cut -d' ' -f 2|cut -c 1-2); lbl=$l1$l2;
    zcat $f | ./vector >> ${MATRIX}.dat
    # usar $i para numerico, $lbl para etiqueta
    echo "$lbl" >> ${MATRIX}.dat
    echo "$i|$lbl|$f|$bn" >> ${MATRIX}.idx
    i=$(($i+1))
#    echo -n .
done < ${MATRIX}.srt
echo "Hecho."
