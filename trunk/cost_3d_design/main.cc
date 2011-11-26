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
#include <fstream>
#include <stdio.h>
#include <cstring>

using namespace std;

int main(int argc, char *argv[])
{
	if(argc != 3){
		cout<< "./CostDesign infile outfile"<<endl;
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
	
	//variables
	Design temp_design();
	Design best_design();	
	
	//read file	
	char buffer[80];
	int module_size;
	input.getline(buffer, 80);
	sscanf(buffer, "%d", &module_size);
	
	Module temp_module;
	for(int j = 0; j< module_size; j++)
	{
		
	}
	
	//setting related parameters, from Xiangyu's, can changed to read from input file	
	PROCESS_PARA process;
	PACKAGE_PARA package;
	BONDING_PARA bonding;
	BONDING_KNOB knob;
	
	strcpy(process.name, "300mm - 45nm - TSMC - dula gate ox CMOS Logic");
	process.diameter = 300;
	process.technology_node = 45;
	process.default_metal_layer = 6;
	process.metal_layer_cost = 249.492;
	process.wafer_cost = 4232.795;
	process.wafer_sort_cost = 1182.66;
	process.defect_density = 0.4;
	process.kgd_cost = 4.565;

	bonding.thinning_cost = 250;
	bonding.d2w_bonding_yield = 0.99;
	bonding.w2w_bonding_yield = 0.99;
	bonding.w2w_bonding_cost = 22.9;
	bonding.d2w_bonding_cost = 115/20;
	bonding.tsv_etch_cost = 250;
	bonding.tsv_laser_cost = 0.001;

	knob.face = 0;
	knob.tsv = 1;
	knob.wafer_bonding = 1;

	//package.name = "pBGA";
	package.package_cost = 0.218;
	package.package_yield = 0.995;
	package.package_test_cost = 0.544;
	package.package_test_yield = 0.998;

	//used in Tier.cc to calculate metal layer needed
	int fanout = 4;
	float wire_efficient = 0.85;
	float wire_pitch = 0.5; //unit um

	//main body
	
		
	//deconstruct??remember to deconstruct modules
}

