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

typedef list<Module *> ModuleLst;
typedef ModuleLst::iterator ModuleLstIrt;

class Design
{
	private:
		int metal_num;
		float tier_area;
		int tsv_num;	//tsv number to the upper tier, if it is the top one, the value is 0
		float tier_cost;	//each tier cost
		ModuleLst * modules;
		
	public:
		inline Tier(){
			metal_num = 6;
			tier_area = 0.0;
			tier_cost = 0.0;
			tsv_num = 0;
			modules = new ModuleLst();
		}
	
		inline ~Tier(){
			for(ModuleLstItr i= modules->begin(); i!= modules.end(); i++)
				delete (*i);
			delete modules;
		}
	
	//accessors
		inline int getMetal_num() {return metal_num;}
		inline float getTier_area() {return tier_area;}
		inline float getTsv_num() {return tsv_num;}
		inline float getTier_cost() {return tier_cost;}

	//modifiers
		inline void setMetal_num(int num) {metal_num = num;}
		inline void setTier_area(float area) {tier_area = area;}
		inline void setTsv_num(int num) {tsv_num = num;}
		inline void setTier_cost(float cost) {tier_cost = cost;}

	//functions
		void calc_metal_num();
		void calc_area();
		void calc_cost();
}

#endif
