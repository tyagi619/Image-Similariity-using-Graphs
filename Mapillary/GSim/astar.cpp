#include "astar.h"
#include "path_join.h"
#include "set_cover.h"

int nodes_expanded, nodes_popped;


/* operators for label_pair */
bool operator < ( const vertex_label_pair& x, const vertex_label_pair& y )
{
	return (x.vlabel < y.vlabel || x.vlabel == y.vlabel && x.vid < y.vid);
}

bool operator < ( const edge_label_pair& x, const edge_label_pair& y)
{
	return (x.elabel < y.elabel || x.elabel == y.elabel && x.vid < y.vid || x.elabel == y.elabel && x.vid == y.vid && x.eid < y.eid);
}

/* operators for GED_key */
bool operator < ( const GED_key& x, const GED_key& y )
{
	int i;
	if (x.hash_key < y.hash_key) return 1;
	else if (x.hash_key > y.hash_key) return 0;
	for (i = 0; i < x.vertex_mapping.size(); ++ i)
		if (x.vertex_mapping[i] < y.vertex_mapping[i])
			return 1;
		else if (x.vertex_mapping[i] > y.vertex_mapping[i])
			return 0;
	return 0;
}

bool operator > ( const GED_key& x, const GED_key& y )
{
	int i;
	if (x.hash_key > y.hash_key) return 1;
	else if (x.hash_key < y.hash_key) return 0;
	for (i = 0; i < x.vertex_mapping.size(); ++ i)
		if (x.vertex_mapping[i] > y.vertex_mapping[i])
			return 1;
		else if (x.vertex_mapping[i] < y.vertex_mapping[i])
			return 0;
	return 0;
}

bool operator == ( const GED_key& x, const GED_key& y )
{
	int i;
	if (x.hash_key != y.hash_key) return 0;
	for (i = 0; i < x.vertex_mapping.size(); ++ i)
		if (x.vertex_mapping[i] != y.vertex_mapping[i])
			return 0;
	return 1;
}

bool operator != ( const GED_key& x, const GED_key& y )
{
	return !(x == y);
}

/* operators for GED_node */
bool operator < ( const GED_node& x, const GED_node& y )
{
	return (x.f_value < y.f_value);
}

bool operator > ( const GED_node& x, const GED_node& y )
{
	return (x.f_value > y.f_value);
}

bool operator == ( const GED_node& x, const GED_node& y )
{
	return (x.f_value == y.f_value);
}

bool operator != ( const GED_node& x, const GED_node& y )
{
	return (x.f_value != y.f_value);
}

GED_key generate_key( vector<int> &vertex_mapping )
{
	int i; 
	GED_key key;
	for (i = 0, key.hash_key = KEY_INITIAL; i < vertex_mapping.size(); ++ i)
		key.hash_key = key.hash_key * PRIME_FACTOR + vertex_mapping[i];
	key.vertex_mapping = vertex_mapping;
	return key;
}

bool is_adjacent( Graph &g, const int uid, const int vid )
{
	for (unsigned i = 0; i < g[uid].degree; ++ i)
		if (g[uid].edges[i].to == vid) return 1;
	return 0;
}

bool is_adjacent_with_label( Graph &g, const int uid, const int vid, const int label )
{
	for (unsigned i = 0; i < g[uid].degree; ++ i)
		if (g[uid].edges[i].to == vid && g[uid].edges[i].elabel == label) return 1;
	return 0;
}

int estimate_vertex( Graph &g1, Graph &g2, vector<int> &vertex_mapping_g1, vector<int> &vertex_mapping_g2 )
{
	int i, j, sum_count_g1, sum_count_g2;
	vector<int> label_g1;
	vector<int> label_g2;
	for (i = 0; i < g1.vertex_num; ++ i)
		if (vertex_mapping_g1[i] == UNMAPPED)
			label_g1.push_back(g1[i].vlabel);
	for (i = 0; i < g2.vertex_num; ++ i)
		if (vertex_mapping_g2[i] == UNMAPPED)
			label_g2.push_back(g2[i].vlabel);
	sort(label_g1.begin(), label_g1.end());
	sort(label_g2.begin(), label_g2.end());
	i = j = 0;
	sum_count_g1 = sum_count_g2 = 0;
	while (i < label_g1.size() && j < label_g2.size()) {
		if (label_g1[i] == label_g2[j]) ++ i, ++ j;
		else if (label_g1[i] < label_g2[j]) ++ i, ++ sum_count_g1;
		else ++ j, ++ sum_count_g2;
	}
	if (i < label_g1.size()) sum_count_g1 += label_g1.size() - i;
	else sum_count_g2 += label_g2.size() - j;
	return sum_count_g1 >= sum_count_g2 ? sum_count_g1 : sum_count_g2;
}

