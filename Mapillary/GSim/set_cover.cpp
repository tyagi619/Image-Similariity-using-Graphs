#include "set_cover.h"
#include "path_join.h"
#include "path_search.h"
#include "misc.h"
#include "filter.h"


int threshold_set_cover( const int threshold, int depth, const int start, const unsigned num_remain, bool* coverage, const unsigned size, vector<unsigned>** edset, const unsigned len )
{
	bool* nxt_coverage = new bool[size];
	unsigned nxt_remain;
	if (++ depth == threshold) {
		for (unsigned i = start; i != len; ++ i) {
			memcpy(nxt_coverage, coverage, size*sizeof(bool));
			nxt_remain = update_coverage(nxt_coverage, edset[i], num_remain);
			if (nxt_remain == 0) {
				delete[] nxt_coverage;
				return 1;	
			}
		}
		delete[] nxt_coverage;
		return 2;
	} else {
		int sub_min = over_tau, temp;
		for (unsigned i = start; i != len; ++ i) {	
			memcpy(nxt_coverage, coverage, size*sizeof(bool));
			nxt_remain = update_coverage(nxt_coverage, edset[i], num_remain);
			if (nxt_remain == 0) {
				delete[] nxt_coverage;
				return 1;
			} else {
				temp = threshold_set_cover(threshold, depth, i+1, nxt_remain, nxt_coverage, size, edset, len);
				if (temp <= threshold - depth) {
					delete[] nxt_coverage;
					return (++ temp);
				} else if (temp < sub_min) {
					sub_min = temp;
				}
			}
		}
		delete[] nxt_coverage;
		return (++ sub_min);
	}
}

int exact_set_cover( const int threshold, int depth, const int start, const unsigned num_remain, bool* coverage, const unsigned size, vector<unsigned>** edset, const unsigned len )
{
	if (++ depth == threshold) {	
		bool* nxt_coverage = new bool[size];
		for (unsigned i = start; i != len; ++ i) {
			memcpy(nxt_coverage, coverage, size*sizeof(bool));
			if (update_coverage(nxt_coverage, edset[i], num_remain) == 0) {
				delete[] nxt_coverage;
				return 1;	
			}
		}
		delete[] nxt_coverage;
		return 2;
	} else {
		bool** nxt_coverage = new bool*[len-start];
		unsigned* nxt_remain = new unsigned[len-start];
		for (unsigned i = start; i != len; ++ i) {
			nxt_coverage[i-start] = new bool[size];
			memcpy(nxt_coverage[i-start], coverage, size*sizeof(bool));
			nxt_remain[i-start] = update_coverage(nxt_coverage[i-start], edset[i], num_remain);
			if (nxt_remain[i-start] == 0) {
				for (unsigned j = 0; j <= i-start; ++ j) {
					delete[] nxt_coverage[j]; 
				}
				delete[] nxt_coverage; delete[] nxt_remain;
				return 1;
			}
		}
		int sub_min = over_tau, temp;
		for (unsigned i = start; i != len; ++ i) {	
			temp = exact_set_cover(threshold, depth, i+1, nxt_remain[i-start], nxt_coverage[i-start], size, edset, len);
			if (temp == 1) { 
				sub_min = temp; break;
			}
			if (temp < sub_min) {
				sub_min = temp;
			}
		}
		for (unsigned j = 0; j != len-start; ++ j) {
			delete[] nxt_coverage[j]; 
		}
		delete[] nxt_coverage; delete[] nxt_remain;
		return (++ sub_min);	
	}
}

unsigned update_coverage( bool* coverage, vector<unsigned>* site, unsigned num_remain )
{
	for (unsigned j = site->size(), i = 0; i != j; ++ i) {
		if (!coverage[(*site)[i]]) {
			coverage[(*site)[i]] = true;
			-- num_remain;
		}
	}
	return num_remain;
}

