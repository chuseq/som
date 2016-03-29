/***********************************************************************
 * Archivo: som.c
 * Descripcion:
 *  Programa que implementa el algoritmo SOM de Kohonen.
 *  Recibe como entrada un archivo que contiene en la primera linea la 
 *  dimension d, y a continuacion n vectores de dimension d uno por linea
 *  Manda como salida a un archivo o salida estandar una lista de coordenadas 
 *  en R3 para cada uno de los n vectores
 * Uso:  
 *  som [-x NROWS] [-y NCOLS] [-e EPOCHS] [-a ALPHA0] [-s SIGMA0] \
 *      [-o ARCHIVOSALIDA] [-l LOGLEVEL] [-r] archivoentrada
 * donde:
 *  -x NROWS: es la dimension x del mapa, es un valor entero
 *  -y NCOLS: es la dimension y del mapa, es un valor entero
 *  -e EPOCHS: es el numero de epocas o iteraciones, es un valor entero
 *  -a ALPHA0: es el factor de aprendizaje inicial, es un valor real
 *  -s SIGMA0: es el valor sigma inicial para calcular el radio de la 
 *     vecindad, es un valor real
 *  -o ARCHIVOSALIDA: el nombre del archivo de salida opcional
 *  -l LOGLEVEL: si es el valor 1 se guarda un registro de la ejecucion y 
 *     los parametros en un archivo de bitacora. Si el valor es 0, no se 
 *     guarda registro. Si no se indica, si se guarda registro.
 *  -r : indica que la siguiente entrada en cada epoca se tomara de forma 
 *     aleatoria. Si no se indica se hace una seleccion secuencial.
 *  archivoentrada: es el nombre del archivo de de la matriz de datos de
 *     entrada generado con el script matrix.sh
 * Autor: Alfredo Jose Hernandez Alvarez
 * Fecha de ultima modificacion: Enero 2016
 ********************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

int main(int argc, char *argv[]) {
  int 
    epochs=-1,
    nrows=-1,
    ncols=-1;
  double 
    alpha0=0.3, 
    sigma0=-1;
  char *data, *results, *log="som.log";
  double ***m;
  double **x;
  char **l;
  double f, alpha, sigma, di, bmudi, radius, r, h, qe;
  int t, xi, bmui, bmuj, d, n;
  int i, j, k, brnd=0, logl=1;
  char c, tmp[64], tmp2[64], *fecha;
  FILE *fp, *fpo, *fpl;
  
  /* procesamiento de parametros */
  while (--argc>0 && (*++argv)[0]=='-') {
    if (c=*++argv[0]) {
      switch(c) {
      case 'x': 
	nrows=atoi(*++argv); --argc;
	break;
      case 'y': 
	ncols=atoi(*++argv); --argc;
	break;
      case 'e': 
	epochs=atoi(*++argv); --argc;
	break;
      case 'a': 
	alpha0=atof(*++argv); --argc;
	break;
      case 's': 
	sigma0=atof(*++argv); --argc;
	break;
      case 'r': /* siguiente x aleatoria, si no, secuencial */
	brnd=1;
	break;
      case 'l': 
	logl=atoi(*++argv); --argc;
	break;
      case 'o':
	results=*++argv; --argc;
	break;
      } /* switch */
    } /* while */
  } /* while */
  data=*argv;
  if (argc==0 || data==NULL) {
    fprintf(stderr,"Uso: som [-x NROWS] [-y NCOLS] [-e EPOCHS] [-a ALPHA0] [-s SIGMA0] [-o ARCHIVOSALIDA] [-l LOGLEVEL] [-r] archivo\n");
    exit(2);
  }
  if (results==NULL) {
    strcpy(tmp2, (strrchr(data,'/')!=NULL)?strrchr(data,'/')+1:data );
    for(i=0,c=tmp2[i];c!='.' && c!='\0';i++,c=tmp2[i]) ; 
    tmp2[i]='\0';
    results=strcat(tmp2,".coords");
  }
  /* leer archivo de datos */
  fp=fopen(data,"r");
  if(fp==NULL) {
    fprintf(stderr,"No se puede abrir el archivo de datos %s\n",data);
    exit(3);
  }
  fscanf(fp,"%d\n",&d); /*lee la dimension*/
