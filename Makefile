#
# Makefile for vector.c and som.c
# 
# Autor: Alfredo Jose Hernandez Alvarez
CC=gcc

all: vector som

vector: vector.c
	$(CC) -o $@ $<

som: som.c
	$(CC) -lm -o $@ $<

clean:
	rm som vector

