#Reymond Pamelar, Tyler Nguyen
#cssc3703, cssc3761
#CS 530, Spring 2021
#Assignment #2, XE Link-Editor
#makefile

led: led.o 
	g++ led.o -o led
led.o: led.cpp 
	g++ -g -std=c++11 -c led.cpp 

clean: 
	rm *.o led