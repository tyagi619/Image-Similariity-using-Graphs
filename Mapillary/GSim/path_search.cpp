#include "path_search.h"
#include "filter.h"
#include "stopwatch.h"

unsigned gl_cand_1, gl_cand_2, gl_ans, gl_entry;
double _sec = 0, _usec = 0;
unsigned* query_order;
Stopwatch v_time;


void run_cnt_search()
{
	cout << "\nBuilding index...";
#ifdef UNIX
	timeval begin;
	gettimeofday(&begin, NULL);
#else
	double begin = (double)clock();  // type: clock_t
#endif
	/* index phase */
	vectorize_label();
	generate_all_path();
	count_all_path();
	full_graph_cnt_index();

#ifdef UNIX
	timeval end;
	gettimeofday(&end, NULL);
	cout << "Indexing time: " << setiosflags(ios::fixed) << setprecision(3)
		<< double(end.tv_sec-begin.tv_sec) + double(end.tv_usec-begin.tv_usec)/1e6 << endl;
#else
	double end = (double)clock();
	cout << "Indexing time: " << (end - begin)/CLOCKS_PER_SEC << endl;
#endif
	ECHO_INDEX
	/* query phase */
	for (unsigned i = 0; i != qry_size; ++ i) {
#ifdef UNIX
		timeval begin;
		gettimeofday(&begin, NULL);
#else
		double begin = (double)clock();  
#endif
		preprocess(gdb_size+i);
		proc_full_cnt_order(gdb_size+i);

#ifdef UNIX
		timeval end;
		gettimeofday(&end, NULL);
		_sec += double(end.tv_sec-begin.tv_sec); _usec += double(end.tv_usec-begin.tv_usec);
#else
		double end = (double)clock();
#endif
	}
	ECHO_SEARCH
}

void run_sim_search()
{
	cout << "\nBuilding index...";
#ifdef UNIX
	timeval begin;
	gettimeofday(&begin, NULL);
#else
	double begin = (double)clock();		
#endif
	/* index phase */
	vectorize_label();
	generate_all_path();
	count_all_path();
	full_graph_min_index();

#ifdef UNIX
	timeval end; gettimeofday(&end, NULL);
	cout << "Indexing time: " << setiosflags(ios::fixed) << setprecision(3)
		<< double(end.tv_sec-begin.tv_sec) + double(end.tv_usec-begin.tv_usec)/1e6 << "s" << endl;
#else
	double end = (double)clock();
	cout << "Indexing time: " << (end - begin)/CLOCKS_PER_SEC << "s" << endl;
#endif
	ECHO_INDEX
	/* query phase */
	for (unsigned i = 0; i != qry_size; ++ i) {
#ifdef UNIX 
	timeval begin; gettimeofday(&begin, NULL); 
#else 
	double begin = (double)clock(); 
#endif
		preprocess(gdb_size+i);
		if (!local_lab_filter) { proc_full_cnt_order(gdb_size+i); }
		else {
			switch (vf_order) {
			case '0': proc_full_rud_order(gdb_size+i); break;
			case '1': proc_full_imp_order(gdb_size+i); break;
			}
		}

#ifdef UNIX
		timeval end; gettimeofday(&end, NULL);
		_sec += double(end.tv_sec-begin.tv_sec);
		_usec += double(end.tv_usec-begin.tv_usec);
#else
		double end = (double)clock();
#endif
	}
	ECHO_SEARCH
}