int compute_label_difference( vector<int> &label_list_g1, vector<int> &label_list_g2 )
{
	int i = 0, j = 0, sum_count_g1, sum_count_g2;
	//i = j = 0;
	sum_count_g1 = sum_count_g2 = 0;
	while (i < label_list_g1.size() && j < label_list_g2.size()) {
		if (label_list_g1[i] == label_list_g2[j]) ++ i, ++ j;
		else if (label_list_g1[i] < label_list_g2[j]) {
			if (label_list_g1[i] >= 0) ++ sum_count_g1;
			++ i;
		}
		else {
			if (label_list_g2[j] >= 0) ++ sum_count_g2;
			++ j;
		}
	}
	while (i < label_list_g1.size()) {
		if (label_list_g1[i] >= 0) ++ sum_count_g1;
		++ i;	
	}
	while (j < label_list_g2.size()) {
		if (label_list_g2[j] >= 0) ++ sum_count_g2;
		++ j;	
	}
	return sum_count_g1 >= sum_count_g2 ? sum_count_g1 : sum_count_g2;
}

void process_label_list( Graph &g, vector<int> &vlabel_list, vector<int> &elabel_list, vector<int> &vid_to_vlabel_list, vector< vector<int> > &eid_to_elabel_list )
{
	int i, j;
	vertex_label_pair vl_pair;
	vector<vertex_label_pair> vl_pair_list;
	edge_label_pair el_pair;
	vector<edge_label_pair> el_pair_list;
	vector<int> temp;

	/* vertex */
	for (i = 0; i < g.vertex_num; ++ i) {
		vl_pair.vid = i, vl_pair.vlabel = g[i].vlabel;
		vl_pair_list.push_back(vl_pair);
	}
	sort(vl_pair_list.begin(), vl_pair_list.end());
	vid_to_vlabel_list.resize(g.vertex_num);
	for (i = 0; i < vl_pair_list.size(); ++ i) {
		vlabel_list.push_back(vl_pair_list[i].vlabel);
		vid_to_vlabel_list[vl_pair_list[i].vid] = i;
	}

	/* edge */
	for (i = 0; i < g.vertex_num; ++ i) {
		el_pair.vid = i;
		eid_to_elabel_list.push_back(temp);
		eid_to_elabel_list[i].resize(g[i].degree);
		for (j = 0; j < g[i].degree; ++ j) {
			if (g[i].edges[j].to <= i) continue;
			el_pair.eid = j;
			el_pair.elabel = g[i].edges[j].elabel;
			el_pair_list.push_back(el_pair);
		}
	}
	sort(el_pair_list.begin(), el_pair_list.end());
	for (i = 0; i < el_pair_list.size(); ++ i) {
		elabel_list.push_back(el_pair_list[i].elabel);
		eid_to_elabel_list[el_pair_list[i].vid][el_pair_list[i].eid] = i;
		for (j = 0; j < g[g[el_pair_list[i].vid].edges[el_pair_list[i].eid].to].degree; j++) {
			if (g[g[el_pair_list[i].vid].edges[el_pair_list[i].eid].to].edges[j].to == el_pair_list[i].vid)
				eid_to_elabel_list[g[el_pair_list[i].vid].edges[el_pair_list[i].eid].to][j] = i;
		}
	}
}

void copy_label_list( Graph &g, vector<int> &vlabel_list, vector<int> &elabel_list, vector<int> &vid_to_vlabel_list, vector< vector<int> > &eid_to_elabel_list )
{
	vlabel_list.resize(g.vertex_num);
	vid_to_vlabel_list.resize(g.vertex_num);
	eid_to_elabel_list.resize(g.vertex_num);
	elabel_list.resize(g.edge_num);
	for (unsigned i = 0; i != g.vertex_num; ++ i) {
		vlabel_list[i] = g.vlabel_list[i];
		vid_to_vlabel_list[i] = g.vid_to_vlabel[i];
		eid_to_elabel_list[i] = g.eid_to_elabel[i];
	}
	for (unsigned i = 0; i != g.edge_num; ++ i) {
		elabel_list[i] = g.elabel_list[i];
	}
}

