#pragma once

#include "minheap.h"
#include "graph.h"
#include "stopwatch.h"
#include <fstream>

extern vector<Graph> gdb;
extern bool print_ans, print_more;
extern Stopwatch v_time;
extern int nodes_expanded, nodes_popped;


class vertex_label_pair
{
public:
	int vid;
	int vlabel;
};

class edge_label_pair
{
public:
	int vid;
	int eid;
	int elabel;
};

class GED_key
{
public:
	int hash_key;
	vector<int> vertex_mapping;
};

class GED_node
{
public:
	int g_value;
	int f_value;
	int mapped_vertex_num_g1;
	int mapped_vertex_num_g2;
	int remaining_edge_num_g1;
	int remaining_edge_num_g2;
	vector<int> vertex_mapping_g1; 
	vector<int> vertex_mapping_g2;
	vector<int> vlabel_list_g1; 
	vector<int> vlabel_list_g2; 
	vector<int> elabel_list_g1; 
	vector<int> elabel_list_g2; 

	GED_node() {}
	GED_node(int gv, int fv, int mvn_g1, int mvn_g2, int ren_g1, int ren_g2)
		: g_value(gv), f_value(fv), mapped_vertex_num_g1(mvn_g1), mapped_vertex_num_g2(mvn_g2), remaining_edge_num_g1(ren_g1), remaining_edge_num_g2(ren_g2) {} 
};

class Vertex_sort
{
public:
	unsigned id;
	unsigned deg;
	unsigned cand_num;
	int vlabel;

	Vertex_sort(void): id(0), deg(0), cand_num(0) {}
	Vertex_sort(unsigned i, unsigned d, unsigned c): id(i), deg(d), cand_num(c) {}
};

class Priority
{
public:
	unsigned lgid, rgid;
	Graph *lg, *rg;
	unsigned lg_size;
	vector<vector<unsigned> >* unmat;
	pair<unsigned, int>* um_order;
	unsigned um_size;
	vector<vector<unsigned> > lv_cand;
	vector<Vertex_sort> lv_sort;

	Priority(const unsigned p, const unsigned q)
		: lgid(p), rgid(q), lg(&gdb[lgid]), rg(&gdb[rgid]), lg_size(lg->vertex_num), unmat(NULL), um_order(NULL), um_size(0) {}
	Priority(const unsigned p, const unsigned q, vector<vector<unsigned> >* pum, pair<unsigned, int>* perr, const unsigned s)
		: lgid(p), rgid(q), lg(&gdb[lgid]), rg(&gdb[rgid]), lg_size(lg->vertex_num), unmat(pum), um_order(perr), um_size(s) {}
	~Priority() { delete unmat; delete[] um_order; }
	void init_astar();
	inline void roll_back() { rg->resize(rg->vertex_num); }
};


bool operator < (const vertex_label_pair& x, const vertex_label_pair& y);
bool operator < (const edge_label_pair& x, const edge_label_pair& y);
bool operator < (const GED_key& x, const GED_key& y);
bool operator > (const GED_key& x, const GED_key& y);
bool operator == (const GED_key& x, const GED_key& y);
bool operator != (const GED_key& x, const GED_key& y);
bool operator < (const GED_node& x, const GED_node& y);
bool operator > (const GED_node& x, const GED_node& y);
bool operator == (const GED_node& x, const GED_node& y);
bool operator != (const GED_node& x, const GED_node& y);
GED_key generate_key( vector<int> &vertex_mapping );
bool is_adjacent(Graph &g, const int uid, const int vid);
bool is_adjacent_with_label(Graph &g, const int uid, const int vid, const int label);
int estimate_vertex(Graph &g1, Graph &g2, vector<int> &vertex_mapping_g1, vector<int> &vertex_mapping_g2);
int compute_label_difference(vector<int> &label_list_g1, vector<int> &label_list_g2);
int diff_subgraph_label(vector<int> &label_list_q, vector<int> &label_list_g);
bool mis_deg_combine_match(const unsigned p, const unsigned pid, const unsigned q, unsigned* qid, unsigned beg, unsigned end, int threshold, unsigned& count);
int compute_mismatch_error(GED_node& next_node, const unsigned lgid, const unsigned rgid, const int threshold);
int filter_edit_distance(Priority* pri);
int rudim_edit_distance(Priority* pri);

