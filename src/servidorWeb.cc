/*
 *   C++ sockets on Unix and Windows
 *   Copyright (C) 2002
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "YASL.h"      // For Socket, ServerSocket, and SocketException
#include <iostream>    // For cerr and cout
#include <cstdlib>     // For atoi()
#include "json.hpp"   //
#include <fstream>   // Librería necesaria para el manejo de archivos
//#include "checkArgs.h"

using json = nlohmann::json;
const uint32_t RCVBUFSIZE = 3500;    // Size of receive buffer
// TCP client handling function
void HandleTCPClient(TCPSocket *sock, std::string root_dir, std::string notFoundFile, std::string direccionIP) {
	//std::cout << "Inicio de conexión (Establecida y Escuchando ) ";
	try {
		std::cout << sock->getForeignAddress() << ":";
	} catch (SocketException e) {
		std::cerr << "Unable to get foreign address" << std::endl;
	}
	try {
		std::cout << sock->getForeignPort();
	} catch (SocketException e) {
		std::cerr << "Unable to get foreign port" << std::endl;
	}
	//std::cout << std::endl;

	// Send received string and receive again until the end of transmission
	char echoBuffer[RCVBUFSIZE]; //buffer de la cadena echo
	uint32_t largoMensaje = 0;  // Tamaño del mensage recibido
	std::string rutaNavegador = "";
	//std::string pagina = "";
	//std::ifstream lecturaPagina;

//	bool termino = false;
//	bool termino2 = false;
  largoMensaje = sock->recv(echoBuffer, RCVBUFSIZE);
	if(largoMensaje < 0){
    std::cerr << "Error en la solicitud" << std::endl;
	}else{
		echoBuffer[largoMensaje] = '\0';
		rutaNavegador = rutaNavegador + echoBuffer + "\n";
	}

	rutaNavegador = rutaNavegador.substr(4, 8);
	delete sock;
  }


	//Lectura y localización del archivo de configuracion json
	json leerJSON(){
		std::ifstream archivoJSON("bin/config.json");
		if(archivoJSON.is_open()){
			json var;
			var = json::parse(archivoJSON);
			return var;
		}else{
			std::cerr << "Error al intentar leer el archivo JSON" << std::endl;
		}
	}



int main(int argc, char *argv[]) {

	uint16_t echoServPort;
	//std::ifstream archivoJSON("bin/config.json");
	json infoJSON = leerJSON();
	std::string direccionIP = infoJSON["ip"];
	echoServPort = infoJSON["puerto"];
	std::string root_dir = infoJSON["root_dir"];
	std::string notFoundFile = infoJSON["notFoundFile"];
	std::cout << "Estableciendo conexión con el cliente: " + direccionIP + "\n";

	try {
		TCPServerSocket servSock(direccionIP,echoServPort);     // Se mandan los datos rescatados desde el archivo json para luego intentar iniciar la conexion;

		for (;;) {   // Run forever
			HandleTCPClient(servSock.accept(), root_dir, notFoundFile, direccionIP);       // Wait for a client to connect
		}
	} catch (SocketException &e) {
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
	// NOT REACHED

	return EXIT_SUCCESS;
}
