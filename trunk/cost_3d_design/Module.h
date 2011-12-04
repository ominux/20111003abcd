/*
* Module.h
* Funtion: store module information: module ID,gate count, average gate size
	layer number. connection information(connect module and # of
	connection)
* Author: Qiaosha Zou
* Time: 2011-11-18
*/

#ifndef MODULE
#define MODULE

#include <vector>
#include <string.h>
#include <stdio.h>
#include <iostream>

using namespace std;

class Connection
{
	public:
	inline Connection() {moduleID = -1; connect_num = 0;}
	inline Connection(int id, int num) 
		{	moduleID = id;
			connect_num = num;
		}
		int moduleID;
		int connect_num;
	inline int getID() {return moduleID;}
	inline int getNum() {return connect_num;}
};

typedef vector<Connection *> ConnectVect;
typedef ConnectVect::iterator ConnectVectItr;

class Module
{
	private:
		char name[64]; //name of the module
		int moduleID;
		unsigned long gateCount; //total number of gate count
		float moduleArea; //total module area after synthesis, um^2
		int tier_num;

	public:
		//Construction and de-construction
		inline Module()
		{
			moduleID = 0;
			gateCount = 0;
			moduleArea = 0.0;
			tier_num = 0;
		}	
		inline Module(int id, unsigned long count, float size)
		{
			moduleID = id;
			gateCount = count;
			moduleArea = size;
			tier_num = 0;
		}

		inline ~Module()
		{
			//for(ConnectVectItr i = connections->begin(); i!= connections->end(); i++)
			//	delete i;
		}
	
		//accessors
		inline int	getTier() {return tier_num;}
		inline float 	getArea()	{return moduleArea;}
		unsigned long 	getCount()	{return gateCount;}
		inline char *	getName()	{return name;}
		inline int	getID()		{return moduleID;}

		//modifiers
		inline void setTier(int i) {tier_num = i;}
		inline void setID(int i) {moduleID = i;}
		inline void setCount(unsigned long count) {gateCount = count;}
		inline void setArea(int area)	{moduleArea = area;}
		inline void setName(char *newName) {strcpy(name, newName);}

		//others
	
};

#endif
