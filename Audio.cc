#include "Audio.hpp"

using namespace std;

/***************************************************************************/


void Audio::readHeader()
{
	int valfread;
	// Leemos la cabecera
    const int headerSize = sizeof(wav_hdr);
	valfread = fread(&header,headerSize,1,wavFile);
	rewind (wavFile);
}

/***************************************************************************/

void Audio::readSource()
{
	chunk chunkHeader;
	const int chunkSize = sizeof(chunkHeader);
	int valfread;
	
	/***********************************************************************/
    // leemos el RIFF, tamaño de archivo y WAVE
	valfread = fread(&chunkHeader,chunkSize,1,wavFile);
	char data[4];
	valfread = fread(&data,4,1,wavFile);

	// leemos el siguiente descriptor
	valfread = fread(&chunkHeader,chunkSize,1,wavFile);
	while(strncmp(chunkHeader.data, "data", 4) != 0)
	{
		// leemos la basura
		char basura[chunkHeader.size];
		valfread = fread(&basura,chunkHeader.size,1,wavFile);

		// leemos el siguiente descriptor
		valfread = fread(&chunkHeader,chunkSize,1,wavFile);
	}
	/***********************************************************************/

	
	int numOfChan = header.NumOfChan;
	int bitsPerSample = header.bitsPerSample;

	int size = chunkHeader.size;
	int s = size/sizeof(short);
	short* m = new short[s];

	// leemos los datos de audio
	valfread = fread(m,size,1,wavFile);

	// si está en estereo se convierte a mono
	if(numOfChan == 2) s = s/2;
	vector<short> muestras;
	if(numOfChan == 2)
	{
		//cout << "Fuente en stereo - Se convierte a mono" << endl;
		for(int i=0; i<s*2; i+=2)
		{
			muestras.push_back((m[i] + m[i+1]) / 2);
		}
	}

	// Al capturar en 16 bits por muestra el tipo leído es short, se define
	// con signo y su rango va desde -32.768 a 32.767
	if(bitsPerSample == 16)
	{
		if(numOfChan == 2)
		{
			for(int i=0; i<s; i++)
				source.push_back(muestras[i] / 32768.0);
		}
		else
		{
			source.clear();
			for(int i=0; i<s; i++)
				source.push_back(m[i] / 32768.0);
		}
	}
	else
	{
		cout << "\n\nERROR";
		cout << "\n\nEl archivo debe tener 16 bits por sample\n" << endl;
	}

	delete[] m;
	//cout << "m liberado" << endl;
}

/***************************************************************************/

Audio::Audio(char * path, int fuz)
{
	existe = true;
	wavFile = fopen(path , "r");
	if(wavFile == NULL){
		existe = false;
	}

	if(existe)
	{

		fingerPrint.set_empty_key(NULL);

		string n = path;

		name = n.substr(n.size()-15);
		name.resize(11);
		//name = n;

		/***********************************************************************/
		// Static variables
		UPPER_LIMIT = 300;
		LOWER_LIMIT = 40;

		RANGE[0] = 40;
		RANGE[1] = 80;
		RANGE[2] = 120;
		RANGE[3] = 180;
		RANGE[4] = UPPER_LIMIT + 1;

		//Using a little bit of error-correction, damping
		FUZ_FACTOR = fuz;
		/***********************************************************************/

		// Leemos la cabecera
	    readHeader();

		// Leemos el source
		readSource();

		fclose(wavFile);
	}
}
/***************************************************************************/


vector<double> Audio::getSource()
{
	return source;
}


/***************************************************************************/


