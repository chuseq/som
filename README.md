# som
Basic Self Organizing Map implementation for genome analysis

This repository contains files for the thesis work titled "Self Organizing Map for genome analysis".

## som.c
Contains a basic implementation of Kohonen's SOM algorithm.

## vector.c
Contains code to generate a vector representation of a genomic DNA sequence in fasta format. It uses frequencies of dinucleotides. The resulting vector has dimension 16.

## matrix.sh
Bash code to generate a matrix using the program `vector` on a set of genomic sequences

