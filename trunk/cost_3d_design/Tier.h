/* Tier.h
* Function: store the information of each tier:
* 	metal number, tier area, modules in this tier
*	tier cost
* Author: Qiaosha Zou
* Time: 2011-11-19
*/

#ifndef TIER
#define TIER

#include <list>
#include "Module.h"

using namespace std;

typedef list<Module *> ModuleLst;
typedef ModuleLst::iterator ModuleLstItr;

class Tier 
{
	private:
		int default_metal;  //default metal layer
		int metal_num;
		float tier_area;	//unit mm^2
		float tier_cost;	//each tier cost
		ModuleLst * modules;
		
	public:
		inline Tier(){
			default_metal = 6;
			metal_num = 0;
			tier_area = 0.0;
			tier_cost = 0.0;
			modules = new ModuleLst();
		}
	
		inline ~Tier(){
			delete modules;
		}
	
	//accessors
		inline int getMetal_num() {return metal_num;}
		inline float getTier_area() {return tier_area;}
		inline float getTier_cost() {return tier_cost;}
	
	//modifiers
		inline void setMetal_num(int num) {metal_num = num;}
		inline void setTier_area(float area) {tier_area = area;}
		inline void setTier_cost(float cost) {tier_cost = cost;}
		inline void setModule(Module * newModule) {modules->push_back(newModule);}
	
	//functions
		int calc_metal_num(int fo, float ew, float w_pitch);
		float calc_area(int m_num, int fo, float ew, float w_pitch);
		float metal_util(float w_pitch, float ew);
};

#endif
