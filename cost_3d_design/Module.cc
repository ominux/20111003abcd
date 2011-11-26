/*
* Module.cc
* Author:Qiaosha Zou
* Time: 2011-11-18
* description: the main function to add connections
*/

#include "Module.h"

void Module::setConnections(Connection newConnect)
{
	connections->push_back(newConnect);	
}
