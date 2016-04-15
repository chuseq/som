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
MATRIX=../data/matrix2015lbl2
# directorio que contiene los archivos .fna.gz
GENOMES=../genomes/Bacteria

# primero obtener el listado, sacar la primera linea de cada archivo, 
# ordenarlas por nombre de bacteria, mandarlas a archivo .srt
echo -n "Ordenando entrada..."
for f in ${GENOMES}/*.fna.gz 
do
    zcat $f |grep "^>"|grep "complete genome"|grep -v chromosome | grep -v plasmid |head -1
done | sort -t '|' -k 5 > ${MATRIX}.srt
echo "Hecho."

# luego leer del archivo .srt, obtener el nombre del archivo de la secuencia, 
# calcularle el vector, mandarlo a matrix con su etiqueta
echo -n "Generando matriz"
echo 16 > ${MATRIX}.dat
echo > ${MATRIX}.idx
#echo AA AC AG AT CA CC CG CT GA GC GG GT TA TC TG TT
i=1
# e.g. fn=NC_019974
for fn in $(cat ${MATRIX}.srt | cut -d'|' -f4|cut -d'.' -f1)
do
    f=${GENOMES}/${fn}.fna.gz
    bname=$(zcat $f |head -1 |cut -d'|' -f5|sed 's/^ //g;s/\[//g')
    lbl=$(echo "$bname"| cut -c 1-4)
    zcat $f | ./vector >> ${MATRIX}.dat
    # usar $i para numerico, $lbl para etiqueta
    echo "$i" >> ${MATRIX}.dat
    echo "$i|$lbl|$fn|$bname" >> ${MATRIX}.idx
    i=$(($i+1))
    echo -n .
done
echo "Hecho."
