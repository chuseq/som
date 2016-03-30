#!/bin/bash
######################################################################
# Archivo: sompak.sh
# Descripcion:
#  Corre los programas del paquete SOM_PAK 
# Uso:
#  bash sompak.sh
# Autor: Alfredo Jose Hernandez Alvarez
# Fecha de ultima modificacion: Mayo 2015
######################################################################

# rutas
DATADIR=$HOME/Documents/academico/tesis/data
DATAFILE=matrix2015ok
DATAFILENAME=${DATADIR}/${DATAFILE}.dat
RESULTSDIR=$HOME/Documents/academico/tesis/results
PATH=$HOME/Documents/academico/tesis/programas/som_pak-3.1:$PATH
LOGFILE=$RESULTSDIR/sompak2015.log

#inicializacion del mapa
XDIM=30
YDIM=20
# rect|hexa
TOPOL=hexa
# gaussian|bubble
NEIGH=gaussian
RAND=0
#primera fase de entrenamiento
EPOCHS1=10000
ALPHA1=0.05
RADIUS1=5
#segunda fase
EPOCHS2=100000
ALPHA2=0.01
RADIUS2=1
#epochs para sammon
EPOCHSSAM=10000

DATERUN=$(date '+%Y%m%d%H%M%S')
RESULTSFILE=${DATAFILE}_${DATERUN}
# mandar al log
echo "[`date`] ${XDIM}x${YDIM} $TOPOL $NEIGH [$EPOCHS1,$ALPHA1,$RADIUS1] [$EPOCHS2,$ALPHA2,$RADIUS2] $EPOCHSSAM $RESULTSFILE" >> $LOGFILE
# crear directorio para resultados
RESULTSDIR=$RESULTSDIR/$RESULTSFILE
mkdir $RESULTSDIR
# inicio
echo "Etapa 1: iniciando mapa aleatorio"
randinit -din $DATAFILENAME -cout $RESULTSDIR/$RESULTSFILE.cod -xdim $XDIM -ydim $YDIM -topol $TOPOL -neigh $NEIGH -rand $RAND
echo "Etapa 2: entrenamiento"
echo "  Primera fase"
vsom -v 9 -din $DATAFILENAME -cin $RESULTSDIR/$RESULTSFILE.cod -cout $RESULTSDIR/$RESULTSFILE.cod -rlen $EPOCHS1 -alpha $ALPHA1 -radius $RADIUS1
echo "  Segunda fase"
vsom -v 9 -din $DATAFILENAME -cin $RESULTSDIR/$RESULTSFILE.cod -cout $RESULTSDIR/$RESULTSFILE.cod -rlen $EPOCHS2 -alpha $ALPHA2 -radius $RADIUS2
echo "Etapa 3: evaluacion del error"
qerror -din $DATAFILENAME -cin $RESULTSDIR/$RESULTSFILE.cod
echo "Etapa 4: visualizacion"
echo "  Calibracion"
vcal -din $DATAFILENAME -cin $RESULTSDIR/$RESULTSFILE.cod -cout $RESULTSDIR/$RESULTSFILE.cod
echo "  Generando coordenadas"
visual -din $DATAFILENAME -cin $RESULTSDIR/$RESULTSFILE.cod -dout $RESULTSDIR/$RESULTSFILE.coords
echo "  Planes"
planes -cin $RESULTSDIR/$RESULTSFILE.cod -plane 0 -din $DATAFILENAME -ps 1 # sin -ps genera eps
echo "  Umat"
umat -cin $RESULTSDIR/$RESULTSFILE.cod -average 1 -ps 1 > $RESULTSDIR/${RESULTSFILE}_um.ps # sin -ps genera eps otra opcion es -average 1 -median 1
echo "  Sammon"
sammon -cin $RESULTSDIR/$RESULTSFILE.cod -cout $RESULTSDIR/$RESULTSFILE.sam -rlen $EPOCHSSAM -ps 1 #puede ser ps o eps
echo "Hecho."
#fin