void Audio::makeFingerPrint()
{
	// hash map donde guardamos la fingerprint
	google::dense_hash_map<long, vector<DataPoint> > hashMap;

	hashMap.set_empty_key(NULL);

	int totalSize = source.size();

	int CHUNK_SIZE = 2048;

	int amountPossible = totalSize/CHUNK_SIZE;

	// Para cambiar al dominio de la frecuencia necesitamos números complejos
	Complex ** results;

	results = new Complex *[amountPossible];
	for(int i = 0; i<amountPossible; i++)
		results[i] = new Complex[CHUNK_SIZE];

	// Para todos los chunks
	for(int times = 0;times < amountPossible; times++) {
		Complex complex[CHUNK_SIZE];
	    for(int i = 0;i < CHUNK_SIZE;i++) {
	        // Ponemos los datos en el dominio del tiempo en un número complejo con
	        // la parte imaginaria a 0
	        complex[i] = Complex(source[(times*CHUNK_SIZE)+i], 0);
	    }
	    // Análisis FFT para el chunk leído
	    Complex * array = fft(complex, CHUNK_SIZE);
		for(int i=0; i<CHUNK_SIZE; i++)
		    results[times][i] = array[i];

		delete [] array;
	}


	/***********************************************************************/
	// Creación de fingerprint
	double ** highscores;
	long ** recordPoints;

	highscores = new double* [amountPossible];
	for(int i = 0; i<amountPossible; i++)
		highscores[i] = new double[5];
	recordPoints = new long* [amountPossible];
	for(int i = 0; i<amountPossible; i++)
		recordPoints[i] = new long[5];


	for (int t = 0; t < amountPossible; t++)
	{
	    for (int freq = LOWER_LIMIT; freq < UPPER_LIMIT - 1; freq++)
	    {
			// Obtenemos la magnitud
			double mag = log(results[t][freq].abs() + 1);

			// Buscamos en qué rango está
			int index = getIndex(freq);

			// Guardamos la magnitud máxima y su correspondiente frecuencia
			if (mag > highscores[t][index])
			{
			    highscores[t][index] = mag;
			    recordPoints[t][index] = freq;
			}
	    }

	    // creamos el valor hash
	    long h = hash(recordPoints[t][0], recordPoints[t][1], recordPoints[t][2], recordPoints[t][3]);

		if(h == 0)
			h = 1;

		// lo añadimos al hash map
		if (fingerPrint[h].size() == 0)
	    {
			vector<DataPoint> listPoints;
			DataPoint point;
			point.time = t;
			point.songId = name;
			listPoints.push_back(point);

			fingerPrint[h] = listPoints;
	    }
	    else
	    {
			DataPoint point;
			point.time = t;
			point.songId = name;

			fingerPrint[h].push_back(point);
	    }
	}
	/***********************************************************************/

	for(int i=0; i<amountPossible; i++)
		delete [] results[i];
	delete [] results;

	//cout << "results liberado" << endl;

	for(int i=0; i<amountPossible; i++)
		delete [] highscores[i];
	delete [] highscores;

	//cout << "highscores liberado" << endl;

	for(int i=0; i<amountPossible; i++)
		delete [] recordPoints[i];
	delete [] recordPoints;

	//cout << "recordPoints liberado" << endl;
}

/***************************************************************************/

void Audio::makeAndSaveFingerPrint(string ruta)
{
	makeFingerPrint();
	writeFingerPrint(ruta);
}

/***************************************************************************/

google::dense_hash_map<long, vector<DataPoint> > Audio::getFingerPrint()
{
	return fingerPrint;
}

/***************************************************************************/

void Audio::writeFingerPrint(string ruta)
{
	/***********************************************************************/
	// Guardamos la fingerprint
	ofstream fo;
	string f = ruta + name;
	fo.open(f.c_str());
	for (google::dense_hash_map<long,vector<DataPoint> >::iterator it=fingerPrint.begin(); it!=fingerPrint.end(); ++it)
	{
		fo << it->first << endl << "{" << endl;
		for(int i=0; i<it->second.size(); i++)
			fo << "\t[" << it->second[i].songId << "|" << it->second[i].time << "]" << endl;
		fo << "}" << endl;
	}
	fo.close();
	/***********************************************************************/
}

/***************************************************************************/

int Audio::getIndex(int freq)
{
	int i = 0;
	while (RANGE[i] < freq)
		i = i+1;
	return i;
}

/***************************************************************************/

long Audio::hash(int p1, int p2, int p3, int p4)
{
	return (p4 - (p4 % FUZ_FACTOR)) * 100000000 + (p3 - (p3 % FUZ_FACTOR)) * 100000 + (p2 - (p2 % FUZ_FACTOR)) * 100 + (p1 - (p1 % FUZ_FACTOR));
}

/***************************************************************************/

string Audio::getName()
{
	return name;
}

/***************************************************************************/

bool Audio::exists()
{
	return existe;
}

/***************************************************************************/