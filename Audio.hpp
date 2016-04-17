
#ifndef AUDIO

#define AUDIO

#include <iostream>
#include <vector>
#include <sparsehash/dense_hash_map>
#include <cmath>
#include <fstream>
#include <string>
#include <cstring>
#include "Complex.hpp"
#include "FFT.hpp"

using namespace std;

typedef struct  WAV_HEADER{
    char              	RIFF[4];        // RIFF Header      Magic header
    int       			ChunkSize;      // RIFF Chunk Size
    char               	WAVE[4];        // WAVE Header
    char               	fmt[4];         // FMT header
    int      			Subchunk1Size;  // Size of the fmt chunk
    unsigned short     	AudioFormat;    // Audio format 1=PCM,6=mulaw,7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
    unsigned short     	NumOfChan;      // Number of channels 1=Mono 2=Stereo
    int      			SamplesPerSec;  // Sampling Frequency in Hz
    int       			bytesPerSec;    // bytes per second
    unsigned short     	blockAlign;     // 2=16-bit mono, 4=16-bit stereo
    unsigned short     	bitsPerSample;  // Number of bits per sample

}wav_hdr;

typedef struct  CHUNK
{
	char        		data[4];
	int					size;
} chunk;

typedef struct  DATAPOINT
{
	int        		time;
	string			songId;
} DataPoint;


class Audio
{
	private:
		wav_hdr header;
		vector<double> source;
		FILE *wavFile;
		google::dense_hash_map<long, vector<DataPoint> > fingerPrint;
		string name;

		int UPPER_LIMIT;
		int LOWER_LIMIT;

		bool existe;

		int RANGE[5];

		//Using a little bit of error-correction, damping
		int FUZ_FACTOR;

		void readHeader();
		void readSource();
		int getIndex(int freq);
		long hash(int p1, int p2, int p3, int p4);

	public:

		Audio(){}

		Audio(char * path, int fuz);

		vector<double> getSource();

		void makeFingerPrint();

		google::dense_hash_map<long, vector<DataPoint> > getFingerPrint();

		void writeFingerPrint(string ruta);

		void makeAndSaveFingerPrint(string ruta);

		string getName();

		bool exists();
};

#endif