/*
* main.cc
* Function: testing
* Author: Qiaosha Zou qszou@cse.psu.edu
* Time: 2011-11-26
*/


#include "Design.h"
#include "typedef.h"
#include "Tier.h"
#include "Module.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <cstring>
#include <list>

#define POPULATION 9 //total popluation number
#define MAX_ITER  6 //totoal iteration time

using namespace std;

typedef list<Design> DesignList;
typedef DesignList::iterator DesignsItr;

void read_modules(ifstream * file, ModuleLib * lib)
{
	char buffer[80];
	int module_size;
	file->getline(buffer, 80);
	sscanf(buffer, "%d", &module_size);

	for(int i = 0; i< module_size; i++)
	{
		int count; //gate count
		float size; //module area
		int connect_count;
		int module_id;
		file->getline(buffer,80); //get the empty line
		file->getline(buffer,80);
		sscanf(buffer, "%d", &module_id);
		file->getline(buffer, 80);
		sscanf(buffer, "%d %f", &count, &size);

		Module * temp = new Module(module_id, count, size);

		file->getline(buffer,80);
		sscanf(buffer, "%d", &connect_count);

		int id, num;
		for(int m = 0; m < connect_count; m++)
		{
			file->getline(buffer, 80);
			sscanf(buffer, "%d %d", &id, &num);
			Connection * connect = new Connection(id, num);
			temp->setConnections(*connect); //copy the content of the connection to connections in module
			delete connect;
		}		

		lib->push_back(*temp); //copy the module to library
		delete temp;
	}
	
}

bool compare_design(Design first, Design second)
{
	if(first.getDesign_cost() < second.getDesign_cost())
		return true;
	else
		return false;
}

int main(int argc, char *argv[])
{
	if(argc != 4){
		cout<< "./CostDesign infile outfile layer_num"<<endl;
		return 0;
	}
	
	ifstream input;
	ofstream output;

	input.open(argv[1], ios::in);
	if(!input)
	{
		cout<< argv[1] <<"cannot be opened. Exiting..." << endl;
		return -1;
	}

	output.open(argv[2], ios::out);
	if(!output)
	{
		cout<< argv[2] << "cannot be opened. Exiting..." << endl;
		//return -1;
	}

	//get the input of layer number
	int layer_num;
	stringstream ss(argv[3]);
	ss >> layer_num;
	if(layer_num > MAX_TIER)
	{
		cout<<"Max Tier:" << MAX_TIER << endl;
		return -1;
	}	
	//variables
	list<Design> test;
	DesignList design_list;
	ModuleLib  module_lib;	
	
	//read file	
	read_modules(&input, &module_lib);

	//main body
	for(int i =0; i< POPULATION; i++)
	{
		Design * temp = new Design();
		//temp->partition();
		design_list.push_back(*temp);
		(design_list.back()).partition(module_lib, layer_num); //calculate cost in here
		delete temp;
	}	
		
	//sorting based on design cost
	design_list.sort(compare_design);

	int iter_time = 0; //iteration times
	while(iter_time < MAX_ITER)
	{
		list<DesignsItr> loser;

		int ingrade = 0;
		for(DesignsItr ditr = design_list.begin(); ditr != design_list.end(); ditr++, ingrade++)
		{
			//mutate part
			if(ingrade > POPULATION/3){
				if(ingrade<2*POPULATION/3){
					ditr->mutate(module_lib);
				}else{
					loser.push_back(ditr); //list of repartition design
				}
			}
		}
		for(list<DesignsItr>::iterator loseritr = loser.begin(); loseritr != loser.end(); loseritr++)
			design_list.erase(*loseritr);
		for(int regen = loser.size(); regen!=0; regen--)
		{
			Design * temp = new Design();
			design_list.push_back(*temp);
			(design_list.back()).partition(module_lib, layer_num);
			delete temp;
		}

		loser.clear();
		design_list.sort(compare_design);

		iter_time++;
	}

	//remember to output design to file
	int count = 0;
	for(DesignsItr ditr = design_list.begin(); ditr != design_list.end(); ditr++, count++)
	{
		output << "Design: "<<count << endl;	
		ditr->print_design(output);
	
	}
	//deconstruct??remember to deconstruct modules
	design_list.clear();
	module_lib.clear();

	return 0;
}

