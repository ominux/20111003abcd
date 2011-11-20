/* Design.h
* Function: store the information of the design: information of each tier
	total number of cost(cost related info)
* Author: Qiaosha Zou
* Time: 2011-11-19
*/

#ifndef DESIGN
#define DESIGN

#include <vector>
#include "Tier.h"

typedef vector<Tier *> TierVect;

class Design
{
	private:
		//at most support 4 tier design
		float design_cost;	//final design total cost
		TierVect * stackings; 	//all the tiers that the design includes
	public:
		inline Design(){
			design_cost = 0.0;
			stackings = new TierVect();
		}
		
	//accessors
		inline float getDesign_cost() {return design_cost;}
		inline TierVect * getStacking() {return stackings;}
	//modifiers
		inline void setDesign_cost(float cost) {design_cost = cost;}
		void setStacking(TierVect* newTier);
	//functions
		void calc_design_cost();
}
