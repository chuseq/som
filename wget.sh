#!/bin/bash
###############################################
# Archivo: wget.sh
# Descripcion:
#  Script para descargar los archivos de las secuencias fasta de genomas 
#  desde el servidor FTP del NCBI
# Uso: bash wget.sh
# Autor: Alfredo Jose Hernandez Alvarez
# Fecha de ultima modificacion: Abril 2016
######################################################################
# Before NCBI ftp site reorganization
#wget -r -nH -g on -A .fna ftp://ftp.ncbi.nlm.nih.gov/genomes/Bacteria/
# Post December 2015 after NCBI ftp site reorganization
# To download the historic, not updated anymore data, use the following url with the above wget command:
# ftp://ftp.ncbi.nlm.nih.gov/genomes/archive/old_refseq/Bacteria/
# With the previous commands we download all available .fna files from the ftp, but we dont use all (see matrix.sh script)
# To download the "new" ones:
# See ftp://ftp.ncbi.nlm.nih.gov/pub/factsheets/HowTo_Downloading_Genomic_Data.pdf
wget ftp://ftp.ncbi.nlm.nih.gov/genomes/refseq/bacteria/assembly_summary.txt 
# We just want "complete genomes", so instead of 60000 files, we only download aprox 4700
cat assembly_summary.txt| grep "Complete Genome"| awk '{FS="\t"} !/^#/ {print $20}' | sed -r 's|(ftp://ftp.ncbi.nlm.nih.gov/genomes/all/)(GCF_.+)|\1\2/\2_genomic.fna.gz|' > bacteria-fna-urls.txt
wget -i bacteria-fna-urls.txt
# Warning: aprox 5GB of disk space needed as of April 2016
#FINISHED --2016-04-15 16:16:34--
#Downloaded: 4727 files, 4.8G in 1h 30m 54s (930 KB/s)
