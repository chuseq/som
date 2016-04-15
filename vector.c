/*******************************************************************
 * Archivo: vector.c 
 * Descripcion:
 *  Programa en C que recibe de la entrada estandar una secuencia 
 *  genomica en formato FASTA y manda a la salida estandar su vector 
 *  de probabilidades cij de 16 componentes. Solo se toman en cuenta 
 *  los simbolos A, C, G, T
 * Uso:
 *    cat entrada.fa | ./vector > salida.dat
 * Autor: Alfredo Jose Hernandez Alvarez
 * Fecha de ultima modificacion: Enero 2016
 ********************************************************************/

#include <stdio.h>

int base(int b) {
  switch (b) {
  case 'A': return 0;
  case 'C': return 1;
  case 'G': return 2;
  case 'T': return 3;
  }
  return -1;
}
int label(int b) {
  switch (b) {
  case 0: return 'A';
  case 1: return 'C';
  case 2: return 'G';
  case 3: return 'T';
  }
  return ' ';
}
int main () {
  int i, j;
  int c, p=0, n=0, 
    /* ni es un arreglo de dimension 4 para guardar el numero de ocurrencias de las bases */
    ni[4]={0,0,0,0}, 
    /* nij es una matriz de 4x4 para guardar el numero de ocurrencias de los dinucleotidos o pares de bases */
    nij[4][4]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    /* fi es para la frecuencia de las bases
       fij para las frecuencias de los dinucleotidos */
  double fi[4], fij[4][4], cij[4][4];
  enum { A, C, G, T };
  /* saltamos la primera linea del archivo*/
  while (getchar()!='\n');
  /* leemos la secuencia
     OJO: solamente tomamos en cuenta las letras o bases A C G T
   */
  while ((c=getchar())!=EOF) {
    if (c=='A' || c=='C' || c=='G' || c=='T') {
      n++;
      /* incrementamos el contador de cada base */
      ni[base(c)]++;
      if (p!=0) { 
	/* incrementamos el contador de los dinucleotidos que se van encontrando */
	nij[base(p)][base(c)]++;
      } /*if*/
      p=c;
    } /*if c*/
  } /*while getchar*/
  for(i=0; i<4; i++) {
    fi[i]=(double)ni[i]/n;
  }
  /* se calcula cij y  mandamos a pantalla */
  for(i=0; i<4; i++) {
    for(j=0; j<4; j++) {
      fij[i][j] = (double)nij[i][j] / (n-1);
      cij[i][j] = fij[i][j] - fi[i]*fi[j];
      printf("%f ",cij[i][j]);
    } /*for j*/
  } /*for i*/
  exit(0);
}
