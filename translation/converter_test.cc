/*
Author: Qiaosha zou
Email: qszou@cse.psu.edu
*/

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include "Point.h"
#include "Pin.h"
#include "Converter.h"

using namespace std;

int main(int argc, char *argv[]){

	if(argc != 4){
		cout<<"INPUT: ./translator [netfile] [placement file] [output file]"<<endl;
		return 1;
	}
	Converter short_test(argv[1], argv[2], argv[3]);
	short_test.readPlacement();
	short_test.fileTranslation();
	
	return 0;
}
