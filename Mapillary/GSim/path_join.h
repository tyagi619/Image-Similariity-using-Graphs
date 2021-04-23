#pragma once

#include "astar.h"


void generate_all_path(void);
void dfs_path(const unsigned g, const unsigned source, const unsigned v, Path_gram* prv_gram, const int depth, bool* color);
void count_all_path(void);
void vectorize_label(void);
void cnt_prefix_list(void);
void join_min_prefix_index(void);
unsigned cnt_plength(const unsigned g);
unsigned min_plength(const unsigned g);
void cnt_filter_join(void);
void rud_order_join(void);
void opt_order_join(void);
void imp_order_join(void);
void run_min_prefix(void);
void run_cnt_prefix(void);
unsigned componentize(const unsigned g, unsigned* nomat, unsigned cnt, vector<vector<unsigned> >* component, unsigned**& npid, unsigned*& nsize);

#define ECHO_JOIN \
	cout << "= = = = = = = = = = = = = = = = = = = = = = = = = =" << endl \
	<< "Candidates before non-trivial filters: " << cand << " <" << setiosflags(ios::fixed) << setprecision(2) \
	<< 100*(static_cast<float>(cand) / static_cast<float>(gdb_size*(gdb_size-1) >> 1)) << "%>" << endl \
	<< "Candidates need GED computation: " << dist_cand << " <" << setiosflags(ios::fixed) << setprecision(2) \
	<< 100*(static_cast<float>(dist_cand) / static_cast<float>(gdb_size*(gdb_size-1) >> 1)) << "%>" << endl \
	<< "Answers: " << ans << " / candidate pairs <" << setiosflags(ios::fixed) << setprecision(2) \
	<< 100*(static_cast<float>(ans) / static_cast<float>(dist_cand)) << "%>" << endl \
	<< "= = = = = = = = = = = = = = = = = = = = = = = = = =" << endl;

inline int count_lower_bound( const unsigned path_num, const int dest_num ) {
	return (path_num <= dest_num? 0: (path_num - dest_num));
}

// main data structure
extern vector<Graph> gdb;
extern vector<Path_gram*>* pdb;			
extern Hash_gram** hdb;					
extern vector<unsigned> uid;			
extern set<unsigned>** eds;
extern unsigned* glen;
extern fmap path_freq;
extern gmap path_graph;

// assist data structure
extern unsigned gdb_size;
extern int* status;
extern unsigned global_esize;
extern unsigned* global_elabs;
extern unsigned** card;					
extern unsigned** tau_card;
extern vector<int>* elab;				
extern vector<unsigned>* ecnt;			
extern vector<int>* vlab;				
extern vector<unsigned>* vcnt;			
extern vector<iter_graph>* git;
extern Freq_comp fcomp;
extern bool count_filter, min_edit_filter, local_lab_filter, deg_match_cond, filter_only;
extern char vf_order, version;
extern unsigned max_vnum;