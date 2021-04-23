#pragma once

#include "misc.h"
#ifdef MPOOL
extern pj_pool_t *mem_pool;
#endif 

unsigned BIT_PER_ID = 8, BIT_PER_LABEL = 8, NUM_PER_UNINT = 4;
unsigned BIT_PER_ID_SHIFT = 3, BIT_PER_LABEL_SHIFT = 3, NUM_PER_UNINT_SHIFT = 2;
unsigned PACK_VERTEX_SIZE_G1, PACK_VERTEX_SIZE_G2;
unsigned PACK_LABEL_SIZE_G1, PACK_LABEL_SIZE_G2;
unsigned UNMAPPED_SHORT = 0xFF, MASK_SHORT = 0xFF;


class StateValue
{
public:
	int hash_val;
	vector<unsigned> vertex_mapping;

	StateValue() {}
	StateValue(vector<unsigned> &m): vertex_mapping(m) {}
};

class StateKey
{
public:
	int g_value, f_value;
	int mapped_vertex_num_g1, mapped_vertex_num_g2;
	int remain_edge_num_g1, remain_edge_num_g2;
	vector<unsigned> vertex_mapping_g1; 
	vector<unsigned> vertex_mapping_g2; 
	vector<int> vlabel_list_g1; 
	vector<int> vlabel_list_g2;

	StateKey() {}
	StateKey(int gv, int fv, int mvn_g1, int mvn_g2, int ren_g1, int ren_g2)
		: g_value(gv), f_value(fv), mapped_vertex_num_g1(mvn_g1), mapped_vertex_num_g2(mvn_g2), remain_edge_num_g1(ren_g1), remain_edge_num_g2(ren_g2) {} 
};

/* operators for GED_key */
bool operator < ( const StateValue& x, const StateValue& y ) {
	int i;
	if (x.hash_val < y.hash_val) return 1;
	else if (x.hash_val > y.hash_val) return 0;
	for (i = 0; i < x.vertex_mapping.size(); ++ i)
		if (x.vertex_mapping[i] < y.vertex_mapping[i])
			return 1;
		else if (x.vertex_mapping[i] > y.vertex_mapping[i])
			return 0;
	return 0;
}

bool operator > ( const StateValue& x, const StateValue& y ) {
	int i;
	if (x.hash_val > y.hash_val) return 1;
	else if (x.hash_val < y.hash_val) return 0;
	for (i = 0; i < x.vertex_mapping.size(); ++ i)
		if (x.vertex_mapping[i] > y.vertex_mapping[i])
			return 1;
		else if (x.vertex_mapping[i] < y.vertex_mapping[i])
			return 0;
	return 0;
}

bool operator == ( const StateValue& x, const StateValue& y ) {
	int i;
	if (x.hash_val != y.hash_val) return 0;
	for (i = 0; i < x.vertex_mapping.size(); ++ i)
		if (x.vertex_mapping[i] != y.vertex_mapping[i])
			return 0;
	return 1;
}

bool operator != ( const StateValue& x, const StateValue& y ) {
	return !(x == y);
}

/* operators for GED_node */
bool operator < ( const StateKey& x, const StateKey& y ) {
	return (x.f_value < y.f_value);

bool operator > ( const StateKey& x, const StateKey& y ) {
	return (x.f_value > y.f_value);
}

bool operator == ( const StateKey& x, const StateKey& y ) {
	return (x.f_value == y.f_value);
}

bool operator != ( const StateKey& x, const StateKey& y ) {
	return (x.f_value != y.f_value);
}