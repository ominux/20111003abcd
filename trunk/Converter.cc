/*
Author: Qiaosha Zou
email: qszou@cse.psu.edu
cc file for converter
*/

#include "Converter.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

void Converter::readPlacement()
{
	ifstream input;
	char buffer[100];
	int height, width;
	int cellnum;
	string header;

	input.open(placefile, ios::in);
	if(!input){
		cout << placefile << "could not be opended. Exiting..." << endl;
		exit(-1);
	}		
	else
		cout << placefile << "was opened"<< endl;
	
	header = "# Standard cells";
	//get the cell size
	/*do{
		input.getline(buffer, 100);
		int compare = strncmp(header.c_str(), buffer, sizeof(header));	
	}while(strncmp(header.c_str(), buffer, sizeof(header)));
	*/

	for(int i=0; i<11; i++)
		input.getline(buffer, 100);

	sscanf(buffer, "# Standard cells %d %*s", &cellnum);

	cout << "Converter::cell num is "<< cellnum << endl;	

	header = "# Chip size";
	//get the chip size
	/*do{
		input.getline(buffer, 100);	
	}while(!strncmp(header.c_str(), buffer, sizeof(header)));
	*/
	
	for(int i=0; i<14; i++)
		input.getline(buffer, 100);

	sscanf(buffer, "# Chip size %d %*s %d %*s", &width, &height);
	
	cout << "Converter::chip size is "<< width << "*" << height << endl;
	//delete the unneeded lines
	for(int i=0; i<8; i++)
	{
		input.getline(buffer, 100);
	}

	for(int i=0; i < cellnum; i++){
		char cellname[10];
		string name;
		int pinX, pinY;
		map<string, Pin > trytest;

		input.getline(buffer, 100);
		sscanf(buffer, "%s %d %d %*s", cellname, &pinX, &pinY);	
		pinX = pinX + (width/2);
		pinY = pinY + (height/2);
		name =(string)cellname;
		cout << name <<" " <<cellname<<endl;
		if(pinX < 0 || pinX > width || pinY < 0 || pinY > height){
			cout << "Converter::the pin is invalid"<<endl;
		}
		else{
			Pin temppin=Pin(pinX, pinY);
			PlaceMap[name]=temppin;
		}
	}

	/*	for(PlaceMapItr=PlaceMap.begin(); PlaceMapItr!= PlaceMap.end(); PlaceMapItr++){
		cout << "cellname :"<<PlaceMapItr->first  << (PlaceMapItr->second).getX() << (PlaceMapItr->second).getY()<<endl;
		}*/
	cout << "Converter::total allocated space for placemap is "<<(int)PlaceMap.size() << endl;
	
	//closing the file
	input.close();
}

void Converter::fileTranslation()
{
	ofstream output;
	ifstream input;
	char buffer[100];

	//open the nets and placement files
	input.open(netfile, ios::in);
	if(!input){
		cout << netfile << " could not be opened. Exiting..."<<endl;
		exit(-1);
	}
	else
		cout << netfile << " was opened" << endl;

	output.open(outfile, ios::out);
	if(!output)
		cout << outfile << " Could not be opend, not output labyrinth file" <<endl;
	else
		cout << outfile << " was opened for labyrinthe file" << endl;

	//get the number of nets
	int numNets;

	for(int j=0; j<6; j++)	
		input.getline(buffer, 80);

	sscanf(buffer, "NumNets %*s %d", &numNets);
	nets.resize(numNets);

	cout << "Converter:: the number of nets is "<<numNets << endl;
	output<< "num net "<< numNets <<endl;

	//skip the next two lines
	input.getline(buffer, 100);
	input.getline(buffer, 100);

	for(int i=0; i<numNets; i++){
		int netID, numPins;
		input.getline(buffer, 100);
		sscanf(buffer, "NetDegree %*s %d", &numPins);
		//get the net name
		char netName[10];
		stringstream ss;
		ss << i;
		strcpy(netName, (ss.str()).c_str());		

		netID = i;
		nets[i] = new Net(numPins, netID, netName);
		
		output<< netName << " " << netID << " " << numPins << endl;
				
		//set pins location
		for(int j=0; j< numPins; j++){
			int pinX, pinY;
			char cellname[10];
			input.getline(buffer, 100);
			sscanf(buffer, "%s %*s %*s %d %d", cellname, &pinX, &pinY);
		
			PlaceMapItr = PlaceMap.find(cellname);
			Pin tempPin = PlaceMapItr->second;
			tempPin.setX(pinX+(tempPin.getX()));
			tempPin.setY(pinY+(tempPin.getY()));
			cout << "Converter::the new x of pin is "<<tempPin.getX() << endl;
			cout << "Converter:: the new y of pin is "<<tempPin.getY() << endl;
			nets[i]->setPin(j, &tempPin);		
			output << " " << tempPin.getX() << " " <<tempPin.getY() << endl;			
		} 	
	}		

	//closing files!!
	input.close();
	output.close();
}