/* x = matriz de entrada 2 dimensiones al final nxd */
  x=(double **)malloc(1*sizeof(double *));
/* x0 vector tamano d */
  x[0]=(double *)malloc(d*sizeof(double));
/* labels 64 chars */
  l=(char **)malloc(1*sizeof(char *));
  l[0]=(char *)malloc(64*sizeof(char));
  i=0; j=0; n=0;
  while((c=getc(fp))!=EOF) { /* leemos los datos */
    if(c!='\n' && c!=' ') {
      tmp[j++]=c;
    } else {
      tmp[j]='\0'; 
      if(i<d){ /* valor */
	x[n][i]=atof(tmp);
	i++;      
      } else { /* label */
	strncpy(l[n],tmp,strlen(tmp)+1);
      }
      j=0;
      if(c=='\n') { /* nueva linea */
	n++; 
	i=0; 
	x=(double **)realloc(x,(n+1)*sizeof(double *));
	x[n]=(double *)malloc(d*sizeof(double));
	l=(char **)realloc(l,(n+1)*sizeof(char *));
	l[n]=(char *)malloc(64*sizeof(char));
      }
    }
  }  /*while*/
  fclose(fp);
  /* valores por default si no los dan */
  if(epochs==-1) { epochs=n*10; }
  if(nrows==-1) { nrows=(int)sqrt(n); }
  if(ncols==-1) { ncols=nrows; }
  if(sigma0==-1) { sigma0=nrows/2.0; }
  /* inicio aleatorio del mapa */
  srand(time(NULL));
  m=(double ***)malloc(nrows*sizeof(double **));
  for (i=0;i<nrows;i++) {
    m[i]=(double **)malloc(ncols*sizeof(double *));
    for (j=0;j<ncols;j++) {
      m[i][j]=(double *)malloc(d*sizeof(double));
      for (k=0;k<d;k++) {
	m[i][j][k]=(double)(1.0*rand()/(RAND_MAX+1.0));
      }
    } /*for j*/
  } /*for i*/
  /* ciclo de epochs */
  for(t=0;t<epochs;t++) {
    if(brnd==1) { xi=1+(int)(n*(rand()/(RAND_MAX+1.0))); /* seleccion aleatoria del siguiente vector de entrada */ }
    else { xi=t%n; /* seleccion secuencial */ }  
    /* encontrar la bmu usando distancia euclidiana */
    bmui=0; bmuj=0; bmudi=1000000.0;
    for (i=0;i<nrows;i++) {
      for (j=0;j<ncols;j++) {
	di=0.0;
	for (k=0;k<d;k++) {
	  f=x[xi][k]-m[i][j][k];
	  di+=f*f; 
	}	       
	if (di<bmudi) {
	  bmui=i; bmuj=j; bmudi=di;
	}
      }
    } /* bmu */ 
    /* actualizar vecindario de la bmu */
    /* factor de aprendizaje alpha */
    alpha=alpha0*(1.0-((double)t/(double)epochs));  /* lineal */
    /*alpha=1.0+(alpha0-1.0)*(double)(epochs-t)/(double)epochs;*/
    /*alpha=alpha0/(t+1.0);*/               /* inversa */
    /*alpha=alpha0*exp(t/-10.0);*/      /* exponencial */
    /*alpha=alpha0*exp(t/-(epochs/log(sqrt(nrows*nrows+ncols*ncols)/2.0))); */
    /*alpha=alpha0*exp(t/-(epochs/log((nrows+ncols)/2.0)));*decrece un poco mas rapido*/
    /*alpha=alpha0*exp(t/-(epochs/log((nrows*ncols)/2.0)));*decrece aun mas rapido*/
    
    /* sigmai para el radio */
    sigma=sigma0*(1.0-((double)t/(double)epochs));  /* lineal */
    /*sigma=1.0+(sigma0-1.0)*(double)(epochs-t)/(double)epochs;*/
    /*sigma=sigma0/(t+1.0);*/           /* inversa */
    /*sigma=sigma0*exp(t/-10.0);      * exponencial */
    /*sigma=sigma0*exp(t/-(epochs/log(sqrt(nrows*nrows+ncols*ncols)/2.0))); */
    /*sigma=sigma0*exp(t/-(epochs/log((nrows+ncols)/2.0)));*decrece un poco mas rapido*/
    /*sigma=sigma0*exp(t/-(epochs/log((nrows*ncols)/2.0)));*decrece aun mas rapido*/

    radius=-2.0*sigma*sigma;
    /*    printf("alpha=%f sigma=%f radius=%f\n",alpha,sigma,radius);*/

    /* actualizar los nodos en el vecindario 
       aqui actualizamos todos los nodos del mapa usando un vecindario gaussiano, es decir, la actualizacion esta ponderada de acuerdo a la distancia al nodo
       pero podriamos actualizar solamente los necesarios de acuerdo a si la vecindad es rectangular o hexagonal
       Dado i,j = bmui,bmuj
       - si es rectangular, solo [i-1,j], [i+1,j], [i,j-1], [i,j+1]
       - si es hexagonal, [i-1,j], [i+1,j], [i,j-1], [i,j+1], [i-1,j-1], [i-1,j+1]
       podemos hacer una funcion que regrese 0 si esta en el vecindario y que regrese h si no
    */
    for (i=0;i<nrows;i++) {
      for (j=0;j<ncols;j++) {
	r=0.0;
	/* r = distancia de x=i,j a la bmu=bmui,bmuj */
	f=bmui-i; r+=f*f;
	f=bmuj-j; r+=f*f;
	h=exp(r/radius);
	for (k=0;k<d;k++) {
	  m[i][j][k] += alpha * h * (x[xi][k] - m[i][j][k]);
	}	       
      }
    } /* actualizacion */ 
  } /* for epochs */

  /* resultados */
  fpo=fopen(results,"w");
  if(fpo==NULL) {
    fprintf(stderr,"No se puede abrir el archivo de salida %s\n",results);
    exit(4);
  }
  /* calculo del error */
  qe=0.0;
  for(xi=0;xi<n;xi++) {
    bmui=0; bmuj=0; bmudi=1000000.0;
    for (i=0;i<nrows;i++) {
      for (j=0;j<ncols;j++) {
	di=0.0;
	for (k=0;k<d;k++) {
	  f=x[xi][k]-m[i][j][k];
	  di+=f*f; 
	}	       
	if (di<bmudi) { /* x,y == i,j */
	  bmui=i; bmuj=j; bmudi=di;
	}
      }
    } 
    qe+=bmudi;
    fprintf(fpo,"%f %d %d %s\n",bmudi,bmui,bmuj,l[xi]);
  } /* for */
  qe/=n;
  fclose(fpo);

  /* salida a archivo bitacora */
  if (logl==1) {
    fpl=fopen(log,"a+");
    if(fp==NULL) {
      fprintf(stderr,"No se puede abrir el archivo bitácora %s\n",log);
      /*      exit(5);*/
    } else {
      fprintf(fpl,"d=%d n=%d ",d,n);
      fprintf(fpl,"map=%dx%d epochs=%d ",nrows,ncols,epochs);
      fprintf(fpl,"next=%s ",(brnd==1)?"rnd":"seq");
      fprintf(fpl,"alpha0=%f sigma0=%f ",alpha0,sigma0);
      fprintf(fpl,"qe=%f ",qe);
      fprintf(fpl,"%s %s ",data, results);
      fprintf(fpl,"\n");
      fclose(fpl);
    }
  } /* logl */

  /* salida a pantalla */
  fprintf(stderr,"d=%d n=%d ",d,n);
  fprintf(stderr,"map=%dx%d epochs=%d ",nrows,ncols,epochs);
  fprintf(stderr,"next=%s ",(brnd==1)?"rnd":"seq");
  fprintf(stderr,"alpha0=%f sigma0=%f ",alpha0,sigma0);
  fprintf(stderr,"qe=%f ",qe);
  fprintf(stderr,"%s %s ",data, results);
  fprintf(stderr,"\n");
  /* free(m); free(l); free(x); free(data); free(results); */
  exit(0);
} /*main*/

