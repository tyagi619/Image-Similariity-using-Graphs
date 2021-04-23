#include "graph.h"


void Vertex::push_edge( unsigned eid, unsigned from, unsigned to, int elabel )
{
	unsigned n = edges.size();
	edges.resize(n+1);
	edges[n].eid = eid;
	edges[n].from = from;
	edges[n].to = to;
	edges[n].elabel = elabel;
}

char Graph::read( istream& is, char prev_tag, set<int>& edge_label )
{
	char tag = prev_tag;
	unsigned i, j, k;

	if (tag == 't') {
		is >> tag >> i >> tag;
		this->name = i;
	} else {
		cerr << "ill-formated file!" << endl;
		exit(1);
	}

	while (!is.eof() && (tag == 'v')) {
		is >> i >> j >> tag;
		this->resize(this->size()+1);
		this->back().vlabel = j;
	}
	unsigned cnt = 0;
	while (!is.eof() && (tag == 'e')) {
		is >> i >> j >> k >> tag;	// does not control redundancy
		if (i != j) {
			if (i >= this->size() || j >= this->size()) {
				cerr << "(" << i << ", " << j << ") - incorrect edge entry at graph " << this->name << "!" << endl;
				cerr << "program ignored the entry..." << endl; continue;
			}

			(*this)[i].push_edge(cnt, i, j, k);
			(*this)[j].push_edge(cnt, j, i, k);
			edge_label.insert(k);
			++ cnt;
		} else {
			cerr << "a possible erroneous entry at graph " << this->name << "!" << endl;
			cerr << "program ignored the entry..." << endl;
		}
	}

	vertex_num = this->size();
	for (unsigned i = 0; i != vertex_num; ++ i) {
		(*this)[i].degree = (*this)[i].edges.size();
	}
	edge_num = cnt;	
	total_num = vertex_num + edge_num;
	sort_edge();

	return tag;
}

char Graph::read( istream& is, char prev_tag )
{
	char tag = prev_tag;
	unsigned i, j, k;

	if (tag == 't') {
		is >> tag >> i >> tag;
		this->name = i;
	}

	while (!is.eof() && (tag == 'v')) {
		is >> i >> j >> tag;
		this->resize(this->size()+1);
		this->back().vlabel = j;
	}
	unsigned cnt = 0;
	while (!is.eof() && (tag == 'e')) {
		is >> i >> j >> k >> tag;	// does not control redundancy
		if (i != j) {
			if (i >= this->size() || j >= this->size()) {
				cerr << "(" << i << ", " << j << ") - incorrect edge entry at graph " << this->name << "!" << endl;
				cerr << "program ignored the entry..." << endl; continue;
			}

			(*this)[i].push_edge(cnt, i, j, k);
			(*this)[j].push_edge(cnt, j, i, k);
			++ cnt;
		} else {
			cerr << "a possible erroneous entry at graph " << this->name << "!" << endl;
			cerr << "program ignored the entry..." << endl;
		}
	}

	vertex_num = this->size();
	for (unsigned i = 0; i != vertex_num; ++ i) {
		(*this)[i].degree = (*this)[i].edges.size();
	}
	edge_num = cnt;	
	total_num = vertex_num + edge_num;
	sort_edge();

	return tag;
}

int Graph::get_elab( const unsigned i, const unsigned j ) const
{
	for (unsigned m = 0, n = (*this)[i].degree; m != n; ++ m) {
		if ((*this)[i].edges[m].to == j) return (*this)[i].edges[m].elabel;
		if ((*this)[i].edges[m].to > j) return DUMMY_LABEL;
	}
	return DUMMY_LABEL;
}

void Graph::write( ostream& os )
{
	os << "t " << name << " " << vertex_num << endl;
	for (unsigned i = 0; i != vertex_num; ++ i) {
		os << "v " << i << " " << (*this)[i].vlabel << endl; 
	}
	for (unsigned i = 0; i != vertex_num; ++ i) {
		for (unsigned j = 0; j != (*this)[i].degree; ++ j) {
			if ((*this)[i].edges[j].from < (*this)[i].edges[j].to) {
				os << "e " << (*this)[i].edges[j].from << " " << (*this)[i].edges[j].to << " " << (*this)[i].edges[j].elabel << endl;
			}
		}
	}
}

void Graph::sort_edge()
{
	for (unsigned i = 0; i != vertex_num; ++ i) {
		sort((*this)[i].edges.begin(), (*this)[i].edges.end(), Edge_comp());
	}
}

bool Graph::edge_feasible( unsigned i, unsigned j ) const
{
	for (unsigned m = 0, n = (*this)[i].degree; m != n; ++ m) {
		if ((*this)[i].edges[m].to == j) return true;
		if ((*this)[i].edges[m].to > j) return false;
	}
	return false;
}

bool Graph::connectivity() const
{
	for (unsigned i = 0; i < vertex_num; ++ i) {
		if ((*this)[i].degree == 0) return false;
	}

	Disjoin_set ds(vertex_num);
	for (unsigned i = 0; i < vertex_num; ++ i) {
		ds.father_table[i] = i;
	}
	for (unsigned i = 0; i < vertex_num; ++ i) {
		for (unsigned j = 0; j < (*this)[i].degree; ++ j) {
			if (i > (*this)[i].edges[j].to) {
				ds.union_set(i, (*this)[i].edges[j].to);
			}
		}
	}
	for (unsigned i = 1; i < vertex_num; ++ i) {
		if (!ds.same_set(0, i)) return false;
	}
	return true;
}

bool Graph::simplicity() const
{
	for (unsigned i = 0; i < vertex_num; ++ i) {
		if ((*this)[i].degree > 1) {
			for (unsigned j = 1; j < (*this)[i].degree; ++ j) {
				if ((*this)[i].edges[j].to == (*this)[i].edges[j-1].to) 
					return false;
			}
		}
	}
	return true;
}

Hash_gram* copy_value(vector<Path_gram*>& p) {
	unsigned j = p.size();
	Hash_gram* h = new Hash_gram[j];
	for (unsigned i = 0; i != j; ++ i) {
		h[i].hash_val = p[i]->hash_val;
		h[i].pid = i;
	}
	sort(h, h+j, Path_comp());
	return h;
}

bool operator < ( const VertexPair& x, const VertexPair& y ) {
	return (x.att < y.att || x.att == y.att && x.vid < y.vid);
}

bool operator < ( const VertexPairTwo& x, const VertexPairTwo& y ) {
	return (x.att < y.att || x.att == y.att && x.deg > y.deg || x.att == y.att && x.deg == y.deg && x.vid < y.vid);
}