#include "filter.h"
#include "set_cover.h"
#include "misc.h"

extern int max_err;


bool size_filtering( const unsigned p, const unsigned q )
{
	int err = abs((int)gdb[p].vertex_num - (int)gdb[q].vertex_num);
	if (err <= tau) {
		err += abs((int)gdb[p].edge_num - (int)gdb[q].edge_num);
		if (err > max_err) max_err = err;	
		return (err <= tau);
	} else 
		return false;
}

bool label_filtering( const unsigned p, const unsigned q )
{
	int v_err = 0, m = vlab[p].size(), n = vlab[q].size(), overlap = 0;
	unsigned s = 0, t = 0;
	while (s != m && t != n) {
		if (vlab[p][s] == vlab[q][t]) {
			overlap += vcnt[p][s] <= vcnt[q][t]? vcnt[p][s]: vcnt[q][t];
			++ s, ++ t;
		} else if (vlab[p][s] < vlab[q][t]) {
			++ s;
		} else {
			++ t;
		}
	}

	m = gdb[p].vertex_num - overlap, n = gdb[q].vertex_num - overlap;
	v_err = m > n? m: n;

	if (v_err > tau) return false;

	int e_err = 0;
	s = 0, t = 0;
	m = elab[p].size(), n = elab[q].size();
	overlap = 0;
	while (s != m && t != n) {
		if (elab[p][s] == elab[q][t]) {
			overlap += ecnt[p][s] <= ecnt[q][t]? ecnt[p][s]: ecnt[q][t];
			++ s, ++ t;
		} else if (elab[p][s] < elab[q][t]) {
			++ s;
		} else {
			++ t;
		}
	}

	m = gdb[p].edge_num - overlap, n = gdb[q].edge_num - overlap;
	e_err = m > n? m: n;
	return (v_err + e_err <= tau);
}

bool count_filtering( const unsigned p, const unsigned q )
{
	unsigned i = 0, j = 0;
	const unsigned m = gdb[p].path_num, n = gdb[q].path_num;
	int overlap = 0;
	while (i != m && j != n) {
		if (hdb[p][i].hash_val == hdb[q][j].hash_val) {
			++ overlap;
			++ i, ++ j;
		} else if (hdb[p][i].hash_val < hdb[q][j].hash_val) 
			++ i;
		else 
			++ j;
	}

	return (overlap >= count_lower_bound(m, tau_card[p][tau]) && overlap >= count_lower_bound(n, tau_card[q][tau]));
}

