

all: Server



Complex.o: Complex.cc Complex.hpp
	
	g++ -c -o Complex.o Complex.cc -O2 -std=c++11

Server.o: Server.cc FFT.hpp Complex.hpp MySQL.hpp

	g++ -c -o Server.o Server.cc -O2 -std=c++11 -lmysqlclient -pthread

Audio.o: Audio.cc Audio.hpp 

	g++ -c -o Audio.o Audio.cc -O2 -std=c++11

ContenedorAudio.o: ContenedorAudio.cc ContenedorAudio.hpp

	g++ -c -o ContenedorAudio.o ContenedorAudio.cc -O2 -std=c++11

Server: Server.o Complex.o Audio.o ContenedorAudio.o
	
	g++ -o Server Server.o Complex.o Audio.o ContenedorAudio.o -O2 -std=c++11 -lmysqlclient -pthread
	
clean:

	rm *.o
	
