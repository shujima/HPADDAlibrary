ADtest: HPADDAboard.c main.c HPADDAboard.h 
	gcc -o Test HPADDAboard.c HPADDAboard.h main.c -lbcm2835 -lm
