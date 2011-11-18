/*
* Module.cc
* Author:Qiaosha Zou
* Time: 2011-11-18
* description: the main function to add connections
*/

#include "Module.h"

void Module::setConnections(Connect * newConnect)
{
	connections->push_front(newConnect);	
}
