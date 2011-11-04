/*
authro: Qiaosha zou
email: qszou@cse.psu.edu
the Pennsylvania State University

This class is to translate the standard .pl format
to labyrinth format
*/

#ifndef CONVERTER_H
#define CONVERTER_H

#include <vector>
#include <list>
#include <string.h>
#include <iostream>
#include <fstream>
#include <map>

#include "Pin.h"


using namespace std;

class Converter
{
private:
	char netfile[100];
	char placefile[100];
	char outfile[100];
	map<string, Pin> PlaceMap;
	map<string, Pin>::iterator PlaceMapItr;
public:
//-----------------------constructor----------------------------
	Converter() ;
	inline Converter(char * nets, char * place, char * out) 
		{strcpy(netfile, nets);
		 strcpy(placefile, place);
		 strcpy(outfile, out);
		}
	inline ~Converter(){
		//for(NetVectItr i = nets.begin(); i!= nets.end(); i++)
		//	delete (*i);
		PlaceMap.~map();
	}

//-----------------------get/set function-----------------------
	//inline NetVect * getNets() {return nets;}
	inline char * getNetfile() {return netfile;}
	inline char * getPlacefile() {return placefile;}

	inline void setOutfile(char * out) {strcpy(outfile, out);}
	inline void setPlacename(char * in) {strcpy(placefile, in);}
	inline void setNetfile(char * in) {strcpy(netfile, in);}
	//inline setNetnum(int i) {this->nets->resize(i);}

//----------------------functions------------------------------
	void fileTranslation();
	void readPlacement();
};

#endif
