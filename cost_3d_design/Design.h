/* Design.h
* Function: store the information of the design: information of each tier
	total number of cost(cost related info)
* Author: Qiaosha Zou
* Time: 2011-11-19
*/

#ifndef DESIGN
#define DESIGN

#include <vector>
#include <list>
#include <fstream>
#include <stdio.h>
#include "Tier.h"
#include "Module.h"
#include "typedef.h"

#define MAX_TIER 4

using namespace std;

typedef vector<Tier *> TierVect;
typedef TierVect::iterator TierVectItr;
typedef vector<Module *> ModuleLib;
typedef ModuleLib::iterator ModuleLibItr;

class Design
{
	private:
		//at most support 4 tier design
		float design_cost;	//final design total cost
		TierVect * stackings; 	//all the tiers that the design includes
		int tsv_num[MAX_TIER-1];
		TierVect * best_tiers;
		float best_cost;

	public:
		inline Design(){
			design_cost = 0.0;
			best_cost = 0.0;
			stackings = new TierVect();
			best_tiers = new TierVect();
			for(int i = 0; i<MAX_TIER; i++)
				tsv_num[i] = 0;
		}

		inline ~Design(){
			//for(TierVectItr i =stackings->begin(); i!= stackings->end(); i++)
			//	delete(*i);
			cout << "stackings address" << stackings << "size " << stackings->size() << endl;
			stackings->clear();
			best_tiers->clear();
		}
		
	//accessors
		inline int* getTSV_num() {return tsv_num;}
		inline float getDesign_cost() {return design_cost;}
		inline TierVect* getStacking() {return stackings;}
		inline float getBest_cost() {return best_cost;}
	//modifiers
		inline void setDesign_cost(float cost) {design_cost = cost;}
		inline void setStacking(Tier * newTier) {stackings->push_back(newTier);}
		inline void setTSV_num(int i, int num) {tsv_num[i] = num;}
	//functions
		float calc_design_cost(ModuleLib all_module);
		void partition(ModuleLib all_module, int layer_num); //random select modules and put them into tier
		void mutate(ModuleLib all_module); //adjust modules between tier to get better cost
		void calc_tsv_num(ModuleLib all_module);

		void print_design(ofstream &output);
};

#endif