int compute_mismatch_error( GED_node& next_node, const unsigned lgid, const unsigned rgid, const int threshold )
{
	int m, n;
	bool found;
	Graph &lg = gdb[lgid], &rg = gdb[rgid];
	unsigned i = rg.path_num, j = lg.path_num;
	unsigned *pid = new unsigned[j], p_size = -1;
	unsigned *qid = new unsigned[i], q_size = -1;
	vector<Path_gram*> &lp = pdb[lgid], &rp = pdb[rgid];
	for (i = 0; i != j; ++ i) {
		found = false;
		for (m = 0; m != qs; ++ m) {
			if (next_node.vertex_mapping_g1[lp[i]->vpath[m]] != UNMAPPED) {
				found = true;
				break;
			}
		}
		if (!found) {
			pid[++ p_size] = i;	
		}
	}
	for (j = 0, i = rg.path_num; j != i; ++ j) {
		found = false;
		for (n = 0; n != qs; ++ n) {
			if (next_node.vertex_mapping_g2[rp[j]->vpath[n]] != UNMAPPED) {
				found = true;
				break;
			}
		}
		if (!found) {
			qid[++ q_size] = j;
		}
	}

	unsigned *p_nomat = new unsigned[++ p_size], p_cnt = -1;	
	unsigned *q_nomat = new unsigned[++ q_size], q_cnt = -1;
	unsigned m_beg, *m_cnt = new unsigned[p_size];
	unsigned *n_cnt = new unsigned[q_size];
	i = j = 0;
	unsigned pos = 0;
	while (i != p_size && j != q_size) {
		if (!fcomp(lp[pid[i]], rp[qid[j]])) {
			if (!fcomp(rp[qid[j]], lp[pid[i]])) {
				m_beg = i;
				m_cnt[pos] = n_cnt[pos] = 0;
				while (true) {
					if (!mis_deg_combine_match(lgid, pid[i], rgid, qid, j, q_size, threshold, m_cnt[pos])) {
						if (threshold == 0) {
							delete[] pid; delete[] qid; delete[] p_nomat; delete[] q_nomat;	delete[] m_cnt;	delete[] n_cnt; 
							return 1;
						}
						p_nomat[++ p_cnt] = pid[i];
					}
					if (++ i == p_size || lp[pid[i]]->hash_val != lp[pid[i-1]]->hash_val) break;
				}
				while (true) {
					if (!mis_deg_combine_match(rgid, qid[j], lgid, pid, m_beg, i, threshold, n_cnt[pos])) {
						if (threshold == 0) {
							delete[] pid; delete[] qid; delete[] p_nomat; delete[] q_nomat; delete[] m_cnt;	delete[] n_cnt; 
							return 1;
						}
						q_nomat[++ q_cnt] = qid[j];
					}
					if (++ j == q_size || rp[qid[j]]->hash_val != rp[qid[j-1]]->hash_val) break;
				}
				++ pos;
			} else {
				if (threshold == 0) {
					delete[] pid; delete[] qid; delete[] p_nomat; delete[] q_nomat;	delete[] m_cnt;	delete[] n_cnt; 
					return 1;
				}
				q_nomat[++ q_cnt] = qid[j];
				++ j;
			}
		} else {
			if (threshold == 0) {
				delete[] pid; delete[] qid; delete[] p_nomat; delete[] q_nomat;	delete[] m_cnt;	delete[] n_cnt; 
				return 1;
			}
			p_nomat[++ p_cnt] = pid[i];
			++ i;
		}
	}

	/* count filter */
	int overlap = 0;
	for (unsigned k = 0; k != pos; ++ k) {
		overlap += m_cnt[k] > n_cnt[k]? n_cnt[k]: m_cnt[k];
	}
	if (overlap < count_lower_bound(p_size, tau_card[lgid][threshold]) || overlap < count_lower_bound(q_size, tau_card[rgid][threshold])) {
		delete[] pid; delete[] qid; delete[] p_nomat; delete[] q_nomat;
		delete[] m_cnt;	delete[] n_cnt;
		return (threshold + 1);
	}

	/* min-edit filter */
	if (i == p_size) {
		while (j != q_size) {
			q_nomat[++ q_cnt] = qid[j];
			++ j;
		}
	} else {
		while (i != p_size) {
			p_nomat[++ p_cnt] = pid[i];
			++ i;
		}
	}
	++ p_cnt, ++ q_cnt;
	m = n = 0;

	if (p_cnt != 0) {
		set<unsigned> p_compo;
		for (i = 0; i != p_cnt; ++ i) {
			for (m = 0; m != qs; ++ m) {
				p_compo.insert(lp[p_nomat[i]]->vpath[m]);
			}
		}
		m = mismat_cot_err(lgid, rgid, p_compo, p_nomat, p_cnt, threshold);
		if (m > threshold) {
			delete[] pid; delete[] qid; delete[] p_nomat; delete[] q_nomat;
			delete[] m_cnt;	delete[] n_cnt; 
			return m;
		}
	}
	if (q_cnt != 0) {
		set<unsigned> q_compo;
		for (j = 0; j != q_cnt; ++ j) {
			for (n = 0; n != qs; ++ n) {
				q_compo.insert(rp[q_nomat[j]]->vpath[n]);
			}
		}
		n = mismat_cot_err(rgid, lgid, q_compo, q_nomat, q_cnt, threshold);
		if (n > threshold) {
			delete[] pid; delete[] qid; delete[] p_nomat; delete[] q_nomat;
			delete[] m_cnt;	delete[] n_cnt; 
			return n;
		}
	}

	delete[] pid; delete[] qid; delete[] p_nomat; delete[] q_nomat;
	delete[] m_cnt;	delete[] n_cnt; 
	return (m > n? m: n);
}

