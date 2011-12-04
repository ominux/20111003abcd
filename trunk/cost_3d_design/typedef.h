/* typedef.h
 * Author: Xiangyu Dong, xydong@cse.psu.edu
 */

#ifndef TYPEDEF_H
#define TYPEDEF_H

#define FANOUT 4
#define WIRE_PITCH 0.00032 //mm
#define WIRE_EFF 0.85
#define RENT_P	0.6
#define ALPHA   0.8     //FANOUT/(FANOUT+1)
#define RENT_K	1.4
#define AREA_UTIL 0.8
#define TSV_PITCH 0.000049 //unit mm^2

typedef struct {
	char name[50];				// technology name
	double diameter;			// diameter of a single wafer, unit: mm
	int technology_node;		// technology node, unit: nm
	double default_metal_layer;	// default number of metal layer
	double metal_layer_cost;	// additional cost of metal layer
	double wafer_cost;			// Foundry margin added to the yielded wafer cost
	double wafer_sort_cost;		// The cost of wafer sort
	double defect_density;		// wafer defect density, unit: number per cm^2
	double kgd_cost;			// cost per tested good die
} PROCESS_PARA;

typedef struct {
	char name[10];				// pakcaging name
	double package_yield;		// packaging yield
	double package_cost;		// packaging cost
	double package_test_yield;	// packaging test yield;
	double package_test_cost;	// packaging test cost;
} PACKAGE_PARA;

typedef struct {
	double w2w_bonding_cost;	// wafer-to-wafer bonding cost
	double w2w_bonding_yield;	// wafer-to-wafer bonding yield
	double d2w_bonding_cost;	// die-to-wafer bonding cost
	double d2w_bonding_yield;	// die-to-wafer bonding yield
	double tsv_laser_cost;		// TSV cost (laser), per TSV
	double tsv_etch_cost;		// TSV cost (etch), per mask
	double thinning_cost;		// thinning cost
} BONDING_PARA;

typedef struct {
	int wafer_bonding;			// 0: W2W bonding, 1: D2W bonding
	int face;					// 0: Face-to-back bonding, 1: Face-to-face bonding
	int tsv;					// 0: Laser; 1: Etch
} BONDING_KNOB;


#endif
