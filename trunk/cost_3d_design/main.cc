/*
* main.cc
* Function: testing
* Author: Qiaosha Zou qszou@cse.psu.edu
* Time: 2011-11-10
*/


#include "Area.h"
#include "Area.cc"
#include <iostream>
#include <stdio.h>

using namespace std;

int main(int argc, char *argv[])
{
	unsigned long  gatecount;
	//float gatesize;	
	float rent_p;
	float fanout;
	float wire_pitch;
	float utilization;
	int metal_layer;	

	float result;
	
	if(argc == 2){
		sscanf(argv[1], "%ld", &gatecount);
		//sscanf(argv[2], "%f", &gatesize);
		Area test(gatecount);
		result = test.area_calc();
		cout << "area is" << result <<  endl;
		return 0;	
	}	

	else if(argc == 7){
		sscanf(argv[1], "%ld", &gatecount);
		//sscanf(argv[2], "%f", &gatesize);
		sscanf(argv[3], "%f", &rent_p);
		sscanf(argv[4], "%f", &wire_pitch);
		sscanf(argv[5], "%f", &fanout);
		sscanf(argv[6], "%f", &utilization);
		sscanf(argv[7], "%d", &metal_layer);
		Area test(gatecount, rent_p, fanout, wire_pitch, utilization, metal_layer);
		result = test.area_calc();
		cout << "area is"<< result<<endl;
		return 0;	
	}
		
	else
	{
		cout<<"input format: gatecount [rent_p] [wire_pitch] [avg fanout] [utilization] [metal_layer]" << endl;
		return -1;
	}

}