int rudim_edit_distance( Priority* pri )
{
	int i, j;
	GED_key key;
	vector<int> vlabel_list_g1;
	vector<int> vlabel_list_g2;
	vector<int> elabel_list_g1;
	vector<int> elabel_list_g2;
	vector<int> vid_to_vlabel_list_g1;
	vector<int> vid_to_vlabel_list_g2;
	vector< vector<int> > eid_to_elabel_list_g1;
	vector< vector<int> > eid_to_elabel_list_g2;

	/* compute label list */
	copy_label_list(*pri->lg, vlabel_list_g1, elabel_list_g1, vid_to_vlabel_list_g1, eid_to_elabel_list_g1);
	copy_label_list(*pri->rg, vlabel_list_g2, elabel_list_g2, vid_to_vlabel_list_g2, eid_to_elabel_list_g2);

	/* start node */
	GED_node start_node(0, 0, 0, 0, pri->lg->edge_num, pri->rg->edge_num);
	start_node.vertex_mapping_g1.resize(pri->lg->vertex_num);
	start_node.vertex_mapping_g2.resize(pri->rg->vertex_num + tau);
	start_node.vlabel_list_g1 = vlabel_list_g1;
	start_node.vlabel_list_g2 = vlabel_list_g2;
	start_node.elabel_list_g1 = elabel_list_g1;
	start_node.elabel_list_g2 = elabel_list_g2;
	for (i = 0; i < pri->lg->vertex_num; ++ i)
		start_node.vertex_mapping_g1[i] = UNMAPPED;
	for (i = 0; i < pri->rg->vertex_num + tau; ++ i)
		start_node.vertex_mapping_g2[i] = UNMAPPED;
	key = generate_key(start_node.vertex_mapping_g1);

	/* insert start node */
	MinHeap<GED_node, GED_key> nodes;
	nodes.Insert(key, start_node);

	/* search for best mapping */
	GED_node current_node, next_node;
	bool dummy_flag;
	int g_value_new, h_value_new;
	int next_vertex_id_g1;
	int next_vertex_id_g2;
	int next_vertex_deg_g1;
	int next_vertex_deg_g2;
	int adjacent_vertex_g1_id;
	int adjacent_vertex_g2_id;
	int inner_edge_num_g1;
	int inner_edge_num_g2;

	nodes_expanded = nodes_popped = 0;
	while (!nodes.Empty()) {
		nodes.ExtractMin(key, current_node);
		if (current_node.mapped_vertex_num_g1 == pri->lg->vertex_num) {
			return current_node.f_value;
		}

		/* update neighbor nodes */
		dummy_flag = 0;
		next_vertex_id_g1 = pri->lv_sort[current_node.mapped_vertex_num_g1].id;
		next_vertex_deg_g1 = pri->lv_sort[current_node.mapped_vertex_num_g1].deg;
		for (i = 0; i < pri->lv_sort[current_node.mapped_vertex_num_g1].cand_num; ++ i) {
			if (dummy_flag) break;
			next_vertex_id_g2 = pri->lv_cand[next_vertex_id_g1][i];
			if (current_node.vertex_mapping_g2[next_vertex_id_g2] != UNMAPPED) continue; 

			if (next_vertex_id_g2 >= pri->rg->vertex_num) dummy_flag = 1;
			g_value_new = current_node.g_value + ((*pri->lg)[next_vertex_id_g1].vlabel == (*pri->rg)[next_vertex_id_g2].vlabel ? 0 : 1);
			next_vertex_deg_g2 = (*pri->rg)[next_vertex_id_g2].degree;
			if (g_value_new + abs(next_vertex_deg_g1 - next_vertex_deg_g2) > tau) continue; // g(x) + delta(degree) > tau
			++ nodes_expanded;
			/* compute g(x) */
			/* compare new induced graph */
			for (j = 0, inner_edge_num_g1 = 0; j < next_vertex_deg_g1; ++ j) {
				adjacent_vertex_g1_id = (*pri->lg)[next_vertex_id_g1].edges[j].to;
				if (current_node.vertex_mapping_g1[adjacent_vertex_g1_id] != UNMAPPED) {
					++ inner_edge_num_g1;
				   	if (!is_adjacent_with_label(*pri->rg, next_vertex_id_g2, current_node.vertex_mapping_g1[adjacent_vertex_g1_id], (*pri->lg)[next_vertex_id_g1].edges[j].elabel)) {
						++ g_value_new;
						if (g_value_new > tau) break;
					}
				}
			}
			if (g_value_new > tau) continue;
			for (j = 0, inner_edge_num_g2 = 0; j < next_vertex_deg_g2; ++ j) {
				adjacent_vertex_g2_id = (*pri->rg)[next_vertex_id_g2].edges[j].to;
				if (current_node.vertex_mapping_g2[adjacent_vertex_g2_id] != UNMAPPED) {
					++ inner_edge_num_g2;
					if (!is_adjacent(*pri->lg, next_vertex_id_g1, current_node.vertex_mapping_g2[adjacent_vertex_g2_id])) {
						++ g_value_new;
						if (g_value_new > tau) break;
					}
				}
			}
			if (g_value_new > tau) continue;

			/* compute h(x) */
			next_node.g_value = g_value_new;
			next_node.mapped_vertex_num_g1 = current_node.mapped_vertex_num_g1 + 1;
			next_node.mapped_vertex_num_g2 = current_node.mapped_vertex_num_g2 + ((*pri->rg)[next_vertex_id_g2].vlabel == DUMMY_LABEL ? 0 : 1);
			next_node.remaining_edge_num_g1 = current_node.remaining_edge_num_g1 - inner_edge_num_g1;
			next_node.remaining_edge_num_g2 = current_node.remaining_edge_num_g2 - inner_edge_num_g2;

			/* ||V1|-|V2|| + ||E1|-|E2|| */
			h_value_new = abs((int(pri->lg->vertex_num) - next_node.mapped_vertex_num_g1) - (int(pri->rg->vertex_num) - next_node.mapped_vertex_num_g2)) + abs(next_node.remaining_edge_num_g1 - next_node.remaining_edge_num_g2);
			if (g_value_new + h_value_new > tau) continue;

			/* Delta(V) + Delta(E) */
			next_node.vertex_mapping_g1 = current_node.vertex_mapping_g1; 
			next_node.vertex_mapping_g2 = current_node.vertex_mapping_g2;
			next_node.vertex_mapping_g1[next_vertex_id_g1] = next_vertex_id_g2; 
			next_node.vertex_mapping_g2[next_vertex_id_g2] = next_vertex_id_g1;
			next_node.vlabel_list_g1 = current_node.vlabel_list_g1;
			next_node.vlabel_list_g2 = current_node.vlabel_list_g2;
			next_node.elabel_list_g1 = current_node.elabel_list_g1;
			next_node.elabel_list_g2 = current_node.elabel_list_g2;
			next_node.vlabel_list_g1[vid_to_vlabel_list_g1[next_vertex_id_g1]] = -1;
			for (j = 0; j < next_vertex_deg_g1; j++) {
				adjacent_vertex_g1_id = (*pri->lg)[next_vertex_id_g1].edges[j].to;
				if (next_node.vertex_mapping_g1[adjacent_vertex_g1_id] != UNMAPPED) 
					next_node.elabel_list_g1[eid_to_elabel_list_g1[next_vertex_id_g1][j]] = -1;
			}
			if (next_vertex_id_g2 < pri->rg->vertex_num) {
				next_node.vlabel_list_g2[vid_to_vlabel_list_g2[next_vertex_id_g2]] = -1;
				for (j = 0; j < next_vertex_deg_g2; j++) {
					adjacent_vertex_g2_id = (*pri->rg)[next_vertex_id_g2].edges[j].to;
					if (current_node.vertex_mapping_g2[adjacent_vertex_g2_id] != UNMAPPED) 
						next_node.elabel_list_g2[eid_to_elabel_list_g2[next_vertex_id_g2][j]] = -1;
				}
			}

			h_value_new = compute_label_difference(next_node.vlabel_list_g1, next_node.vlabel_list_g2);
			if (g_value_new + h_value_new > tau) continue;

			h_value_new += compute_label_difference(next_node.elabel_list_g1, next_node.elabel_list_g2);
			if (g_value_new + h_value_new > tau) continue;

			/* update priority queue */
			key = generate_key(next_node.vertex_mapping_g1);
			next_node.f_value = g_value_new + h_value_new;
			if (nodes.Find(key) >= 0) {
				if (next_node < nodes.GetValue(key))
					nodes.UpdateKey(key, next_node);
			} else {
				nodes.Insert(key, next_node);
			}
		}
	}

	return OVER_BOUND;
}