Priority* norder_content_filtering( const unsigned p, const unsigned q )
{
	unsigned i = 0, j = 0, k;
	const unsigned m = gdb[p].path_num, n = gdb[q].path_num;
	unsigned *p_nomat = new unsigned[m], p_cnt = -1;
	unsigned *q_nomat = new unsigned[n], q_cnt = -1;
	unsigned m_beg, *m_cnt =  new unsigned[m];
	unsigned *n_cnt = new unsigned[n];
	unsigned pos = 0;

	/* count mismatching q-grams */
	if (!deg_match_cond) {
		while (i != m && j != n) {
			if (hdb[p][i].hash_val == hdb[q][j].hash_val) {
				m_cnt[pos] = n_cnt[pos] = 0;
				while (true) {
					++ m_cnt[pos];
					if (++ i == m || hdb[p][i].hash_val != hdb[p][i-1].hash_val) break;
				}
				while (true) {
					++ n_cnt[pos];
					if (++ j == n || hdb[q][j].hash_val != hdb[q][j-1].hash_val) break;
				}
				++ pos;
			} else if (hdb[p][i].hash_val < hdb[q][j].hash_val) {
				p_nomat[++ p_cnt] = hdb[p][i].pid;
				++ i;
			} else {
				q_nomat[++ q_cnt] = hdb[q][j].pid;
				++ j;
			}		
		}
	} else {
		bool *p_uni_took = new bool[m], *q_uni_took = new bool[n];
		memset(p_uni_took, 0, sizeof(bool)*m);
		memset(q_uni_took, 0, sizeof(bool)*n);
		while (i != m && j != n) {
			if (hdb[p][i].hash_val == hdb[q][j].hash_val) {	
				m_beg = i;
				m_cnt[pos] = n_cnt[pos] = 0;
				while (true) {
					if (!deg_path_match(p, hdb[p][i].pid, q, j, n, q_uni_took, m_cnt[pos])) 
						p_nomat[++ p_cnt] = hdb[p][i].pid;
					if (++ i == m || hdb[p][i].hash_val != hdb[p][i-1].hash_val) break;
				}
				while (true) {
					if (!deg_path_match(q, hdb[q][j].pid, p, m_beg, i, p_uni_took, n_cnt[pos])) 
						q_nomat[++ q_cnt] = hdb[q][j].pid;
					if (++ j == n || hdb[q][j].hash_val != hdb[q][j-1].hash_val) break;
				}
				++ pos;
			} else if (hdb[p][i].hash_val < hdb[q][j].hash_val) {				
				p_nomat[++ p_cnt] = hdb[p][i].pid;
				++ i;
			} else {
				q_nomat[++ q_cnt] = hdb[q][j].pid;
				++ j;
			}
		}
		delete[] p_uni_took; delete[] q_uni_took;
	}
	
	/* count filtering */
	int overlap = 0;
	for (k = 0; k != pos; ++ k) 
		overlap += m_cnt[k] > n_cnt[k]? n_cnt[k]: m_cnt[k];
	if (overlap < count_lower_bound(gdb[p].path_num, tau_card[p][tau]) || overlap < count_lower_bound(gdb[q].path_num, tau_card[q][tau])) {
		CLEAR_MN
		return NULL;
	}
	
	if (i == m) {
		while (j != n) {
			q_nomat[++ q_cnt] = hdb[q][j].pid;
			++ j;
		}
	} else {
		while (i != m) {
			p_nomat[++ p_cnt] = hdb[p][i].pid;
			++ i;
		}
	}

	/* min-edit filtering */
	vector<vector<unsigned> > *p_compo = NULL, *q_compo = NULL;
	pair<unsigned, int> *p_err = NULL, *q_err = NULL;
	int p_total = 0, q_total = 0;
	if (++ p_cnt != 0) {
		unsigned** p_id;
		unsigned* p_size;
		p_compo = new vector<vector<unsigned> >;
		i = componentize(p, p_nomat, p_cnt, p_compo, p_id, p_size);
		if (i > tau) {
			CLEAR_MN
			CLEAR_P
		}

		p_err = new pair<unsigned, int>[i];
		for (k = 0; k != i; ++ k) {
			if (p_total == tau) {			
				CLEAR_MN
				CLEAR_P
			}
			p_err[k].first = k;
			p_err[k].second = mismat_cot_err(p, q, (*p_compo)[k], p_id[k], p_size[k], tau-p_total);
			p_total += p_err[k].second;
			if (p_total > tau) {			
				CLEAR_MN
				CLEAR_P
			}
		}
		for (k = 0; k != i; ++ k) 
			delete[] p_id[k];
		delete[] p_id;
		delete[] p_size;
	}	
	
	if (++ q_cnt != 0) {
		unsigned** q_id;
		unsigned* q_size;
		q_compo = new vector<vector<unsigned> >;
		j = componentize(q, q_nomat, q_cnt, q_compo, q_id, q_size);
		if (j > tau) {
			CLEAR_MN
			CLEAR_Q
		}

		q_err = new pair<unsigned, int>[j];
		for (k = 0; k != j; ++ k) {
			if (q_total == tau) {
				CLEAR_MN
				CLEAR_Q
			}
			q_err[k].first = k;
			q_err[k].second = mismat_cot_err(q, p, (*q_compo)[k], q_id[k], q_size[k], tau-q_total);
			q_total += q_err[k].second;
			if (q_total > tau) {
				CLEAR_MN
				CLEAR_Q
			}
		}
		for (k = 0; k != j; ++ k) {
			delete[] q_id[k];
		}
		delete[] q_id; delete[] q_size;
	}

	CLEAR_MN
	if (p_err > q_err) {
		Priority* pp = new Priority(p, q);	
		return pp;
	} else {
		Priority* qq = new Priority(q, p);
		return qq;
	}
}

/* content filter includes:
   1) count filter; 2) min-edit filter */