/* threshold is set to tau */
int thresh_ext_err( const unsigned g, const unsigned size )
{
	set<unsigned> vset;
	for (unsigned i = 0; i != size; ++ i) {
		merge_vertex_set(pdb[g][i]->vpath, vset);
	}
	
	vector<unsigned>** edset = new vector<unsigned>*[vset.size()];
	unsigned len = 0;
	for (set<unsigned>::const_iterator it = vset.begin(), end = vset.end(); it != end; ++ len, ++ it) {
		edset[len] = new vector<unsigned>;
		copy_vec(eds[g][*it], edset[len], size);
	}
	reduce_vec(edset, len, 1 + (size >> 5));	

	int err;
	if (len < 3) {
		err = len;
	} else {
		bool* coverage = new bool[size];
		memset(coverage, 0, sizeof(bool)*size);
		err = threshold_set_cover(tau, 0, 0, size, coverage, size, edset, len);	
		delete[] coverage;
	}

	for (unsigned j = vset.size(), i = 0; i != j; ++ i) {
		delete edset[i];
	}
	delete[] edset;
	
	return err;
}

int mismat_cot_err( const unsigned g, const unsigned h, vector<unsigned>& compo, unsigned* pid, const unsigned pid_size, const int threshold )
{
	vector<unsigned>** edset = new vector<unsigned>*[compo.size()];
	unsigned len = 0;
	for (vector<unsigned>::const_iterator it = compo.begin(), end = compo.end(); it != end; ++ len, ++ it) {
		edset[len] = new vector<unsigned>;
		copy_vec(eds[g][*it], edset[len], pid, pid_size);
	}
	reduce_vec(edset, len);

	int loc_err;
	if (len < 3) {	
		loc_err = len;
	} else {
		bool* coverage = new bool[pid_size];
		memset(coverage, 0, pid_size*sizeof(bool));
		loc_err = exact_set_cover(threshold, 0, 0, pid_size, coverage, pid_size, edset, len);
		delete[] coverage;
	}

	for (unsigned i = 0, j = compo.size(); i != j; ++ i) {
		delete edset[i];
	}
	delete[] edset;

	if (loc_err > threshold)
		return loc_err;

	/* vertex label */
	Graph &gg = gdb[g];
	int cot_err = 0;
	int *lab = new int[qs*pid_size], lab_cnt = 0;
	for (vector<unsigned>::const_iterator it = compo.begin(), end = compo.end(); it != end; ++ lab_cnt, ++ it) {
		lab[lab_cnt] = gg[*it].vlabel;
	}
	sort(lab, lab+lab_cnt);
	unsigned cumu = 1, pos;
	len = 1;
	int temp_lab = lab[0];
	while (len != lab_cnt) {
		if (lab[len] == temp_lab) {
			++ cumu, ++ len;
		} else {
			pos = bin_search(vlab[h], lab[len-1]);
			if (pos == vlab[h].size()) {	
				cot_err += cumu;
			} else if (cumu > vcnt[h][pos]) {
				cot_err += cumu - vcnt[h][pos];
			}
			temp_lab = lab[len];
			cumu = 1;
			++ len;
		}
	}
	pos = bin_search(vlab[h], lab[len-1]);
	if (pos == vlab[h].size()) {	
		cot_err += cumu;
	} else if (cumu > vcnt[h][pos]) {
		cot_err += cumu - vcnt[h][pos];
	}

	if (cot_err > threshold) {
		delete[] lab;
		return over_tau;
	}

	/* edge label */
	lab_cnt = 0;
	set<unsigned> edge;
	for (unsigned j = 0; j != pid_size; ++ j) {
		for (int k = 0; k != under_qs; ++ k) {
			if (edge.find(pdb[g][pid[j]]->epath[k]) == edge.end()) {
				edge.insert(pdb[g][pid[j]]->epath[k]);
				lab[lab_cnt] = gg.get_elab(pdb[g][pid[j]]->vpath[k], pdb[g][pid[j]]->vpath[k+1]);
				++ lab_cnt;
			}
		}
	}
	sort(lab, lab+lab_cnt);
	len = 1, cumu = 1;
	temp_lab = lab[0];
	while (len != lab_cnt) {
		if (lab[len] == temp_lab) {
			++ cumu, ++ len;
		} else {
			pos = bin_search(elab[h], lab[len-1]);
			if (pos == elab[h].size()) {
				cot_err += cumu;
			} else if(cumu > ecnt[h][pos]) {
				cot_err += cumu - ecnt[h][pos];
			}
			cumu = 1, ++ len;
		}
	}
	pos = bin_search(elab[h], lab[len-1]);
	if (pos == elab[h].size()) {	
		cot_err += cumu;
	} else if (cumu > ecnt[h][pos]) {
		cot_err += cumu - ecnt[h][pos];
	}

	delete[] lab;
	return (loc_err > cot_err? loc_err: cot_err);
}

