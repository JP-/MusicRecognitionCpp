/*
 *
 * Servidor que escucha llamadas para reconocer audio
 *
 */


#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <vector>
#include "Complex.hpp"
#include "FFT.hpp"
#include "MySQL.hpp"
#include "Audio.hpp"
#include "ContenedorAudio.hpp"
#include <sys/types.h>
#include <dirent.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <thread>

using namespace std;

/****************************************************************/

ContenedorAudio cont;
int fuz = 4;


void procesarCliente(int client, char * ip)
{
	int si;
	int size;
	char * key = new char[11];
	
	recv(client, key, 11, 0);
	
	resultadoAnalisis result;
	string str(key);
	str.resize(11);
	string k = str;

	// directorio de subida de audio
	str = "/home/cophrade/uploads/" + str;
	
	Audio wav((char *) str.c_str(), fuz);

	if(wav.exists())
	{
		result = cont.matching(&wav);
		//consultarBD(result);

		// valor mínimo para devolver un resultado
		if(result.resultado > 20)
		{		
			char * buffer2 = new char[11];
			for(int i=0; i<result.marcha.size(); i++)
				buffer2[i] = result.marcha[i];
			send(client, buffer2, 11, 0);
		}
		else
		{
			// no se ha obtenido un resultado mínimo, se devuelve "notmin"
			char * buffer2 = new char[6];
			buffer2[0] = 'n';
			buffer2[1] = 'o';
			buffer2[2] = 't';
			buffer2[3] = 'm';
			buffer2[4] = 'i';
			buffer2[5] = 'n';
			send(client, buffer2, 6, 0);
		}

		nuevaEscucha(k, result, ip);
	}
	else
	{
		// se ha producido un error, se devuelve "error"
		char * buffer2 = new char[5];
		buffer2[0] = 'e';
		buffer2[1] = 'r';
		buffer2[2] = 'r';
		buffer2[3] = 'o';
		buffer2[4] = 'r';
		send(client, buffer2, 5, 0);
	}

	
	close(client);	
}


int main(int argc,char *argv[])
{

	bool crearbd = false;	

	// si la bd de fingerprints ya está creada
	if(!crearbd)
	{
		// se le pasa la ruta donde se almacenan las fingerprints
		cont= ContenedorAudio(argv[1]);

		cout << "BD cargada con " << cont.size() << " fingerprints\n" << endl;


		// inicialización bd mysql
		initMySQL();

		
		// Inicialización Server
		/*********************************************************************/
		int client, server;
	    int portNum = 8080;
	    struct sockaddr_in server_addr, client_addr;
	    socklen_t size2;
	    client = socket(AF_INET, SOCK_STREAM, 0);
	    if (client < 0)
	    {
	        cout << "\nError establishing socket..." << endl;
	        exit(1);
	    }
	    server_addr.sin_family = AF_INET;
	    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
	    server_addr.sin_port = htons(portNum);
	    if ((bind(client, (struct sockaddr*)&server_addr,sizeof(server_addr))) < 0)
	    {
	        cout << "=> Error binding connection, the socket has already been established..." << endl;
	        return -1;
	    }
	    size2 = sizeof(server_addr);
		/*********************************************************************/
		

		/*********************************************************************/
		// esperamos clientes
		// 10 clientes en paralelo como mucho
		while(true)
		{
			vector<thread*> hebras;
			for(int i=0; i<10; i++)
			{
			    listen(client, 1);
				server = accept(client, (struct sockaddr *)&client_addr, &size2);

				cout << "Nuevo cliente" << endl;

				thread *t1 = new thread(procesarCliente, server, inet_ntoa(client_addr.sin_addr));

				hebras.push_back(t1);
			}

			for(int i=0; i<10; i++)
			{
				hebras[i]->join();
			}
		}
		/*********************************************************************/
	}
	else
	{
		ContenedorAudio::crearBD(argv[1], fuz);
	}

    return 0;
}
