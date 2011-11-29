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
#include "typedef.h"

using namespace std;

typedef list<Module *> ModuleLst;
typedef ModuleLst::iterator ModuleLstItr;

class Tier 
{
	private:
		int metal_num;
		float tier_area;	//unit mm^2
		ModuleLst * modules;
		
	public:
		inline Tier(){
			metal_num = 0;
			tier_area = 0.0;
			modules = new ModuleLst();
		}
	
		inline ~Tier(){
			delete modules;
		}
	
	//accessors
		inline int getMetal_num() {return metal_num;}
		inline float getTier_area() {return tier_area;}
		inline ModuleLst * getModules() {return modules;}
			
	//modifiers
		inline void setMetal_num(int num) {metal_num = num;}
		inline void setTier_area(float area) {tier_area = area;}
		inline void setModule(Module * newModule) {modules->push_back(newModule);}
	
	//functions
		int calc_metal_num();
		float calc_area(int m_num);
		float metal_util(); //called by mutate
};

#endif
