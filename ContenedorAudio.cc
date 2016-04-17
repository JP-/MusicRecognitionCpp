#include "ContenedorAudio.hpp"
#include <sys/types.h>
#include <string>
#include <dirent.h>

using namespace std;

/***************************************************************************/


ContenedorAudio::ContenedorAudio(char * path)
{
	DIR *dir;
	struct dirent *ent;

	db.set_empty_key(NULL);

	ruta = path;
	ncanciones = 0;
	
	dir = opendir (path);

	if (dir == NULL)
		cout << "No puedo abrir el directorio" << endl;

	/* Leyendo uno a uno todos los archivos que hay */
	while ((ent = readdir (dir)) != NULL)
	{
		/* Nos devolverá el directorio actual (.) y el anterior (..), como hace ls */
		if ( (strcmp(ent->d_name, ".")!=0) && (strcmp(ent->d_name, "..")!=0) )
		{
			cargarFingerPrint(ent->d_name);
			ncanciones++;
			//cout << "leída " << ent->d_name << endl;
		}
	}
	closedir (dir);
}

/***************************************************************************/


void ContenedorAudio::cargarFingerPrint(char * file)
{
	ifstream fi;
	fi.open(ruta + file);
	
	string line;
	fi >> line;
    while(!fi.eof())
    {
		long hash = stol(line, nullptr);
		if(line == "0")
		{
			hash = 1;
		}

		vector<DataPoint> listaDP;

		fi >> line;
		fi >> line;
		while(line.find("}")==string::npos)
		{
			string songId = line.substr(line.find("[")+1, line.find("|")-1);
			int time = stoi(line.substr(line.find("|")+1, line.find("]")));

			DataPoint dp;
		    dp.songId = songId;
			dp.time = time;

		    listaDP.push_back(dp);

			fi >> line;
		}

		if(db[hash].size() == 0)
		{
			db[hash] = listaDP;
		}
		else
		{
			db[hash].insert(db[hash].end(), listaDP.begin(), listaDP.end());
		}
		
		fi >> line;
    }
}

/***************************************************************************/

resultadoAnalisis ContenedorAudio::matching(Audio * audio)
{
	google::dense_hash_map<string, google::dense_hash_map<int,int> > matchMap;
	matchMap.set_empty_key("valor nulo");
	audio->makeFingerPrint();
	google::dense_hash_map<long,vector<DataPoint> > fingerPrint = audio->getFingerPrint();
	for (google::dense_hash_map<long,vector<DataPoint> >::iterator it=fingerPrint.begin(); it!=fingerPrint.end(); ++it)
	{
	    long key = it->first;
	    vector<DataPoint> values = it->second;

		vector<DataPoint> listPoints;
		if (db[key].size() != 0)
		{
			listPoints = db[key];

			/***************************************************************/
			// hacer esto eficiente
			//cout << key << " -> " << values.size() << "x" << listPoints.size() << endl;
			
		    for(int i=0; i<values.size(); i++)
		    {
				for (int k=0; k<listPoints.size(); k+=2)
				{
				    // diferencia de tiempo entre el dataPoint y t
				    int offset = abs(listPoints[k].time - values[i].time);

					if (matchMap[listPoints[k].songId].size() != 0)
				    {
						google::dense_hash_map<int, int> * tmpMap = &matchMap[listPoints[k].songId];
						int count = tmpMap->operator[](offset);

						tmpMap->operator[](offset) = count + 1;
				    }
				    else
				    {
						google::dense_hash_map<int, int> tmpMap;
						tmpMap.set_empty_key(-1);
						tmpMap[offset] = 1;
						matchMap[listPoints[k].songId] = tmpMap;
				    }

				    if(k+1<listPoints.size())
					{
						// diferencia de tiempo entre el dataPoint y t
					    int offset2 = abs(listPoints[k+1].time - values[i].time);

						if (matchMap[listPoints[k+1].songId].size() != 0)
					    {
							google::dense_hash_map<int, int> * tmpMap = &matchMap[listPoints[k+1].songId];
							int count = tmpMap->operator[](offset2);

							tmpMap->operator[](offset2) = count + 1;
					    }
					    else
					    {
							google::dense_hash_map<int, int> tmpMap;
							tmpMap.set_empty_key(-1);
							tmpMap[offset] = 1;
							matchMap[listPoints[k+1].songId] = tmpMap;
					    }
					}
				}
			}
			/***************************************************************/
		}
	}
	
	return analizarMatch(&matchMap);
}

