#pragma once

#include "misc.h"


class Edge
{
public:
	unsigned eid;
	unsigned from;
	unsigned to;
	int elabel;
};

class Vertex
{
public:
	int vlabel;
	unsigned degree;
	unsigned* elabs;
	vector<Edge> edges;	

	Vertex(void): vlabel(DUMMY_LABEL), degree(0) {}
	void push_edge(unsigned eid, unsigned from, unsigned to, int elabel);
};

class Graph: public vector<Vertex>
{
public:
	Graph(void):
	  vertex_num(0), edge_num(0), vlabel_list(NULL), elabel_list(NULL), vid_to_vlabel(NULL), eid_to_elabel(NULL) {};
	~Graph() {
		delete[] vlabel_list;
		delete[] elabel_list;
		delete[] vid_to_vlabel;
		delete[] eid_to_elabel;
	}

	char read(istream& is, char prev_tag, set<int>& edge_label);
	char read(istream& is, char prev_tag);
	void write(ostream& os);
	void sort_edge();
	int get_elab(const unsigned i, const unsigned j) const;
	void get_order();	
	bool edge_feasible(unsigned i, unsigned j) const;
	bool connectivity() const;
	bool simplicity() const;
	unsigned is_adjacent(unsigned i, unsigned j) const {
		for (unsigned k = 0; k < (*this)[i].degree; ++ k) {
			if ((*this)[i].edges[k].to == j) return (*this)[i].edges[k].eid;
			if ((*this)[i].edges[k].to > j) return -1;
		}
		return -1;
	} 

	// public class members
	unsigned name;
	unsigned vertex_num;
	unsigned edge_num;
	unsigned total_num;
	unsigned path_num;
	int* vlabel_list;
	int* elabel_list;
	int* vid_to_vlabel;
	vector<int>* eid_to_elabel;
};

class Graph_comp 
{
public:
	bool operator() (const Graph &lg, const Graph &rg) const {
		if (lg.total_num == rg.total_num) {
			if (lg.vertex_num == rg.vertex_num) {
				if (lg.edge_num == rg.edge_num) return (lg.name < rg.name);
				else return (lg.edge_num < rg.edge_num);
			} else return (lg.vertex_num < rg.vertex_num);
		} else return (lg.total_num < rg.total_num);
	}
};

class Edge_comp
{
public:
	bool operator() (const Edge& le, const Edge& re) {
		return (le.to < re.to);
	}
};

class Path_gram
{
public:
	char* chars_path;
	unsigned hash_val;
	unsigned* vpath;
	unsigned* epath;
	bool normal;	
	bool backward;

	Path_gram(): backward(false) {
		chars_path = new char[path_qs];
		vpath = new unsigned[qs];
		epath = new unsigned[under_qs];
	}
	Path_gram(Path_gram& pgram): backward(false) {
		chars_path = new char[path_qs];
		memcpy(chars_path, pgram.chars_path, sizeof(char)*path_qs);
		vpath = new unsigned[qs];
		memcpy(vpath, pgram.vpath, sizeof(unsigned)*qs);
		epath = new unsigned[under_qs];
		memcpy(epath, pgram.epath, sizeof(unsigned)*under_qs);
	}
	~Path_gram() { 
		delete[] chars_path;
		delete[] vpath;
		delete[] epath;
	}
	void fill_path_info() {
		normal = normalize_string(chars_path, path_qs, backward);
		hash_val = bitwise_hash(chars_path, path_qs);
	}
};

class Hash_gram
{
public:
	unsigned hash_val;
	unsigned pid;
};

class Freq_comp
{
public:
	fmap* fm;
	Freq_comp(fmap* m): fm(m) {}

	bool operator() (const Path_gram* lp, const Path_gram* rp) const {
		return ((*fm)[lp->hash_val] < (*fm)[rp->hash_val]
		|| (*fm)[lp->hash_val] == (*fm)[rp->hash_val] && lp->hash_val < rp->hash_val);
	}
};

class Path_comp 
{
public:
	bool operator() (const Path_gram* lp, const Path_gram* rp) const {
		return (lp->hash_val < rp->hash_val);
	}

	bool operator() (const Hash_gram& lh, const Hash_gram& rh) const {
		return (lh.hash_val < rh.hash_val || lh.hash_val == rh.hash_val && lh.pid < rh.pid);
	}
};

Hash_gram* copy_value(vector<Path_gram*>& p);

class VertexPair {
public:
	unsigned vid, att;

	VertexPair(): vid(0), att(0) {}
};

bool operator < ( const VertexPair& x, const VertexPair& y );

class VertexPairTwo {
public:
	unsigned vid, deg, att;

	VertexPairTwo(): vid(0), deg(0), att(0) {}
};

bool operator < ( const VertexPairTwo& x, const VertexPairTwo& y );