void preprocess( const unsigned q )
{
	Graph& g = gdb[q];
	/* label, degree */
	vertex_label_pair* vpair_list = new vertex_label_pair[g.vertex_num];
	edge_label_pair* epair_list = new edge_label_pair[g.edge_num];

	unsigned i, j, count = -1, pos;
	for (i = 0; i != g.vertex_num; ++ i) {
		vpair_list[i].vid = i;
		vpair_list[i].vlabel = g[i].vlabel;

		for (j = 0; j != g[i].degree; ++ j) {
			if (i < g[i].edges[j].to) {
				epair_list[++ count].elabel = g[i].edges[j].elabel;
				epair_list[count].eid = j;
				epair_list[count].vid = i;
			}
			pos = bin_search(global_elabs, global_esize, g[i].edges[j].elabel);
			if (pos < global_esize) {
				++ g[i].elabs[pos];
			}
		}
	}

	sort(vpair_list, vpair_list+i);
	sort(epair_list, epair_list+g.edge_num);
	g.vlabel_list[0] = vpair_list[0].vlabel;
	g.vid_to_vlabel[vpair_list[0].vid] = 0;
	count = 1;
	for (i = 1; i != g.vertex_num; ++ i) {
		g.vlabel_list[i] = vpair_list[i].vlabel;
		g.vid_to_vlabel[vpair_list[i].vid] = i;
		if (vpair_list[i].vlabel == vpair_list[i-1].vlabel) {
			++ count;
		} else {
			vlab[q].push_back(vpair_list[i-1].vlabel);
			vcnt[q].push_back(count);
			count = 1;
		}
	}
	vlab[q].push_back(vpair_list[i-1].vlabel);
	vcnt[q].push_back(count);

	count = 1;
	g.elabel_list[0] = epair_list[0].elabel;
	g.eid_to_elabel[epair_list[0].vid][epair_list[0].eid] = 0;
	for (j = 0; j != g[g[epair_list[0].vid].edges[epair_list[0].eid].to].degree; ++ j) {
		if (g[g[epair_list[0].vid].edges[epair_list[0].eid].to].edges[j].to == epair_list[0].vid) {
			g.eid_to_elabel[g[epair_list[0].vid].edges[epair_list[0].eid].to][j] = 0;
		}
	}
	for (i = 1; i != g.edge_num; ++ i) {
		g.elabel_list[i] = epair_list[i].elabel;
		g.eid_to_elabel[epair_list[i].vid][epair_list[i].eid] = i;
		for (j = 0; j != g[g[epair_list[i].vid].edges[epair_list[i].eid].to].degree; ++ j) {
			if (g[g[epair_list[i].vid].edges[epair_list[i].eid].to].edges[j].to == epair_list[i].vid) {
				g.eid_to_elabel[g[epair_list[i].vid].edges[epair_list[i].eid].to][j] = i;
			}
		}
		if (epair_list[i].elabel == epair_list[i-1].elabel) {
			++ count;
		} else {
			elab[q].push_back(epair_list[i-1].elabel);
			ecnt[q].push_back(count);
			count = 1;
		}
	}
	elab[q].push_back(epair_list[i-1].elabel);
	ecnt[q].push_back(count);

	delete[] vpair_list; delete[] epair_list;

	/* path */
	bool* color = new bool[g.vertex_num+QUE_SIZE];
	memset(color, 0, sizeof(bool)*(g.vertex_num+QUE_SIZE));
	for (i = 0; i != g.vertex_num; ++ i) {
		dfs_path(q, i, i, NULL, 1, color);
	}
	g.path_num = pdb[q].size();
	delete[] color;

	sort(pdb[q].begin(), pdb[q].end(), fcomp);
	hdb[q] = copy_value(pdb[q]);
	for (i = 0; i != g.path_num; ++ i) {
		for (j = 0; j != qs; ++ j) {
			eds[q][pdb[q][i]->vpath[j]].insert(i);
		}
	}

	for (i = 0; i != g.vertex_num; ++ i) {
		card[q][i] = eds[q][i].size();
	}
	sort(card[q], card[q]+g.vertex_num, greater<unsigned>());
	for (i = 0; i <= tau; ++ i) {
		for (j = 0; j != i; ++ j) {
			tau_card[q][i] += card[q][j];
		}
	}
}

void full_graph_cnt_index()
{
	for (unsigned i = 0; i != gdb_size; ++ i) {
		if (gdb[i].path_num == 0) {
			uid.push_back(i);
			status[i] = -1;
		} else {
			glen[i] = cnt_plength(i);
			if (glen[i] > gdb[i].path_num) {
				uid.push_back(i);
				glen[i] = gdb[i].path_num;
				status[i] = 1;
			} else {
				for (unsigned j = 0; j != glen[i]; ++ j) {
					path_graph[pdb[i][j]->hash_val].insert(i);
				}
			}		
		}
	}
}

void full_graph_min_index()
{
	for (unsigned i = 0; i != gdb_size; ++ i) {
		if (gdb[i].path_num == 0) {
			uid.push_back(i);
			status[i] = -1;
		} else {
			glen[i] = min_plength(i);
			if (glen[i] > gdb[i].path_num) {
				uid.push_back(i);
				glen[i] = gdb[i].path_num;
				status[i] = 1;
			} else {
				for (unsigned j = 0; j != glen[i]; ++ j) {	// only status = 0 in inverted list
					path_graph[pdb[i][j]->hash_val].insert(i);
				}
			}	
		}
	}
}