int filter_edit_distance( Priority* pri )
{
	//ImprovedEstimateSwitch = tau - 1;
	int i, j;
	GED_key key;
	vector<int> vlabel_list_g1;
	vector<int> vlabel_list_g2;
	vector<int> elabel_list_g1;
	vector<int> elabel_list_g2;
	vector<int> vid_to_vlabel_list_g1;
	vector<int> vid_to_vlabel_list_g2;
	vector< vector<int> > eid_to_elabel_list_g1;
	vector< vector<int> > eid_to_elabel_list_g2;

	/* compute label list */
	copy_label_list(*pri->lg, vlabel_list_g1, elabel_list_g1, vid_to_vlabel_list_g1, eid_to_elabel_list_g1);
	copy_label_list(*pri->rg, vlabel_list_g2, elabel_list_g2, vid_to_vlabel_list_g2, eid_to_elabel_list_g2);

	/* start node */
	GED_node start_node;
	start_node.f_value = 0;
	start_node.g_value = 0;
	start_node.mapped_vertex_num_g1 = 0;	
	start_node.mapped_vertex_num_g2 = 0;	
	start_node.remaining_edge_num_g1 = pri->lg->edge_num;
	start_node.remaining_edge_num_g2 = pri->rg->edge_num;
	start_node.vertex_mapping_g1.resize(pri->lg->vertex_num);
	start_node.vertex_mapping_g2.resize(pri->rg->vertex_num + tau);
	start_node.vlabel_list_g1 = vlabel_list_g1;
	start_node.vlabel_list_g2 = vlabel_list_g2;
	start_node.elabel_list_g1 = elabel_list_g1;
	start_node.elabel_list_g2 = elabel_list_g2;
	for (i = 0; i < pri->lg->vertex_num; ++ i)
		start_node.vertex_mapping_g1[i] = UNMAPPED;
	for (i = 0; i < pri->rg->vertex_num + tau; ++ i)
		start_node.vertex_mapping_g2[i] = UNMAPPED;
	key = generate_key(start_node.vertex_mapping_g1);

	/* insert start node */
	MinHeap<GED_node, GED_key> nodes;
	nodes.Insert(key, start_node);

	/* search for best mapping */
	GED_node current_node, next_node;
	bool dummy_flag;
	int f_value, g_value_new, h_value_new;
	int next_vertex_id_g1;
	int next_vertex_id_g2;
	int next_vertex_deg_g1;
	int next_vertex_deg_g2;
	int adjacent_vertex_g1_id;
	int adjacent_vertex_g2_id;
	int inner_edge_num_g1;
	int inner_edge_num_g2;

	nodes_expanded = nodes_popped = 0;
	while (!nodes.Empty()) {
		nodes.ExtractMin(key, current_node);
		if (current_node.mapped_vertex_num_g1 == pri->lg->vertex_num) {
			return current_node.f_value;
		} else {
			f_value = current_node.f_value;
		}

		/* update neighbor nodes */
		dummy_flag = 0;
		next_vertex_id_g1 = pri->lv_sort[current_node.mapped_vertex_num_g1].id;
		next_vertex_deg_g1 = pri->lv_sort[current_node.mapped_vertex_num_g1].deg;
		for (i = 0; i < pri->lv_sort[current_node.mapped_vertex_num_g1].cand_num; ++ i) {
			if (dummy_flag) break;
			next_vertex_id_g2 = pri->lv_cand[next_vertex_id_g1][i];
			if (current_node.vertex_mapping_g2[next_vertex_id_g2] != UNMAPPED) continue; 

			if (next_vertex_id_g2 >= pri->rg->vertex_num) dummy_flag = 1;
			g_value_new = current_node.g_value + ((*pri->lg)[next_vertex_id_g1].vlabel == (*pri->rg)[next_vertex_id_g2].vlabel ? 0 : 1);
			next_vertex_deg_g2 = (*pri->rg)[next_vertex_id_g2].degree;
			if (g_value_new + abs(next_vertex_deg_g1 - next_vertex_deg_g2) > tau) continue; // g(x) + delta(degree) > tau
			++ nodes_expanded;
			/* compute g(x) */
			/* compare new induced graph */
			for (j = 0, inner_edge_num_g1 = 0; j < next_vertex_deg_g1; ++ j) {
				adjacent_vertex_g1_id = (*pri->lg)[next_vertex_id_g1].edges[j].to;
				if (current_node.vertex_mapping_g1[adjacent_vertex_g1_id] != UNMAPPED) {
					++inner_edge_num_g1;
				   	if (!is_adjacent_with_label(*pri->rg, next_vertex_id_g2, current_node.vertex_mapping_g1[adjacent_vertex_g1_id], (*pri->lg)[next_vertex_id_g1].edges[j].elabel)) {
						++ g_value_new;
						if (g_value_new > tau) break;
					}
				}
			}
			if (g_value_new > tau) continue;
			for (j = 0, inner_edge_num_g2 = 0; j < next_vertex_deg_g2; ++ j) {
				adjacent_vertex_g2_id = (*pri->rg)[next_vertex_id_g2].edges[j].to;
				if (current_node.vertex_mapping_g2[adjacent_vertex_g2_id] != UNMAPPED) {
					++inner_edge_num_g2;
					if (!is_adjacent(*pri->lg, next_vertex_id_g1, current_node.vertex_mapping_g2[adjacent_vertex_g2_id])) {
						++ g_value_new;
						if (g_value_new > tau) break;
					}
				}
			}
			if (g_value_new > tau) continue;

			/* compute h(x) */
			next_node.g_value = g_value_new;
			next_node.mapped_vertex_num_g1 = current_node.mapped_vertex_num_g1 + 1;
			next_node.mapped_vertex_num_g2 = current_node.mapped_vertex_num_g2 + ((*pri->rg)[next_vertex_id_g2].vlabel == DUMMY_LABEL ? 0 : 1);
			next_node.remaining_edge_num_g1 = current_node.remaining_edge_num_g1 - inner_edge_num_g1;
			next_node.remaining_edge_num_g2 = current_node.remaining_edge_num_g2 - inner_edge_num_g2;

			/* ||V1|-|V2|| + ||E1|-|E2|| */
			h_value_new = abs((int(pri->lg->vertex_num) - next_node.mapped_vertex_num_g1) - (int(pri->rg->vertex_num) - next_node.mapped_vertex_num_g2)) + abs(next_node.remaining_edge_num_g1 - next_node.remaining_edge_num_g2);
			if (g_value_new + h_value_new > tau) continue;

			/* Delta(V) + Delta(E) */
			next_node.vertex_mapping_g1 = current_node.vertex_mapping_g1; 
			next_node.vertex_mapping_g2 = current_node.vertex_mapping_g2;
			next_node.vertex_mapping_g1[next_vertex_id_g1] = next_vertex_id_g2; 
			next_node.vertex_mapping_g2[next_vertex_id_g2] = next_vertex_id_g1;
			next_node.vlabel_list_g1 = current_node.vlabel_list_g1;
			next_node.vlabel_list_g2 = current_node.vlabel_list_g2;
			next_node.elabel_list_g1 = current_node.elabel_list_g1;
			next_node.elabel_list_g2 = current_node.elabel_list_g2;
			next_node.vlabel_list_g1[vid_to_vlabel_list_g1[next_vertex_id_g1]] = -1;
			for (j = 0; j < next_vertex_deg_g1; j++) {
				adjacent_vertex_g1_id = (*pri->lg)[next_vertex_id_g1].edges[j].to;
				if (next_node.vertex_mapping_g1[adjacent_vertex_g1_id] != UNMAPPED) 
					next_node.elabel_list_g1[eid_to_elabel_list_g1[next_vertex_id_g1][j]] = -1;
			}
			if (next_vertex_id_g2 < pri->rg->vertex_num) {
				next_node.vlabel_list_g2[vid_to_vlabel_list_g2[next_vertex_id_g2]] = -1;
				for (j = 0; j < next_vertex_deg_g2; j++) {
					adjacent_vertex_g2_id = (*pri->rg)[next_vertex_id_g2].edges[j].to;
					if (current_node.vertex_mapping_g2[adjacent_vertex_g2_id] != UNMAPPED) 
						next_node.elabel_list_g2[eid_to_elabel_list_g2[next_vertex_id_g2][j]] = -1;
				}
			}

			h_value_new = compute_label_difference(next_node.vlabel_list_g1, next_node.vlabel_list_g2);
			if (g_value_new + h_value_new > tau) continue;

			h_value_new += compute_label_difference(next_node.elabel_list_g1, next_node.elabel_list_g2);
			if (g_value_new + h_value_new > tau) continue;

			/* content based filtering, use ImprovedEstimateSwitch to control */
			if (tau == g_value_new) {	
				j = compute_mismatch_error(next_node, pri->lgid, pri->rgid, 0);
				if (g_value_new + j > tau) continue;
				if (j > h_value_new) h_value_new = j;
			}	

			/* update priority queue */
			key = generate_key(next_node.vertex_mapping_g1);
			next_node.f_value = g_value_new + h_value_new;
			if (nodes.Find(key) >= 0) {
				if (next_node < nodes.GetValue(key))
					nodes.UpdateKey(key, next_node);
			}
			else 
				nodes.Insert(key, next_node);
		}
	}

	return OVER_BOUND;
}