Priority* content_filtering( const unsigned p, const unsigned q )
{
	unsigned i = 0, j = 0, k;
	const unsigned m = gdb[p].path_num, n = gdb[q].path_num;
	unsigned *p_nomat = new unsigned[m], p_cnt = -1;
	unsigned *q_nomat = new unsigned[n], q_cnt = -1;	
	unsigned m_beg, *m_cnt = new unsigned[m];
	unsigned *n_cnt = new unsigned[n];
	unsigned pos = 0;

	/* count mismatching q-grams */
	if (!deg_match_cond) {
		while (i != m && j != n) {
			if (hdb[p][i].hash_val == hdb[q][j].hash_val) {
				m_cnt[pos] = n_cnt[pos] = 0;
				while (true) {
					++ m_cnt[pos];
					if (++ i == m || hdb[p][i].hash_val != hdb[p][i-1].hash_val) break;
				}
				while (true) {
					++ n_cnt[pos];
					if (++ j == n || hdb[q][j].hash_val != hdb[q][j-1].hash_val) break;
				}
				++ pos;
			} else if (hdb[p][i].hash_val < hdb[q][j].hash_val) {
				p_nomat[++ p_cnt] = hdb[p][i].pid;
				++ i;
			} else {
				q_nomat[++ q_cnt] = hdb[q][j].pid;
				++ j;
			}		
		}
	} else {
		bool *p_uni_took = new bool[m], *q_uni_took = new bool[n];
		memset(p_uni_took, 0, sizeof(bool)*m);
		memset(q_uni_took, 0, sizeof(bool)*n);
		while (i != m && j != n) {
			if (hdb[p][i].hash_val == hdb[q][j].hash_val) {	
				m_beg = i;
				m_cnt[pos] = n_cnt[pos] = 0;
				while (true) {
					if (!deg_path_match(p, hdb[p][i].pid, q, j, n, q_uni_took, m_cnt[pos])) 
						p_nomat[++ p_cnt] = hdb[p][i].pid;
					if (++ i == m || hdb[p][i].hash_val != hdb[p][i-1].hash_val) break;
				}
				while (true) {
					if (!deg_path_match(q, hdb[q][j].pid, p, m_beg, i, p_uni_took, n_cnt[pos])) 
						q_nomat[++ q_cnt] = hdb[q][j].pid;
					if (++ j == n || hdb[q][j].hash_val != hdb[q][j-1].hash_val) break;
				}
				++ pos;
			} else if (hdb[p][i].hash_val < hdb[q][j].hash_val) {				
				p_nomat[++ p_cnt] = hdb[p][i].pid;
				++ i;
			} else {
				q_nomat[++ q_cnt] = hdb[q][j].pid;
				++ j;
			}
		}
		delete[] p_uni_took; delete[] q_uni_took;
	}

	/* count filtering */
	int overlap = 0;
	for (k = 0; k != pos; ++ k)
		overlap += m_cnt[k] > n_cnt[k]? n_cnt[k]: m_cnt[k];
	if (overlap < count_lower_bound(gdb[p].path_num, tau_card[p][tau]) || overlap < count_lower_bound(gdb[q].path_num, tau_card[q][tau])) {
		CLEAR_MN
		return NULL;
	}
	
	if (i == m) {
		while (j != n) {
			q_nomat[++ q_cnt] = hdb[q][j].pid;
			++ j;
		}
	} else {
		while (i != m) {
			p_nomat[++ p_cnt] = hdb[p][i].pid;
			++ i;
		}
	}

	/* min-edit filtering */
	vector<vector<unsigned> > *p_compo = NULL, *q_compo = NULL;
	pair<unsigned, int> *p_err = NULL, *q_err = NULL;
	int p_total = 0, q_total = 0;
	if (++ p_cnt != 0) {
		unsigned** p_id;
		unsigned* p_size;
		p_compo = new vector<vector<unsigned> >;
		i = componentize(p, p_nomat, p_cnt, p_compo, p_id, p_size);
		if (i > tau) {
			CLEAR_MN
			CLEAR_P
		}

		p_err = new pair<unsigned, int>[i];
		for (k = 0; k != i; ++ k) {
			if (p_total == tau) {			
				CLEAR_MN
				CLEAR_P
			}
			p_err[k].first = k;
			p_err[k].second = mismat_cot_err(p, q, (*p_compo)[k], p_id[k], p_size[k], tau-p_total);
			p_total += p_err[k].second;
			if (p_total > tau) {			
				CLEAR_MN
				CLEAR_P
			}
		}
		for (k = 0; k != i; ++ k) 
			delete[] p_id[k];
		delete[] p_id;
		delete[] p_size;
	} else i = 0;
	
	if (++ q_cnt != 0) {
		unsigned** q_id;
		unsigned* q_size;
		q_compo = new vector<vector<unsigned> >;
		j = componentize(q, q_nomat, q_cnt, q_compo, q_id, q_size);
		if (j > tau) {
			CLEAR_MN
			CLEAR_Q
		}

		q_err = new pair<unsigned, int>[j];
		for (k = 0; k != j; ++ k) {
			if (q_total == tau) {
				CLEAR_MN
				CLEAR_Q
			}
			q_err[k].first = k;
			q_err[k].second = mismat_cot_err(q, p, (*q_compo)[k], q_id[k], q_size[k], tau-q_total);
			q_total += q_err[k].second;
			if (q_total > tau) {
				CLEAR_MN
				CLEAR_Q
			}
		}
		for (k = 0; k != j; ++ k) {
			delete[] q_id[k];
		}
		delete[] q_id; delete[] q_size;
	} else j = 0;

	if (p_total > max_err) max_err = p_total;	
	if (q_total > max_err) max_err = q_total;
	CLEAR_MN
#ifdef MORDER
	delete[] p_err;
	delete p_compo;
	if (q_err != NULL) 
		sort(q_err, q_err+j, Pair_comp());
	Priority* qq = new Priority(q, p, q_compo, q_err, j);
	return qq;
#else
	if (p_total > q_total) {
		delete[] q_err;
		delete q_compo;
		if (p_err != NULL) 
			sort(p_err, p_err+i, Pair_comp());
		Priority* pp = new Priority(p, q, p_compo, p_err, i);						
		return pp;
	} else {
		delete[] p_err;
		delete p_compo;
		if (q_err != NULL) 
			sort(q_err, q_err+j, Pair_comp());
		Priority* qq = new Priority(q, p, q_compo, q_err, j);
		return qq;
	}
#endif	
}