void proc_full_cnt_order( const unsigned q )
{
	bool qry_status = true;
	int qry_len, cand_1 = 0, cand_2 = 0, ans = 0;
	Graph& g = gdb[q];
	unsigned low_size = g.total_num - tau, up_size = g.total_num + tau;
	vector<Path_gram*>& p = pdb[q];
	Priority *pri;

	if (g.path_num == 0) { qry_status = false; }
	else {
		qry_len = min_edit_filter? min_plength(q): cnt_plength(q);
		if (qry_len > g.path_num) { qry_len = g.path_num; }
	}
	
	if (qry_status) {	// probe inverted index
		const_iter_graph qit;
		bool* processed = new bool[gdb_size];
		memset(processed, 0, gdb_size*sizeof(bool));

		for (int i = 0; i != qry_len; ++ i) {
			qit = path_graph.find(p[i]->hash_val);
			if (qit != path_graph.end()) {
				for (set<unsigned>::const_iterator ip = qit->second.begin(), end = qit->second.end(); ip != end; ++ ip) {
					if (processed[*ip]) continue;
					processed[*ip] = true;

					if (gdb[*ip].total_num < low_size) continue;
					if (gdb[*ip].total_num > up_size) break;
					if (size_filtering(*ip, q)) {
						++ cand_1;
						if (label_filtering(*ip, q)) {
							if (count_filtering(*ip, q)) {
								++ cand_2;
								pri = new Priority(q, *ip);
								compute_rud_dist(pri, ans, filter_only);
								delete pri;
							}
						}
					}
				}
			}
		}

		for (unsigned i = 0, j = uid.size(); i != j; ++ i) {
			if (gdb[uid[i]].total_num < low_size) continue;
			else if (gdb[uid[i]].total_num > up_size) break;
			else if (size_filtering(uid[i], q)) {
				++ cand_1;
				if (label_filtering(uid[i], q)) {
					if (count_filtering(uid[i], q)) {
						++ cand_2;
						pri = new Priority(q, uid[i]);
						compute_rud_dist(pri, ans, filter_only);
						delete pri;
					}	
				}
			}
		}
		delete[] processed;
	} else {
		for (unsigned i = 0; i != gdb_size; ++ i) {
			if (gdb[i].total_num < low_size) continue;
			if (gdb[i].total_num > up_size) break;
			if (size_filtering(i, q)) {
				++ cand_1;
				if (label_filtering(i, q)) {
					if (count_filtering(i, q)) {
						++ cand_2;
						pri = new Priority(q, i);
						compute_rud_dist(pri, ans, filter_only);
						delete pri;
					}
				}
			}
		}
	}

	gl_cand_1 += cand_1; gl_cand_2 += cand_2; gl_ans += ans;
}

void proc_full_rud_order( const unsigned q )
{
	bool qry_status = true;
	int qry_len, cand_1 = 0, cand_2 = 0, ans = 0;
	Graph& g = gdb[q];
	unsigned low_size = g.total_num - tau, up_size = g.total_num + tau;
	vector<Path_gram*>& p = pdb[q];
	Priority *pri;

	if (g.path_num == 0) { qry_status = false; }
	else {
		qry_len = min_plength(q);
		if (qry_len > g.path_num) {	qry_len = g.path_num; }
	}
	
	if (qry_status) {	
		const_iter_graph qit;
		bool* processed = new bool[gdb_size];
		memset(processed, 0, gdb_size*sizeof(bool));

		for (int i = 0; i != qry_len; ++ i) {
			qit = path_graph.find(p[i]->hash_val);
			if (qit != path_graph.end()) {
				for (set<unsigned>::const_iterator ip = qit->second.begin(), end = qit->second.end(); ip != end; ++ ip) {
					if (processed[*ip]) continue;
					processed[*ip] = true; 

					if (gdb[*ip].total_num < low_size) continue;
					if (gdb[*ip].total_num > up_size) break;
					if (size_filtering(*ip, q)) {
						++ cand_1;
						if (label_filtering(*ip, q)) {
							pri = norder_content_filtering(*ip, q);
							if (pri != NULL) {
								++ cand_2;
								compute_rud_dist(pri, ans, filter_only);
								delete pri;
							}						
						}
					}
				}
			}
		}
		for (unsigned i = 0, j = uid.size(); i != j; ++ i) {
			if (gdb[uid[i]].total_num < low_size) continue;
			if (gdb[uid[i]].total_num > up_size) break;
			if (size_filtering(uid[i], q)) {
				++ cand_1;
				if (label_filtering(uid[i], q)) {
					if (gdb[uid[i]].path_num == 0 && gdb[q].path_num == 0) {
						++ cand_2;
						pri = new Priority(q, uid[i]);
						compute_rud_dist(pri, ans, filter_only);
						delete pri;
					} else {
						pri = norder_content_filtering(uid[i], q);
						if (pri != NULL) {
							++ cand_2;
							compute_rud_dist(pri, ans, filter_only);
							delete pri;
						}
					}		
				}
			}
		}
		delete[] processed;
	} else {
		for (unsigned i = 0; i != gdb_size; ++ i) {
			if (gdb[i].total_num < low_size) continue;
			if (gdb[i].total_num > up_size) break;
			if (size_filtering(i, q)) {
				++ cand_1;
				if (label_filtering(i, q)) {
					pri = norder_content_filtering(i, q);
					if (pri != NULL) {
						++ cand_2;
						compute_rud_dist(pri, ans, filter_only);
						delete pri;
					}
				}
			}
		}
	}
	
	gl_cand_1 += cand_1; gl_cand_2 += cand_2; gl_ans += ans;
}