bool mis_deg_combine_match( const unsigned p, const unsigned pid, const unsigned q, unsigned* qid, unsigned beg, unsigned end, int threshold, unsigned& count )
{
	int max_p, max_q, sum_p, sum_q, err, p_elab, q_elab;
	Graph &pp = gdb[p], &qq = gdb[q];
	vector<Path_gram*> &rp = pdb[q];
	Path_gram *cur, *ref = pdb[p][pid];
	int* diff = new int[qs];
	bool round;
	
	while (true) {
		cur = rp[qid[beg]];
		if (ref->backward == cur->backward) {	// both forward or backward
			max_p = max_q = sum_p = sum_q = 0;
			round = true;
			for (int s = 0; s != qs; ++ s) {
				diff[s] = pp[ref->vpath[s]].degree - qq[cur->vpath[s]].degree;
				if (diff[s] > 0) {
					if (max_p < diff[s]) {
						max_p = diff[s];
						if (max_p + max_q > threshold) {
							round = false; break;
						}
					}
					sum_p += diff[s];
				}
				if (diff[s] < 0) {
					if (max_q < -diff[s]) {
						max_q = -diff[s];
						if (max_p + max_q > threshold) {
							round = false; break;
						}
					}
					sum_q += -diff[s];
				}
			}
			if (round && sum_p + sum_q <= (threshold << 1)) {
				err = 0;
				for (int s = 0; s < qs; ++ s) {
					for (int t = s+2; t < qs; ++ t) {
						if (p_elab = pp.get_elab(ref->vpath[s], ref->vpath[t]), p_elab != DUMMY_LABEL) {
							if (q_elab = qq.get_elab(cur->vpath[s], cur->vpath[t]), q_elab != DUMMY_LABEL) {	// both exist with identical label
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
				THRESHOLD_MATCH
			}
			if (ref->normal || cur->normal) {
				max_p = max_q = sum_p = sum_q = 0;
				round = true;
				for (int s = 0; s != qs; ++ s) {
					diff[s] = pp[ref->vpath[s]].degree - qq[cur->vpath[under_qs-s]].degree;
					if (diff[s] > 0) {
						if (max_p < diff[s]) {
							max_p = diff[s];
							if (max_p + max_q > threshold) {
								round = false; break;
							}
						}
						sum_p += diff[s];
					}
					if (diff[s] < 0) {
						if (max_q < -diff[s]) {
							max_q = -diff[s];
							if (max_p + max_q > threshold) {
								round = false; break;
							}
						}
						sum_q += -diff[s];
					}
				}
				if (round && sum_p + sum_q <= (threshold << 1)) {
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
					THRESHOLD_MATCH
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
						if (max_p + max_q > threshold) {
							round = false; break;
						}
					}
					sum_p += diff[s];
				}
				if (diff[s] < 0) {
					if (max_q < -diff[s]) {
						max_q = -diff[s];
						if (max_p + max_q > threshold) {
							round = false; break;
						}
					}
					sum_q += -diff[s];
				}
			}
			if (round && sum_p + sum_q <= (threshold << 1)) {
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
				THRESHOLD_MATCH
			}
			if (ref->normal || cur->normal) {
				max_p = max_q = sum_p = sum_q = 0;
				round = true;
				for (int s = 0; s != qs; ++ s) {
					diff[s] =  pp[ref->vpath[s]].degree - qq[cur->vpath[s]].degree; 
					if (diff[s] > 0) {
						if (max_p < diff[s]) {
							max_p = diff[s];
							if (max_p + max_q > threshold) {
								round = false; break;
							}
							sum_p += diff[s];
						}
						if (diff[s] < 0) {
							if (max_q < -diff[s]) {
								max_q = -diff[s];
								if (max_p + max_q > threshold) {
									round = false; break;
								}
							}
							sum_q += -diff[s];
						}
					}
				} 
				if (round && sum_p + sum_q <= (threshold << 1)) {
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
					THRESHOLD_MATCH
				}
			}
		}

		if (++ beg == end || rp[qid[beg]]->hash_val != rp[qid[beg-1]]->hash_val) 
			break;
	}

	delete[] diff;
	return false;
}

void Priority::init_astar()
{
	lv_cand.resize(lg_size);	
	lv_sort.resize(lg_size);
	int pos;
	unsigned i, j;
	for (i = 0; i != lg_size; ++ i) {
		for (pos = 0; pos != rg->vertex_num; ++ pos) {
			COMP_CD
		}
	}

	pos = -1;
	if (unmat != NULL) { 
		bool* canded = new bool[lg_size];
		memset(canded, 0, sizeof(bool)*lg_size);

		if (um_order != NULL) {
			for (i = 0; i != um_size; ++ i) {
				j = um_order[i].first;	
				for (vector<unsigned>::const_iterator it = (*unmat)[j].begin(), end = (*unmat)[j].end(); it != end; ++ it) {
					lv_sort[++ pos].id = *it;
					lv_sort[pos].deg = (*lg)[*it].degree;
					lv_sort[pos].vlabel = (*lg)[*it].vlabel;
					canded[*it] = true;
				}
			}
		} else {
			for (i = 0; i != um_size; ++ i) {
				for (vector<unsigned>::const_iterator it = (*unmat)[i].begin(), end = (*unmat)[i].end(); it != end; ++ it) {
					lv_sort[++ pos].id = *it;
					lv_sort[pos].deg = (*lg)[*it].degree;
					lv_sort[pos].vlabel = (*lg)[*it].vlabel;
					canded[*it] = true;
				}
			}
		}
		
		for (i = 0; i != lg_size; ++ i) {
			if (!canded[i]) {
				lv_sort[++ pos].id = i;
				lv_sort[pos].deg = (*lg)[i].degree;
				lv_sort[pos].vlabel = (*lg)[i].vlabel;
			}
		}
		delete[] canded;
	} else {
		for (i = 0; i != lg_size; ++ i) {
			lv_sort[++ pos].id = i;
			lv_sort[pos].deg = (*lg)[i].degree;
			lv_sort[pos].vlabel = (*lg)[i].vlabel;
		}
	}

	rg->resize(rg->vertex_num + tau);
	for (i = 0; i != lg_size; ++ i) {
		if ((*lg)[lv_sort[i].id].degree + 1 <= tau) {
			for (j = rg->vertex_num; j != rg->size(); ++ j) {
				lv_cand[lv_sort[i].id].push_back(j);
			}
		}
		lv_sort[i].cand_num = lv_cand[lv_sort[i].id].size();
	}
}