/***************************************************************************/

resultadoAnalisis ContenedorAudio::analizarMatch(google::dense_hash_map<string,google::dense_hash_map<int,int> > * matchMap)
{
	int bestCount = 0;
	string bestSong;
	int bestCount2 = 0;
	string bestSong2;
	int bestCount3 = 0;
	string bestSong3;
	int bestCount4 = 0;
	string bestSong4;
	int bestCount5 = 0;
	string bestSong5;

	resultadoAnalisis resultado;

	for (google::dense_hash_map<string,google::dense_hash_map<int,int> >::iterator it=matchMap->begin(); it!=matchMap->end(); ++it)
	{
	    string id = it->first;

		google::dense_hash_map<int,int> tmpMap = it->second;
	    int bestCountForSong = 0;

		for (google::dense_hash_map<int,int>::iterator it2=tmpMap.begin(); it2!=tmpMap.end(); ++it2)
		{
			if (it2->second > bestCountForSong) {
				bestCountForSong = it2->second;
			}
	    }

	    if (bestCountForSong > bestCount)
	    {
			bestCount5 = bestCount4;
			bestSong5 = bestSong4;
			bestCount4 = bestCount3;
			bestSong4 = bestSong3;
			bestCount3 = bestCount2;
			bestSong3 = bestSong2;
			bestCount2 = bestCount;
			bestSong2 = bestSong;
			bestCount = bestCountForSong;
			bestSong = id;
	    }
	}


	
	cout << bestSong << " " << bestCount << endl;
	if(bestSong2 != "")
	    cout << bestSong2 << " " << bestCount2 << endl;
	if(bestSong3 != "")
	    cout << bestSong3 << " " << bestCount3 << endl;
	if(bestSong4 != "")
		cout << bestSong4 << " " << bestCount4 << endl;
	if(bestSong5 != "")
		cout << bestSong4 << " " << bestCount5 << endl;


	resultado.marcha = bestSong;
	resultado.resultado = bestCount;
	

	return resultado;
}

/***************************************************************************/

int ContenedorAudio::size()
{
	return ncanciones;
}

/***************************************************************************/

void ContenedorAudio::crearBD(char * ruta, int fuz)
{
	vector<string> marchas;

	ifstream fe("completas.txt");

	string video;

	while(!fe.eof())
	{
		getline(fe, video);
		if(video != "")
			marchas.push_back(video);
	}
	fe.close();

	ofstream fs("completas.txt");
	for(int i=0; i<marchas.size(); i++)
		fs << marchas[i] << endl;



	DIR *dir;
	struct dirent *ent;

	dir = opendir (ruta);

	if (dir == NULL)
		cout << "No puedo abrir el directorio" << endl;

	// Leyendo uno a uno todos los archivos que hay
	while ((ent = readdir (dir)) != NULL)
	{
		// Nos devolverá el directorio actual (.) y el anterior (..), como hace ls
		if ( (strcmp(ent->d_name, ".")!=0) && (strcmp(ent->d_name, "..")!=0) )
		{
			string a = ruta;
			a = a + ent->d_name;

			string n = ent->d_name;
			n.resize(11);

			if(find(marchas.begin(), marchas.end(), n) == marchas.end())
			{
				Audio wav((char*)a.c_str(), fuz);
				wav.makeAndSaveFingerPrint("/home/juanpi/fingers/");
				fs << n << endl;
				cout << n << endl;
			}
		}
	}
	closedir (dir);

	fs.close();
}

/***************************************************************************/