void proc_full_imp_order( const unsigned q )
{
	bool qry_status = true;
	int qry_len, cand_1 = 0, cand_2 = 0, ans = 0;
	Graph& g = gdb[q];
	unsigned low_size = g.total_num - tau, up_size = g.total_num + tau;
	vector<Path_gram*>& p = pdb[q];
	Priority *pri;

	if (g.path_num == 0) { qry_status = false; }
	else {
		qry_len = min_plength(q);
		if (qry_len > g.path_num) {	
			qry_len = g.path_num;
		}
	}
	
	if (qry_status) {	// probe inverted index
		const_iter_graph qit;
		bool* processed = new bool[gdb_size];
		memset(processed, 0, gdb_size*sizeof(bool));
		for (int i = 0; i != qry_len; ++ i) {
			qit = path_graph.find(p[i]->hash_val);
			if (qit != path_graph.end()) {
				for (set<unsigned>::const_iterator ip = qit->second.begin(), end = qit->second.end(); ip != end; ++ ip) {
					if (processed[*ip]) continue;
					processed[*ip] = true; 
					if (gdb[*ip].total_num < low_size) continue;
					if (gdb[*ip].total_num > up_size) break;
					if (size_filtering(*ip, q)) {
						++ cand_1;
						if (label_filtering(*ip, q)) {
							pri = content_filtering(*ip, q);
							if (pri != NULL) {
								++ cand_2;
								compute_rud_dist(pri, ans, filter_only);
								delete pri;
							}						
						}
					}
				}
			}

		}
		for (unsigned i = 0, j = uid.size(); i != j; ++ i) {
			if (gdb[uid[i]].total_num < low_size) continue;
			if (gdb[uid[i]].total_num > up_size) break;
			if (size_filtering(uid[i], q)) {
				++ cand_1;
				if (label_filtering(uid[i], q)) {
					if (gdb[uid[i]].path_num == 0 && gdb[q].path_num == 0) {
						++ cand_2;
						pri = new Priority(q, uid[i]);
						compute_rud_dist(pri, ans, filter_only);
						delete pri;
					} else {
						pri = content_filtering(uid[i], q);
						if (pri != NULL) {
							++ cand_2;
							compute_rud_dist(pri, ans, filter_only);
							delete pri;
						}
					}		
				}
			}
		}
		delete[] processed;
	} else {
		for (unsigned i = 0; i != gdb_size; ++ i) {
			if (gdb[i].total_num < low_size) continue;
			if (gdb[i].total_num > up_size) break;
			if (size_filtering(i, q)) {
				++ cand_1;
				if (label_filtering(i, q)) {
					pri = content_filtering(i, q);
					if (pri != NULL) {
						++ cand_2;
						compute_rud_dist(pri, ans, filter_only);
						delete pri;
					}
				}
			}
		}
	}
	
	gl_cand_1 += cand_1; gl_cand_2 += cand_2; gl_ans += ans;
}

int max_err, acc_num = 0;

void run_rs_join( void )
{
cout << "\nBuilding index for R...";
#ifdef UNIX
	timeval begin;
	gettimeofday(&begin, NULL);
#else
	double begin = (double)clock();		
#endif
	/* index phase */
	vectorize_label();
	generate_all_path();
	count_all_path();
	full_graph_min_index();

#ifdef UNIX
	timeval end; gettimeofday(&end, NULL);
	cout << "Indexing time: " << setiosflags(ios::fixed) << setprecision(3)
		<< double(end.tv_sec-begin.tv_sec) + double(end.tv_usec-begin.tv_usec)/1e6 << endl;
#else
	double end = (double)clock();
	cout << "Indexing time: " << (end - begin)/CLOCKS_PER_SEC << endl;
#endif
	ECHO_INDEX
	/* query phase */
	for (unsigned i = 0; i != qry_size; ++ i) {
#ifdef UNIX 
	timeval begin; gettimeofday(&begin, NULL); 
#else 
	double begin = (double)clock(); 
#endif
		preprocess(gdb_size+i);
		if (!local_lab_filter) { proc_full_cnt_order(gdb_size+i); }
		else {
			switch (vf_order) {
				case '0': proc_full_rud_order(gdb_size+i); break;
				case '1': proc_full_imp_order(gdb_size+i); break;
			}
		}

#ifdef UNIX
		timeval end; gettimeofday(&end, NULL);
		_sec += double(end.tv_sec-begin.tv_sec);
		_usec += double(end.tv_usec-begin.tv_usec);
#else
		double end = (double)clock();
#endif
	}
	ECHO_SEARCH
}