int mismat_cot_err( const unsigned g, const unsigned h, set<unsigned>& compo, unsigned* pid, const unsigned pid_size, const int threshold )
{
	vector<unsigned>** edset = new vector<unsigned>*[compo.size()];
	unsigned len = 0;
	for (set<unsigned>::const_iterator it = compo.begin(), end = compo.end(); it != end; ++ len, ++ it) {
		edset[len] = new vector<unsigned>;
		copy_vec(eds[g][*it], edset[len], pid, pid_size);
	}
	reduce_vec(edset, len);

	int loc_err;
	if (len < 3) {
		loc_err = len;
	} else {
		bool* coverage = new bool[pid_size];
		memset(coverage, 0, pid_size*sizeof(bool));
		loc_err = exact_set_cover(threshold, 0, 0, pid_size, coverage, pid_size, edset, len);
		delete[] coverage;
	}

	for (unsigned i = 0, j = compo.size(); i != j; ++ i) {
		delete edset[i];
	}
	delete[] edset;

	if (loc_err > threshold) {
		return loc_err;
	}

	/* content-based filtering */
	int cot_err = 0;
	int *lab = new int[qs*pid_size], lab_cnt = 0;
	for (set<unsigned>::const_iterator it = compo.begin(); it != compo.end(); ++ lab_cnt, ++ it) {
		lab[lab_cnt] = gdb[g][*it].vlabel;
	}
	sort(lab, lab+lab_cnt);
	unsigned cumu = 1, pos;
	len = 1;
	int temp_lab = lab[0];
	while (len != lab_cnt) {
		if (lab[len] == temp_lab) {
			++ cumu, ++ len;
		} else {
			pos = bin_search(vlab[h], lab[len-1]);
			if (pos == vlab[h].size()) {	
				cot_err += cumu;
			} else if (cumu > vcnt[h][pos]) {
				cot_err += cumu - vcnt[h][pos];
			}
			temp_lab = lab[len];
			cumu = 1;
			++ len;
		}
	}
	pos = bin_search(vlab[h], lab[len-1]);
	if (pos == vlab[h].size()) {	
		cot_err += cumu;
	} else if (cumu > vcnt[h][pos]) {
		cot_err += cumu - vcnt[h][pos];
	}

	if (cot_err > threshold) {
		delete[] lab;
		return over_tau;
	}

	lab_cnt = 0;
	set<unsigned> edge;
	for (unsigned j = 0; j != pid_size; ++ j) {
		for (int k = 0; k != under_qs; ++ k) {
			if (edge.find(pdb[g][pid[j]]->epath[k]) == edge.end()) {
				edge.insert(pdb[g][pid[j]]->epath[k]);
				lab[lab_cnt] = gdb[g].get_elab(pdb[g][pid[j]]->vpath[k], pdb[g][pid[j]]->vpath[k+1]);
				++ lab_cnt;
			}
		}
	}
	sort(lab, lab+lab_cnt);
	len = 1, cumu = 1;
	temp_lab = lab[0];
	while (len != lab_cnt) {
		if (lab[len] == temp_lab) {
			++ cumu, ++ len;
		} else {
			pos = bin_search(elab[h], lab[len-1]);
			if (pos == elab[h].size()) {
				cot_err += cumu;
			} else if(cumu > ecnt[h][pos]) {
				cot_err += cumu - ecnt[h][pos];
			}
			cumu = 1, ++ len;
		}
	}
	pos = bin_search(elab[h], lab[len-1]);
	if (pos == elab[h].size()) {	
		cot_err += cumu;
	} else if (cumu > ecnt[h][pos]) {
		cot_err += cumu - ecnt[h][pos];
	}

	delete[] lab;
	return (loc_err > cot_err? loc_err: cot_err);
}