#define COMP_CD \
	if ((*lg)[i].vlabel == (*rg)[pos].vlabel) { \
	if (abs((int)((*lg)[i].degree - (*rg)[pos].degree)) <= tau) { \
	if (edge_diff((*lg)[i].elabs, (*rg)[pos].elabs, global_esize) <= tau) { \
	lv_cand[i].push_back((unsigned)pos); \
	} \
	} \
	} else { \
	if (abs((int)((*lg)[i].degree - (*rg)[pos].degree)) + 1 <= tau) { \
	if (edge_diff((*lg)[i].elabs, (*rg)[pos].elabs, global_esize) + 1 <= tau) { \
	lv_cand[i].push_back((unsigned)pos); \
	} \
	} \
	}

#define THRESHOLD_MATCH \
	if (err <= threshold) { \
	++ count; \
	delete[] diff; \
	return true; \
	}

#define ECHO_INDEX \
	unsigned list_len = 0, max_len = 0, avg_len = 0, qnum = 0, four_byte = path_graph.size(); \
	for (const_iter_graph it = path_graph.begin(), end = path_graph.end(); it != end; ++ it) { \
		list_len += it->second.size(); \
	for (set<unsigned>::const_iterator ir = it->second.begin(), ie = it->second.end(); ir != ie; ++ ir) { \
		if (status[*ir] == 0) ++ four_byte; \
	} \
	} \
	for (unsigned i = 0; i != gdb_size; ++ i) { \
	qnum += pdb[i].size(); \
	if (pdb[i].size() > max_len) max_len = pdb[i].size(); \
	if (status[i] != 0) ++ four_byte; \
	} \
	cout << "= = = = = = = = = = = = = = = = = = = = = = = = = =" << endl \
		<< "Inverted index size in byte: " << (four_byte << 2) << endl \
		<< "               entry number: " << path_graph.size() << endl \
		<< "Signature avg. number of q-gram: " << setiosflags(ios::fixed) << setprecision(2) << (float)qnum/(float)gdb_size << " <max " << max_len << ">" << endl \
		<< "          multiset size in byte: " << (qs+4)*qnum << endl \
		<< "= = = = = = = = = = = = = = = = = = = = = = = = = =" << endl; 

inline int EstimateEdge( Graph &g1, Graph &g2, const int remaining_edge_num_g1, const int remaining_edge_num_g2 ) {
	return abs(remaining_edge_num_g1 - remaining_edge_num_g2);
}

inline int edge_diff( unsigned* elg, unsigned* erg, const unsigned es ) {
	int diff = 0;
	for (unsigned i = 0; i != es; ++ i) {
		diff += elg[i] - (elg[i] > erg[i]? erg[i]: elg[i]);
	}
	return diff;
}

inline void set_tau() {	tau = 4; }
inline void reset_tau() { tau = 4; }

inline void compute_rud_dist( Priority* pri, int& ans, bool filter_only = false ) {
	if (filter_only) return;
	v_time.resume();
	pri->init_astar();
	int edist = rudim_edit_distance(pri);
	ofstream outfile("aids_result.txt");
	if (print_more)
		cout << pri->lgid << "(" << pri->lg->name << ") - " << pri->rgid << "(" << pri->rg->name << ") " << edist << " #" << nodes_expanded << endl;
		//outfile << pri->lgid << "(" << pri->lg->name << ") - " << pri->rgid << "(" << pri->rg->name << ") " << edist << " #" << nodes_expanded << endl;
	if (edist != OVER_BOUND) {
		++ ans;
		if (!print_more & print_ans)
			cout << pri->lgid << "(" << pri->lg->name << ") - " << pri->rgid << "(" << pri->rg->name << ") " << edist << " #" << nodes_expanded << endl;
			//outfile << pri->lgid << "(" << pri->lg->name << ") - " << pri->rgid << "(" << pri->rg->name << ") " << edist << " #" << nodes_expanded << endl;
	}
	pri->roll_back();
	v_time.pause();
}

inline void compute_opt_dist( Priority* pri, int& ans, bool filter_only = false ) {
	if (filter_only) return;
	v_time.resume();
	pri->init_astar();
	int edist = filter_edit_distance(pri);
	if (print_more)
		cerr << pri->lgid << "(" << pri->lg->name << ") - " << pri->rgid << "(" << pri->rg->name << ") " << edist << " #" << nodes_expanded << endl;
	if (edist != OVER_BOUND) {
		++ ans;
		if (!print_more & print_ans)
			cerr << pri->lgid << "(" << pri->lg->name << ") - " << pri->rgid << "(" << pri->rg->name << ") " << edist << " #" << nodes_expanded << endl;
	}
	pri->roll_back();
	v_time.pause();
}
