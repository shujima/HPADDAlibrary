ADtest: HPADDAlibrary.c main.c HPADDAlibrary.h 
	gcc -o Test HPADDAlibrary.c HPADDAlibrary.h main.c -lbcm2835 -lm
