#pragma once

#include "common.h"


class Pair_comp
{
public:
	bool operator() (const pair<unsigned, int>& lhs, const pair<unsigned, int>& rhs) const {
		return (lhs.second > rhs.second);
	}
};

class Container_comp
{
public:
	bool operator() (const set<unsigned>* lhs, const set<unsigned>* rhs) const {
		return (lhs->size() > rhs->size());
	}

	bool operator() (const set<unsigned>& lhs, const set<unsigned>& rhs) const {
		return (lhs.size() > rhs.size());
	}

	bool operator() (const vector<unsigned>* lhs, const vector<unsigned>* rhs) const {
		return (lhs->size() > rhs->size());
	}

	bool operator() (const vector<unsigned>& lhs, const vector<unsigned>& rhs) const {
		return (lhs.size() > rhs.size());
	}
};

class Disjoin_set 
{
public:
	unsigned* father_table;

	Disjoin_set(const unsigned s) {
		father_table = new unsigned[s];
	}
	~Disjoin_set() { delete[] father_table; }
	
	unsigned find_father(const unsigned x) {
		if (father_table[x] == x) return x;
		else return find_father(father_table[x]);
	}
	void union_set(const unsigned x, const unsigned y) {
		father_table[find_father(x)] = find_father(y);
	}
	bool same_set(const unsigned x, const unsigned y) {
		return find_father(x) == find_father(y);
	}
};

unsigned in_list(unsigned* list, const unsigned len, const unsigned target);
unsigned in_list(vector<unsigned>& list, const unsigned target);
bool normalize_string(char* chars, int right, bool& backward);
bool* init_bool_array(const bool val, const unsigned size);
unsigned bitwise_hash(char* key, const int size);
unsigned bin_search(unsigned* array, const unsigned size, const unsigned target);
unsigned bin_search(vector<int>& array, const int target);
unsigned bin_search(vector<unsigned>* array, const unsigned start, const unsigned target);
unsigned bin_search(vector<int>& array, const unsigned start, const int target);
bool vec_containment(vector<unsigned>* lhs, vector<unsigned>* rhs);
bool bit_vec_containment(unsigned long long* lhs, unsigned long long* rhs, const unsigned bit_size);
void copy_vec(set<unsigned>& src, vector<unsigned>* dst, const unsigned size);
void copy_vec(set<unsigned>& src, vector<unsigned>* dst, unsigned* pid, const unsigned size);
void reduce_vec(vector<unsigned>** st, unsigned& size, const unsigned bit_size);
void reduce_vec(vector<unsigned>** st, unsigned& size);
void merge_vertex_set(unsigned* vpath, set<unsigned>& s);