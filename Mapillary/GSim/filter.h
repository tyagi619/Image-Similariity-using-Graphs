#pragma once

#include "path_join.h"


bool size_filtering(const unsigned p, const unsigned q);
bool label_filtering(const unsigned p, const unsigned q);
bool count_filtering(const unsigned p, const unsigned q);
bool tight_deg_filtering(unsigned p, unsigned q, const int threshold);
bool deg_path_match(const unsigned p, const unsigned pid, const unsigned q, unsigned j, const unsigned size, bool* q_uni_took, unsigned& count);
Priority* content_filtering(const unsigned p, const unsigned q);
Priority* norder_content_filtering(const unsigned p, const unsigned q);

#define CLEAR_MN \
	delete[] m_cnt; \
	delete[] n_cnt; \
	delete[] p_nomat; \
	delete[] q_nomat; 

#define CLEAR_P \
	delete p_compo; \
	for (k = 0; k != i; ++ k) { \
	delete[] p_id[k]; \
	} \
	delete[] p_id; \
	delete[] p_size; \
	delete[] p_err; \
	return NULL; 

#define CLEAR_Q \
	delete p_compo; \
	delete q_compo; \
	for (k = 0; k != j; ++ k) { \
	delete[] q_id[k]; \
	} \
	delete[] q_id; \
	delete[] q_size; \
	delete[] p_err; \
	delete[] q_err; \
	return NULL; 

#define CLEAR_MP \
	delete[] cpy_nomat; \
	delete p_compo; \
	for (unsigned j = 0; j != i; ++ j) { \
	delete[] p_id[j]; \
	} \
	delete[] p_id; \
	delete[] p_size;

#define IF_MATCH \
	if (err <= tau) { \
	if (pos_mark == -1) { \
	pos_mark = j; \
	} else { \
	++ count; \
	delete[] diff; \
	return true; \
	} \
	}

extern unsigned gdb_size;
extern unsigned** card;					
extern unsigned** tau_card;
extern vector<int>* elab;				
extern vector<unsigned>* ecnt;			
extern vector<int>* vlab;			
extern vector<unsigned>* vcnt;			