unsigned componentize( const unsigned g, unsigned* nomat, unsigned cnt, vector<vector<unsigned> >* component, unsigned**& npid, unsigned*& nsize )
{
	Disjoin_set disjoin_set(gdb[g].vertex_num);	// size of father_table equal to vertex number, though not all slots are used
	vector<unsigned> vertex;
	unsigned size, i = 0;
	for (; i != cnt; ++ i) {
		for (int j = 0; j != qs; ++ j) {
			size = in_list(vertex, pdb[g][nomat[i]]->vpath[j]);
			if (size == vertex.size()) {
				vertex.push_back(pdb[g][nomat[i]]->vpath[j]);
				disjoin_set.father_table[pdb[g][nomat[i]]->vpath[j]] = pdb[g][nomat[i]]->vpath[j];	// initialize each vertex into a disjoint set
			}
		}
	}

	size = vertex.size();
	vector<unsigned>* ids = new vector<unsigned>[size];
	unsigned pos;
	for (i = 0; i != cnt; ++ i) {
		pos = in_list(vertex, pdb[g][nomat[i]]->vpath[0]);
		ids[pos].push_back(nomat[i]);
		for (int j = 1; j != qs; ++ j) {
			pos = in_list(vertex, pdb[g][nomat[i]]->vpath[j]);
			ids[pos].push_back(nomat[i]);
			disjoin_set.union_set(pdb[g][nomat[i]]->vpath[j], pdb[g][nomat[i]]->vpath[0]);
		}
	}

	bool* used = new bool[size];
	memset(used, 0, sizeof(bool)*size);
	i = 0;
	while (i != size) {
		used[i] = true;
		component->resize(component->size()+1);
		component->back().push_back(vertex[i]);
		for (pos = i+1; pos != size; ++ pos) {
			if (used[pos]) continue;
			if (disjoin_set.same_set(vertex[i], vertex[pos])) {
				component->back().push_back(vertex[pos]);
				used[pos] = true;
			}
		}

		if (memcmp(used, done, sizeof(bool)*size) == 0) break;

		while (i != size && used[i]) {
			++ i;	// look for next set
		}
	}

	size = component->size();
	npid = new unsigned*[size];
	nsize = new unsigned[size];
	memset(nsize, 0, sizeof(unsigned)*size);
	if (size == 1) {
		npid[0] = new unsigned[cnt];
		memcpy(npid[0], nomat, sizeof(unsigned)*cnt);
		sort(npid[0], npid[0]+cnt);
		nsize[0] = cnt;
	} else {
		for (unsigned p = 0; p != size; ++ p) {
			npid[p] = new unsigned[cnt];
			for (unsigned q = 0, s = (*component)[p].size(); q != s; ++ q) {
				pos = in_list(vertex, (*component)[p][q]);
				for (unsigned m = 0, n = ids[pos].size(); m != n; ++ m) {
					if (nsize[p] == in_list(npid[p], nsize[p], ids[pos][m])) {
						npid[p][nsize[p]] = ids[pos][m];
						++ nsize[p];
					}
				}
			}
			sort(npid[p], npid[p]+nsize[p]);
		}
	}

	delete[] ids;
	delete[] used;
	return size;
}

