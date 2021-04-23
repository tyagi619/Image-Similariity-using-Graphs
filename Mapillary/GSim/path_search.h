#pragma once

#include "astar.h"


void preprocess(const unsigned q);

/* full graph similarity search */
void proc_full_cnt_order(const unsigned q);
void proc_full_rud_order(const unsigned q);
void proc_full_imp_order(const unsigned q);
void full_graph_min_index();
void full_graph_cnt_index();
void run_cnt_search();
void run_sim_search();

/* rs-join */
void run_rs_join(void);  // akin to query R using S

#define ECHO_SEARCH \
	cout << "\nCand_1: " << setiosflags(ios::fixed) << setprecision(3) << gl_cand_1 << " / avg. " << double(gl_cand_1/qry_size) << endl \
	<< "Cand_2: " << setiosflags(ios::fixed) << setprecision(3) << gl_cand_2 << " / avg. " << double(gl_cand_2/qry_size) << endl \
	<< "Answer: " << setiosflags(ios::fixed) << setprecision(3) << gl_ans << " / avg. " << double(gl_ans/qry_size) << endl \
	<< "Filtering time: " << setiosflags(ios::fixed) << setprecision(6) << _sec + _usec/1e6 - v_time.get_time() << "s\n" \
	<< "Verification time: " << setiosflags(ios::fixed) << setprecision(6) << v_time.get_time() << "s\n" \
	<< "Response time: " << setiosflags(ios::fixed) << setprecision(6) << _sec + _usec/1e6 << "s" << endl;

extern unsigned qry_size;
extern vector<Path_gram*>* pdb;			
extern Hash_gram** hdb;					
extern unsigned global_esize;
extern unsigned* global_elabs;
extern bool count_filter, min_edit_filter, local_lab_filter, deg_match_cond, filter_only;
extern char vf_order, version;