
#ifndef CONTAUDIO

#define CONTAUDIO

#include <iostream>
#include <vector>
#include "Audio.hpp"
#include <sparsehash/dense_hash_map>

using namespace std;


typedef struct RESAN
{
	string marcha;
	int resultado;
} resultadoAnalisis;

class ContenedorAudio
{
	private:
		vector<Audio> audios;
		google::dense_hash_map<long, vector<DataPoint> > db;

		int ncanciones;

		string ruta;

		void cargarFingerPrint(char * file);

	public:

		ContenedorAudio(){}

		ContenedorAudio(char * path);

		resultadoAnalisis matching(Audio * audio);

		resultadoAnalisis analizarMatch(google::dense_hash_map<string,google::dense_hash_map<int,int> > * matchMap);

		int size();


		static void crearBD(char * ruta, int fuz);
};

#endif