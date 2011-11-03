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
#include "Maze.h"
#include "MazePoint.h"
#include "PrimMST.h"
#include "RouteTracks.h"
#include "Pin.h"
#include "Net.h"
#include "CoupleFree.h"
#include "Converter.h"

using namespace std;

int main(int argc, char *argv[]){

	char * netfile = "ibm01.nets";
	char * placefile = "ibm01-cu85_Dragon.pl";
	char * out = "ibm01_labyrinth.out";

	Converter short_test(netfile, placefile, out);
	short_test.readPlacement();
	short_test.fileTranslation();
	
	return 0;
}
