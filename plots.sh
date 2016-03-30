#!/bin/bash

# obtiene los archivos plots que contienen el vector de probabilidades 
# de 16 en columna para pasarlo a la funcion plot de gnuplot
# un archivo con el mismo nombre que la bacteria
# Uso: plots.sh
FILES=$(find genomes -name "*.fna")
lbl=0
for f in $FILES
do
#  PLASMID=$(head -1 $f | sed '/plasmid/ { d }')
#  if test -n "$PLASMID"; then
  echo $f
# label numerado
#  lbl=$(expr $lbl + 1)
# label con las 3 primeras letras de las dos primeras palabras del nombre del directorio
#  lbl=$(echo $f|awk 'BEGIN {FS="/"} {print $3}'|awk 'BEGIN {FS="_"} {print substr($1,1,3) "_" substr($2,1,3)}')
  lbl=$(echo $f|sed 's/.fna//g' |awk 'BEGIN {FS="/"} {print $3 "-" $4}')
  cat $f | ./vector | sed 's/ /\n/g' > plots/${lbl}
#  fi # plasmid
done
#real    2m49.262s
#user    1m58.524s
#sys     0m9.782s
