/* Design.cc
* Function: calculate design cost
*	perform 3D partitioning
*	calculate the number of TSVs
* Author: Qiaosha Zou
* Time: 2011-11-25
*/

#include "Design.h"

typedef vector<Connection *> ConnectVect;
typedef ConnectVect::iterator ConnectVectItr;

float Design::calc_design_cost()
{

}

void Design::3d_partition(ModuleLib all_module)
{

}

void Design::calc_tsv_num(ModuleLib all_module)
{
	ModuleLibItr one;
	int	temp_id;
	int 	temp_num;
	int	current_tier;
	int	con_tier;

	for(one = all_module.begin(); one!= all_module.end(); one++)
	{
		//get the connection of one module
		ConnectVect one_con = one->getConnections();
		current_tier = one->getTier();
		for(ConnectVectItr i = one_con.begin(); i!= one_con.end(); i++)
		{
			//get the connected module
			temp_id = i->getID();
			con_tier = all_module[temp_id]->getTier();
			if(con_tier>current_tier)
			{
				temp_num = all_module[temp_id]->getNum();
				//from current tier, every upper tier should add tsv_num
				for(int k = current_tier; k<con_tier; k++)
					tsv_num[k] += temp_num;	
			}			
		}		
	}	
}