bool deg_path_match( const unsigned p, const unsigned pid, const unsigned q, unsigned j, const unsigned size, bool* q_uni_took, unsigned& count )
{
	int max_p, max_q, sum_p, sum_q, err, p_elab, q_elab;
	Graph &pp = gdb[p], &qq = gdb[q];
	Path_gram *cur, *ref = pdb[p][pid];
	int* diff = new int[qs];
	unsigned pos_mark = -1;
	bool round;
	while (true) {
		cur = pdb[q][hdb[q][j].pid];
		if (ref->backward == cur->backward) {	// both forward or backward
			max_p = max_q = sum_p = sum_q = 0;
			round = true;
			for (int s = 0; s != qs; ++ s) {
				diff[s] = pp[ref->vpath[s]].degree - qq[cur->vpath[s]].degree;
				if (diff[s] > 0) {
					if (max_p < diff[s]) {
						max_p = diff[s];
						if (max_p + max_q > tau) {
							round = false; break;
						}
					}
					sum_p += diff[s];
				}
				if (diff[s] < 0) {
					if (max_q < -diff[s]) {
						max_q = -diff[s];
						if (max_p + max_q > tau) {
							round = false; break;
						}
					}
					sum_q += -diff[s];
				}
			}
			if (round && sum_p + sum_q <= (tau << 1)) {
				err = 0;
				for (int s = 0; s < qs; ++ s) {
					for (int t = s+2; t < qs; ++ t) {
						if (p_elab = pp.get_elab(ref->vpath[s], ref->vpath[t]), p_elab != DUMMY_LABEL) {
							if (q_elab = qq.get_elab(cur->vpath[s], cur->vpath[t]), q_elab != DUMMY_LABEL) {
								if (p_elab != q_elab) ++ err;
							} else {	
								++ err;
								-- diff[s], -- diff[t];
							}
						} else {	
							if (qq.edge_feasible(cur->vpath[s], cur->vpath[t])) {
								++ err;
								++ diff[s], ++ diff[t];
							}
						}
					}
				}
				for (int s = 0; s != qs; ++ s) {
					err += diff[s] >= 0? diff[s]: -diff[s]; 
				}
				IF_MATCH
			}
			if (ref->normal || cur->normal) {
				max_p = max_q = sum_p = sum_q = 0;
				round = true;
				for (int s = 0; s != qs; ++ s) {
					diff[s] = pp[ref->vpath[s]].degree - qq[cur->vpath[under_qs-s]].degree;
					if (diff[s] > 0) {
						if (max_p < diff[s]) {
							max_p = diff[s];
							if (max_p + max_q > tau) {
								round = false; break;
							}
						}
						sum_p += diff[s];
					}
					if (diff[s] < 0) {
						if (max_q < -diff[s]) {
							max_q = -diff[s];
							if (max_p + max_q > tau) {
								round = false; break;
							}
						}
						sum_q += -diff[s];
					}
				}
				if (round && sum_p + sum_q <= (tau << 1)) {
					err = 0;
					for (int s = 0; s < qs; ++ s) {
						for (int t = s+2; t < qs; ++ t) {
							if (p_elab = pp.get_elab(ref->vpath[s], ref->vpath[t]), p_elab != DUMMY_LABEL) {
								if (q_elab = qq.get_elab(cur->vpath[under_qs-s], cur->vpath[under_qs-t]), q_elab != DUMMY_LABEL) {
									if (p_elab != q_elab) ++ err;
								} else {
									++ err;
									-- diff[s], -- diff[t];
								}
							} else {	
								if (qq.edge_feasible(cur->vpath[under_qs-s], cur->vpath[under_qs-t])) {
									++ err;
									++ diff[s], ++ diff[t];
								}
							}
						}
					}
					for (int s = 0; s != qs; ++ s) {
						err += diff[s] >= 0? diff[s]: -diff[s]; 
					}
					IF_MATCH	
				}
			}
		} else {	// only one is backward
			max_p = max_q = sum_p = sum_q = 0;
			round = true;
			for (int s = 0; s != qs; ++ s) {
				diff[s] = pp[ref->vpath[s]].degree - qq[cur->vpath[under_qs-s]].degree;
				if (diff[s] > 0) {
					if (max_p < diff[s]) {
						max_p = diff[s];
						if (max_p + max_q > tau) {
							round = false; break;
						}
					}
					sum_p += diff[s];
				}
				if (diff[s] < 0) {
					if (max_q < -diff[s]) {
						max_q = -diff[s];
						if (max_p + max_q > tau) {
							round = false; break;
						}
					}
					sum_q += -diff[s];
				}
			}
			if (round && sum_p + sum_q <= (tau << 1)) {
				err = 0;
				for (int s = 0; s < qs; ++ s) {
					for (int t = s+2; t < qs; ++ t) {
						if (p_elab = pp.get_elab(ref->vpath[s], ref->vpath[t]), p_elab != DUMMY_LABEL) {
							if (q_elab = qq.get_elab(cur->vpath[under_qs-s], cur->vpath[under_qs-t]), q_elab != DUMMY_LABEL) {
								if (p_elab != q_elab) ++ err;
							} else {
								++ err;
								-- diff[s], -- diff[t];
							}
						} else {
							if (qq.edge_feasible(cur->vpath[under_qs-s], cur->vpath[under_qs-t])) {
								++ err;
								++ diff[s], ++ diff[t];
							}
						}
					}
				}
				for (int s = 0; s != qs; ++ s) {
					err += diff[s] >= 0? diff[s]: -diff[s]; 
				}		
				IF_MATCH
			}
			if (ref->normal || cur->normal) {
				max_p = max_q = sum_p = sum_q = 0;
				round = true;
				for (int s = 0; s != qs; ++ s) {
					diff[s] =  pp[ref->vpath[s]].degree - qq[cur->vpath[s]].degree; 
					if (diff[s] > 0) {
						if (max_p < diff[s]) {
							max_p = diff[s];
							if (max_p + max_q > tau) {
								round = false; break;
							}
							sum_p += diff[s];
						}
						if (diff[s] < 0) {
							if (max_q < -diff[s]) {
								max_q = -diff[s];
								if (max_p + max_q > tau) {
									round = false; break;
								}
							}
							sum_q += -diff[s];
						}
					}
				} 
				if (round && sum_p + sum_q <= (tau << 1)) {
					err = 0;
					for (int s = 0; s < qs; ++ s) {
						for (int t = s+2; t < qs; ++ t) {
							if (p_elab = pp.get_elab(ref->vpath[s], ref->vpath[t]), p_elab != DUMMY_LABEL) {
								if (q_elab = qq.get_elab(cur->vpath[s], cur->vpath[t]), q_elab != DUMMY_LABEL) {
									if (p_elab != q_elab) ++ err;
								} else {
									++ err;
									-- diff[s], -- diff[t];
								}
							} else {
								if (qq.edge_feasible(cur->vpath[s], cur->vpath[t])) {
									++ err;
									++ diff[s], ++ diff[t];
								}
							}
						}					
					}
					for (int s = 0; s != qs; ++ s) {
						err += diff[s] >= 0? diff[s]: -diff[s];
					}			
					IF_MATCH
				}
			}
		}

		if (++ j == size || hdb[q][j].hash_val != hdb[q][j-1].hash_val) 
			break;
	}

	delete[] diff;
	if (pos_mark == -1)
		return false;
	else {
		if (!q_uni_took[pos_mark]) {
			++ count;
			q_uni_took[pos_mark] = true;
		}
		return true;
	}
}