
#ifndef MYSQL_

#define MYSQL_

#include <iostream>
#include <string>
#include "ContenedorAudio.hpp"
#include <mysql/mysql.h>
#include <mysql/mysqld_error.h>
#include <sstream>

using namespace std;


MYSQL *myData;


/***************************************************************************/
void initMySQL()
{
	// Intentar iniciar MySQL:
	if(!(myData = mysql_init(0)))
	{
		cout << "Imposible crear el objeto myData" << endl;
	}
	
	const char* usuario = "cophrade";
	const char* clave = "cophrade";
	const char* database_name = ("Cophrade");
	if(!mysql_real_connect(myData, "localhost", usuario, clave, database_name, MYSQL_PORT, NULL, 0))
	{
		// No se puede conectar con el servidor en el puerto especificado.
		    cout << "Imposible conectar con servidor mysql en el puerto " << MYSQL_PORT << endl;
			mysql_close(myData);
	}
	
	if(mysql_ping(myData))
	{
		cout << "Error: conexión imposible" << endl;
		mysql_close(myData);
	}	
}

/***************************************************************************/

void nuevaEscucha(string key, resultadoAnalisis res, string ip)
{
	ostringstream convert;
	convert << res.resultado;
	string n = convert.str();

	// se registra una nueva escucha
	string select = "update Escuchas set idMarcha='" + res.marcha + "', ip='" + ip + "', fecha=now(), resultado=" + n + " where claveEscucha='" + key + "'";
    if(mysql_query(myData, select.c_str()))
    {
        // Error al realizar la consulta:
        cout << "ERROR: " << mysql_error(myData) << endl;
    }
}

/***************************************************************************/

#endif
