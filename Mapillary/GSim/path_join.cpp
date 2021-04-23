#include <assert.h>
#include "path_join.h"
#include "path_search.h"
#include "set_cover.h"
#include "filter.h"


void vectorize_label( void )
{
	vertex_label_pair* vpair_list;
	edge_label_pair* epair_list;
	unsigned count, j, k, pos;
	for (unsigned i = 0; i != gdb_size; ++ i) {
		vpair_list = new vertex_label_pair[gdb[i].vertex_num];
		epair_list = new edge_label_pair[gdb[i].edge_num];

		count = -1;
		for (j = 0; j != gdb[i].vertex_num; ++ j) {
			/* prepare all degree array */
			vpair_list[j].vid = j;
			vpair_list[j].vlabel = gdb[i][j].vlabel;

			for (k = 0; k != gdb[i][j].degree; ++ k) {
				if (j < gdb[i][j].edges[k].to) {
					epair_list[++ count].elabel = gdb[i][j].edges[k].elabel;
					epair_list[count].eid = k;
					epair_list[count].vid = j;
				}
				pos = bin_search(global_elabs, global_esize, gdb[i][j].edges[k].elabel);
				assert(pos != global_esize);
				++ gdb[i][j].elabs[pos];
			}
		}
		/* prepare all degree array */
		sort(vpair_list, vpair_list+j);
		sort(epair_list, epair_list+gdb[i].edge_num);

		gdb[i].vlabel_list[0] = vpair_list[0].vlabel;
		gdb[i].vid_to_vlabel[vpair_list[0].vid] = 0;
		count = 1;
		for (j = 1; j != gdb[i].vertex_num; ++ j) {
			gdb[i].vlabel_list[j] = vpair_list[j].vlabel;
			gdb[i].vid_to_vlabel[vpair_list[j].vid] = j;
			if (vpair_list[j].vlabel == vpair_list[j-1].vlabel) {
				++ count;
			} else {
				vlab[i].push_back(vpair_list[j-1].vlabel);
				vcnt[i].push_back(count);
				count = 1;
			}
		}
		vlab[i].push_back(vpair_list[j-1].vlabel);
		vcnt[i].push_back(count);

		count = 1;
		gdb[i].elabel_list[0] = epair_list[0].elabel;
		gdb[i].eid_to_elabel[epair_list[0].vid][epair_list[0].eid] = 0;
		for (k = 0; k != gdb[i][gdb[i][epair_list[0].vid].edges[epair_list[0].eid].to].degree; ++ k) {
			if (gdb[i][gdb[i][epair_list[0].vid].edges[epair_list[0].eid].to].edges[k].to == epair_list[0].vid) {
				gdb[i].eid_to_elabel[gdb[i][epair_list[0].vid].edges[epair_list[0].eid].to][k] = 0;
			}
		}
		for (j = 1; j != gdb[i].edge_num; ++ j) {
			gdb[i].elabel_list[j] = epair_list[j].elabel;
			gdb[i].eid_to_elabel[epair_list[j].vid][epair_list[j].eid] = j;
			for (k = 0; k != gdb[i][gdb[i][epair_list[j].vid].edges[epair_list[j].eid].to].degree; ++ k) {
				if (gdb[i][gdb[i][epair_list[j].vid].edges[epair_list[j].eid].to].edges[k].to == epair_list[j].vid) {
					gdb[i].eid_to_elabel[gdb[i][epair_list[j].vid].edges[epair_list[j].eid].to][k] = j;
				}
			}
			if (epair_list[j].elabel == epair_list[j-1].elabel) {
				++ count;
			} else {
				elab[i].push_back(epair_list[j-1].elabel);
				ecnt[i].push_back(count);
				count = 1;
			}
		}
		elab[i].push_back(epair_list[j-1].elabel);
		ecnt[i].push_back(count);

		delete[] vpair_list;
		delete[] epair_list;
	}
}

void generate_all_path( void )
{
	bool* color = new bool[max_vnum + QUE_SIZE];
	memset(color, 0, sizeof(bool)*(max_vnum + QUE_SIZE));
	for (unsigned i = 0; i != gdb_size; ++ i) {
		for (unsigned j = 0; j != gdb[i].vertex_num; ++ j) {
			dfs_path(i, j, j, NULL, 1, color);
		}
		gdb[i].path_num = pdb[i].size();
	}
	delete[] color;
}

void dfs_path( const unsigned g, const unsigned source, unsigned v, Path_gram* prv_gram, int depth, bool* color )
{
	Path_gram* cur_gram;
	if (prv_gram == NULL) {
		cur_gram = new Path_gram();
	} else {
		cur_gram = prv_gram;
	}
	color[v] = true;
	cur_gram->vpath[depth-1] = v;
	cur_gram->chars_path[(depth-1) << 1] = gdb[g][v].vlabel;

	if (depth != qs) {
		for (unsigned i = 0; i != gdb[g][v].degree; ++ i) {
			if (!color[gdb[g][v].edges[i].to]) {
				cur_gram->chars_path[1 + ((depth-1) << 1)] = gdb[g][v].edges[i].elabel;
				cur_gram->epath[depth-1] = gdb[g][v].edges[i].eid;
				dfs_path(g, source, gdb[g][v].edges[i].to, cur_gram, depth+1, color);
			}
		}
	} else if (v > source) {
		Path_gram* valid_path = new Path_gram(*cur_gram);
		valid_path->fill_path_info();
		pdb[g].push_back(valid_path);
	} 

	color[v] = false;
	if (prv_gram == NULL) {
		delete cur_gram;
	}
}

void count_all_path( void )
{
	unsigned prv_val;
	for (unsigned i = 0; i != gdb_size; ++ i) {
		prv_val = 0;
		for (unsigned j = 0; j != gdb[i].path_num; ++ j) {
			if (pdb[i][j]->hash_val != prv_val) {
				prv_val = pdb[i][j]->hash_val;
				++ path_freq[pdb[i][j]->hash_val];
			}
		}
	}

	for (unsigned i = 0; i != gdb_size; ++ i) {
		sort(pdb[i].begin(), pdb[i].end(), fcomp);
		hdb[i] = copy_value(pdb[i]);	
		for (unsigned j = 0; j != gdb[i].path_num; ++ j) {
			for (unsigned k = 0; k != qs; ++ k) {
				eds[i][pdb[i][j]->vpath[k]].insert(j);
			}
		}

		for (unsigned j = 0; j != gdb[i].vertex_num; ++ j) {
			card[i][j] = eds[i][j].size();
		}
		sort(card[i], card[i] + gdb[i].vertex_num, greater<unsigned>());
		
		for (int m = 1; m <= tau; ++ m) {
			for (int n = 0; n != m; ++ n) {
				tau_card[i][m] += card[i][n];
			}
		}
	}
}

void cnt_prefix_list( void )
{
	for (unsigned i = 0; i != gdb_size; ++ i) {
		if (gdb[i].path_num == 0) {
			uid.push_back(i);	// underflow graphs with no valid gram
			status[i] = -1;		// not take part in probing inverted list
		} else {
			glen[i] = cnt_plength(i);

			if (glen[i] > gdb[i].path_num) {
				uid.push_back(i);			// underflow graphs without enough gram
				status[i] = 1;
				glen[i] = gdb[i].path_num;	// take part in probing inverted list
			}
			git[i].reserve(glen[i]);	
			for (unsigned j = 0; j != glen[i]; ++ j) {
				path_graph[pdb[i][j]->hash_val].insert(i);
			}
		}	
	}
	
	// clean inverted lists
	iter_graph temp;
	for (iter_graph it = path_graph.begin(); it != path_graph.end();) {
		if (it->second.size() < 2) {
			temp = it;
			++ it;
			path_graph.erase(temp);
		} else {
			for (set<unsigned>::const_iterator is = it->second.begin(); is != it->second.end(); ++ is) {
				git[*is].push_back(it);
			}
			++ it;
		}
	}
}

void join_min_prefix_index( void )
{
	/* status: 0 - normal graph
	           1 - underflow graph without enough grams, into uid
			  -1 - underflow graph with no gram, into uid */
	for (unsigned i = 0; i != gdb_size; ++ i) {
		if (gdb[i].path_num == 0) {
			uid.push_back(i);	
			status[i] = -1;		// not take part in probing inverted list
		} else {
			glen[i] = min_plength(i);
			if (glen[i] > gdb[i].path_num) {
				uid.push_back(i);			
				status[i] = 1;
				glen[i] = gdb[i].path_num;	// take part in probing inverted list
			}
			git[i].reserve(glen[i]);	
			for (unsigned j = 0; j != glen[i]; ++ j) {
				path_graph[pdb[i][j]->hash_val].insert(i);
			}
		}	
	}
}

/* return the length of the prefixes 
   1) under prefix filtering principle */
unsigned cnt_plength( const unsigned g ) 
{
	return (tau*card[g][0] + 1);
}

/* 2) under minimum edit filtering principle */
unsigned min_plength( const unsigned g )
{
	unsigned mid = tau_card[g][tau] + 1;	
	unsigned left = over_tau;
	unsigned right = gdb[g].path_num+1 < mid? gdb[g].path_num+1: mid;

	int err = 0;
	while (left < right) {
		mid = (left + right) >> 1;
		err = thresh_ext_err(g, mid);
		err <= tau? left = (++ mid): right = mid;
	}
	return left;
}

void cnt_filter_join( void )
{
	unsigned* tested = new unsigned[gdb_size], test_cnt = -1, free_end = 0;
	int* backup = new int[gdb_size], ans = 0, dist_cand = 0, cand = 0;
	set<unsigned>::const_iterator ip;
	Priority* pri;

#ifdef UNIX
	timeval begin;
	gettimeofday(&begin, NULL);
#else
	double begin = (double)clock();		
#endif
	// indexed graphs
	for (unsigned i = 0, j = gdb_size - 1; i != j;) {
		switch (status[i])
		{
		case 0: 
			for (vector<iter_graph>::const_iterator iv = git[i].begin(), end = git[i].end(); iv != end; ++ iv) {
				ip = (*iv)->second.find(i);
				for (++ ip; ip != (*iv)->second.end(); ++ ip) {
					if (status[*ip] == -1) continue;	  // class 0 vs. class 0 and 1

					tested[++ test_cnt] = *ip;
					backup[test_cnt] = status[*ip];
					status[*ip] = -1;

					if (*ip > free_end) {
						if (abs((int)gdb[*ip].total_num - (int)gdb[i].total_num) <= tau) {
							free_end = *ip;
						} else break;
					}
					if (size_filtering(*ip, i)) {
						++ cand;
						if (label_filtering(*ip, i)) {
							if (count_filtering(*ip, i)) {
								++ dist_cand;
								pri = new Priority(i, *ip);
								compute_rud_dist(pri, ans, filter_only);
								delete pri;
							}
						}
					}
				}
			}
			break; 
		case 1: 
			for (vector<iter_graph>::const_iterator iv = git[i].begin(), end = git[i].end(); iv != end; ++ iv) {
				ip = (*iv)->second.find(i);
				for (++ ip; ip != (*iv)->second.end(); ++ ip) {
					if (status[*ip] == 0) {  // class 1 vs. class 0
						tested[++ test_cnt] = *ip;
						backup[test_cnt] = status[*ip];
						status[*ip] = -1;

						if (*ip > free_end) {
							if (abs((int)gdb[*ip].total_num - (int)gdb[i].total_num) <= tau) {
								free_end = *ip;
							} else break;
						}

						if (size_filtering(*ip, i)) {
							++ cand;
							if (label_filtering(*ip, i)) {
								if (count_filtering(*ip, i)) {
									++ dist_cand;
									pri = new Priority(i, *ip);
									compute_rud_dist(pri, ans, filter_only);
									delete pri;
								}
							}
						}
					}
				}
			}
			break; 
		default: 
			++ i;
		}

		++ test_cnt;
		if (++ i != gdb_size - 1) {		
			for (unsigned j = 0; j != test_cnt; ++ j) {
				status[tested[j]] = backup[j];
			}
			test_cnt = -1;
		} else break;		
	}
	// un-indexed graphs
	free_end = 0;
	for (unsigned i = 0, m = uid.size(); i != m; ++ i) {
		for (unsigned j = i + 1, n = uid.size(); j != n; ++ j) {
			if (uid[j] > free_end) {
				if (abs((int)gdb[uid[j]].total_num - (int)gdb[uid[i]].total_num) <= tau) {
					free_end = uid[j];
				} else break;
			}

			if (size_filtering(uid[j], uid[i])) {
				++ cand;
				if (label_filtering(uid[j], uid[i])) {
					if (count_filtering(uid[j], uid[i])) {
						++ dist_cand;
						pri = new Priority(uid[i], uid[j]);
						compute_rud_dist(pri, ans, filter_only);
						delete pri;
					}
				}
			}
		}
	}
#ifdef UNIX
	timeval end;
	gettimeofday(&end, NULL);
	cout << "\nSelf-join resp. time: " << setiosflags(ios::fixed) << setprecision(3)\
		<< double(end.tv_sec-begin.tv_sec) + double(end.tv_usec-begin.tv_usec)/1e6 << endl;
#else
	double end = (double)clock();
	cout << "\nSelf-join resp. time: " << (end - begin)/CLOCKS_PER_SEC << endl;
#endif

	ECHO_JOIN
	delete[] tested; delete[] backup;
}

void rud_order_join( void )
{
	unsigned* tested = new unsigned[gdb_size], test_cnt = -1, free_end = 0;
	int* backup = new int[gdb_size], ans = 0, dist_cand = 0, cand = 0;
	set<unsigned>::const_iterator ip;
	Priority *pri;

#ifdef UNIX
	timeval begin;
	gettimeofday(&begin, NULL);
#else
	double begin = (double)clock();		
#endif
	// indexed graphs
	for (unsigned i = 0, j = gdb_size - 1; i != j;) {
		switch (status[i])
		{
		case 0: 
			for (vector<iter_graph>::const_iterator iv = git[i].begin(), end = git[i].end(); iv != end; ++ iv) {
				ip = (*iv)->second.find(i);
				for (++ ip; ip != (*iv)->second.end(); ++ ip) {
					if (status[*ip] == -1) continue;	  // class 0 vs. class 0 and 1

					tested[++ test_cnt] = *ip;
					backup[test_cnt] = status[*ip];
					status[*ip] = -1;

					if (*ip > free_end) {
						if (abs((int)gdb[*ip].total_num - (int)gdb[i].total_num) <= tau) {
							free_end = *ip;
						} else break;
					}
					if (size_filtering(*ip, i)) {
						++ cand;
						if (label_filtering(*ip, i)) {
							pri = norder_content_filtering(*ip, i);
							if (pri != NULL) {
								++ dist_cand;
								compute_rud_dist(pri, ans, filter_only);
								delete pri;
							}
						}
					}
				}
			}
			break; 
		case 1: 
			for (vector<iter_graph>::const_iterator iv = git[i].begin(), end = git[i].end(); iv != end; ++ iv) {
				ip = (*iv)->second.find(i);
				for (++ ip; ip != (*iv)->second.end(); ++ ip) {
					if (status[*ip] == 0) {  // class 0 vs. class 1
						tested[++ test_cnt] = *ip;
						backup[test_cnt] = status[*ip];
						status[*ip] = -1;

						if (*ip > free_end) {
							if (abs((int)gdb[*ip].total_num - (int)gdb[i].total_num) <= tau) {
								free_end = *ip;
							} else break;
						}

						if (size_filtering(*ip, i)) {
							++ cand;
							if (label_filtering(*ip, i)) {
								pri = norder_content_filtering(*ip, i);
								if (pri != NULL) {
									++ dist_cand;
									compute_rud_dist(pri, ans, filter_only);
									delete pri;
								}
							}
						}
					}
				}
			}
			break; 
		default: 
			++ i;
		}

		++ test_cnt;
		if (++ i != gdb_size - 1) {
			for (unsigned j = 0; j != test_cnt; ++ j) {
				status[tested[j]] = backup[j];
			}
			test_cnt = -1;
		} else break;		
	}
	// un-indexed graphs
	free_end = 0;
	for (unsigned i = 0, m = uid.size(); i != m; ++ i) {
		for (unsigned j = i + 1, n = uid.size(); j != n; ++ j) {
			if (uid[j] > free_end) {
				if (abs((int)gdb[uid[j]].total_num - (int)gdb[uid[i]].total_num) <= tau) {
					free_end = uid[j];
				} else break;
			}

			if (size_filtering(uid[j], uid[i])) {
				++ cand;
				if (label_filtering(uid[j], uid[i])) {
					if (gdb[uid[j]].path_num == 0 && gdb[uid[i]].path_num == 0) {
						++ dist_cand;
						pri = new Priority(uid[i], uid[j]);	
						compute_rud_dist(pri, ans, filter_only);
						delete pri;
					} else {
						pri = norder_content_filtering(uid[j], uid[i]);
						if (pri != NULL) {
							++ dist_cand;
							compute_rud_dist(pri, ans, filter_only);
							delete pri;
						}
					}
				}
			}
		}
	}
#ifdef UNIX
	timeval end;
	gettimeofday(&end, NULL);
	cout << "\nSelf-join resp. time: " << setiosflags(ios::fixed) << setprecision(3) << double(end.tv_sec-begin.tv_sec) + double(end.tv_usec-begin.tv_usec)/1e6 << endl;
#else
	double end = (double)clock();
	cout << "\nSelf-join resp. time: " << (end - begin)/CLOCKS_PER_SEC << endl;
#endif

	ECHO_JOIN
	delete[] tested; delete[] backup;
}

void opt_order_join( void )
{
	unsigned* tested = new unsigned[gdb_size], test_cnt = -1, free_end = 0;
	int* backup = new int[gdb_size], ans = 0, dist_cand = 0, cand = 0;
	set<unsigned>::const_iterator ip;
	Priority *pri;

#ifdef UNIX
	timeval begin;
	gettimeofday(&begin, NULL);
#else
	double begin = (double)clock();		
#endif
	/* indexed graphs */
	for (unsigned i = 0, j = gdb_size - 1; i != j;) {
		switch (status[i])
		{
		case 0: 
			for (vector<iter_graph>::const_iterator iv = git[i].begin(), end = git[i].end(); iv != end; ++ iv) {
				ip = (*iv)->second.find(i);
				for (++ ip; ip != (*iv)->second.end(); ++ ip) {
					if (status[*ip] == -1) continue;  // class 0 vs. class 0 and 1

					tested[++ test_cnt] = *ip;
					backup[test_cnt] = status[*ip];
					status[*ip] = -1;

					if (*ip > free_end) {
						if (abs((int)gdb[*ip].total_num - (int)gdb[i].total_num) <= tau) {
							free_end = *ip;
						} else break;
					}
					if (size_filtering(*ip, i)) {
						++ cand;
						if (label_filtering(*ip, i)) {
							pri = content_filtering(*ip, i);
							if (pri != NULL) {
								++ dist_cand;
								compute_rud_dist(pri, ans, filter_only);
							}
							delete pri;
						}
					}
				}
			}
			break; 
		case 1: 
			for (vector<iter_graph>::const_iterator iv = git[i].begin(), end = git[i].end(); iv != end; ++ iv) {
				ip = (*iv)->second.find(i);
				for (++ ip; ip != (*iv)->second.end(); ++ ip) {
					if (status[*ip] == 0) {  // class 0 vs. class 1
						tested[++ test_cnt] = *ip;
						backup[test_cnt] = status[*ip];
						status[*ip] = -1;

						if (*ip > free_end) {
							if (abs((int)gdb[*ip].total_num - (int)gdb[i].total_num) <= tau) {
								free_end = *ip;
							} else break;
						}

						if (size_filtering(*ip, i)) {
							++ cand;
							if (label_filtering(*ip, i)) {
								pri = content_filtering(*ip, i);
								if (pri != NULL) {
									++ dist_cand;
									compute_rud_dist(pri, ans, filter_only);
								}
								delete pri;
							}
						}
					}
				}
			}
			break; 
		default: 
			++ i;	 
		}

		++ test_cnt;
		if (++ i != gdb_size - 1) {		
			for (unsigned j = 0; j != test_cnt; ++ j) {
				status[tested[j]] = backup[j];
			}
			test_cnt = -1;
		} else break;		
	}
	/* un-indexed graphs */
	free_end = 0;
	for (unsigned i = 0, m = uid.size(); i != m; ++ i) {
		for (unsigned j = i + 1, n = uid.size(); j != n; ++ j) {
			if (uid[j] > free_end) {
				if (abs((int)gdb[uid[j]].total_num - (int)gdb[uid[i]].total_num) <= tau) {
					free_end = uid[j];
				} else break;
			}

			if (size_filtering(uid[j], uid[i])) {
				++ cand;
				if (label_filtering(uid[j], uid[i])) {
					if (gdb[uid[j]].path_num == 0 && gdb[uid[i]].path_num == 0) {
						++ dist_cand;
						pri = new Priority(uid[i], uid[j]);	
						compute_rud_dist(pri, ans, filter_only);
						delete pri;
					} else {
						pri = content_filtering(uid[j], uid[i]);
						if (pri != NULL) {
							++ dist_cand;
							compute_rud_dist(pri, ans, filter_only);
						}
						delete pri;
					}
				}
			}
		}
	}
#ifdef UNIX
	timeval end;
	gettimeofday(&end, NULL);
	cout << "\nSelf-join resp. time: " << setiosflags(ios::fixed) << setprecision(3) << double(end.tv_sec-begin.tv_sec) + double(end.tv_usec-begin.tv_usec)/1e6 << endl;
#else
	double end = (double)clock();
	cout << "\nSelf-join resp. time: " << (end - begin)/CLOCKS_PER_SEC << endl;
#endif

	ECHO_JOIN
	delete[] tested; delete[] backup;
}

void imp_order_join( void )
{
	unsigned* tested = new unsigned[gdb_size], test_cnt = -1, free_end = 0;
	int* backup = new int[gdb_size], ans = 0, dist_cand = 0, cand = 0;
	set<unsigned>::const_iterator ip;
	Priority *pri;

#ifdef UNIX
	timeval begin;
	gettimeofday(&begin, NULL);
#else
	double begin = (double)clock();		
#endif
	/* indexed graphs */
	for (unsigned i = 0, j = gdb_size - 1; i != j;) {
		switch (status[i])
		{
		case 0: 
			for (vector<iter_graph>::const_iterator iv = git[i].begin(), end = git[i].end(); iv != end; ++ iv) {
				ip = (*iv)->second.find(i);
				for (++ ip; ip != (*iv)->second.end(); ++ ip) {
					if (status[*ip] == -1) continue;	  // class 0 vs. class 0 and 1

					tested[++ test_cnt] = *ip;
					backup[test_cnt] = status[*ip];
					status[*ip] = -1;

					if (*ip > free_end) {
						if (abs((int)gdb[*ip].total_num - (int)gdb[i].total_num) <= tau) {
							free_end = *ip;
						} else break;
					}
					if (size_filtering(*ip, i)) {
						++ cand;
						if (label_filtering(*ip, i)) {
							pri = content_filtering(*ip, i);
							if (pri != NULL) {
								++ dist_cand;
								compute_opt_dist(pri, ans, filter_only);
								delete pri;
							}
						}
					}
				}
			}
			break; 
		case 1: 
			for (vector<iter_graph>::const_iterator iv = git[i].begin(), end = git[i].end(); iv != end; ++ iv) {
				ip = (*iv)->second.find(i);
				for (++ ip; ip != (*iv)->second.end(); ++ ip) {
					if (status[*ip] == 0) {		// class 0 vs. class 1
						tested[++ test_cnt] = *ip;
						backup[test_cnt] = status[*ip];
						status[*ip] = -1;

						if (*ip > free_end) {
							if (abs((int)gdb[*ip].total_num - (int)gdb[i].total_num) <= tau) {
								free_end = *ip;
							} else break;
						}

						if (size_filtering(*ip, i)) {
							++ cand;
							if (label_filtering(*ip, i)) {
								pri = content_filtering(*ip, i);
								if (pri != NULL) {
									++ dist_cand;
									compute_opt_dist(pri, ans, filter_only);
									delete pri;
								}	
							}
						}
					}
				}
			}
			break; 
		default: 
			++ i;	
		}

		++ test_cnt;
		if (++ i != gdb_size - 1) {		
			for (unsigned j = 0; j != test_cnt; ++ j) {
				status[tested[j]] = backup[j];
			}
			test_cnt = -1;
		} else break;		
	}
	/* un-indexed graphs */
	free_end = 0;
	for (unsigned i = 0, m = uid.size(); i != m; ++ i) {
		for (unsigned j = i + 1, n = uid.size(); j != n; ++ j) {
			if (uid[j] > free_end) {
				if (abs((int)gdb[uid[j]].total_num - (int)gdb[uid[i]].total_num) <= tau) {
					free_end = uid[j];
				} else break;
			}

			if (size_filtering(uid[j], uid[j])) {
				++ cand;
				if (label_filtering(uid[j], uid[i])) {
					if (gdb[uid[j]].path_num == 0 && gdb[uid[i]].path_num == 0) {
						++ dist_cand;
						pri = new Priority(uid[i], uid[j]);	// no matching order, put smaller graph left
						compute_opt_dist(pri, ans, filter_only);
						delete pri;
					} else {
						pri = content_filtering(uid[j], uid[i]);
						if (pri != NULL) {
							++ dist_cand;
							compute_opt_dist(pri, ans, filter_only);
							delete pri;
						}
					}	
				}
			}
		}
	}
#ifdef UNIX
	timeval end;
	gettimeofday(&end, NULL);
	cout << "\nSelf-join resp. time: " << setiosflags(ios::fixed) << setprecision(3) << double(end.tv_sec-begin.tv_sec) + double(end.tv_usec-begin.tv_usec)/1e6 << endl;
#else
	double end = (double)clock();
	cout << "\nSelf-join resp. time: " << (end - begin)/CLOCKS_PER_SEC << endl;
#endif

	ECHO_JOIN
	delete[] tested; delete[] backup;
}

void run_min_prefix( void )
{
#ifdef UNIX
	timeval begin;
	gettimeofday(&begin, NULL);
#else
	double begin = (double)clock();		
#endif
	/* indexing phase */
	vectorize_label();
	generate_all_path();
	count_all_path();	
	join_min_prefix_index();

#ifdef UNIX
	timeval end;
	gettimeofday(&end, NULL);
	cout << "\nIndexing time: " << setiosflags(ios::fixed) << setprecision(3)
		<< double(end.tv_sec-begin.tv_sec) + double(end.tv_usec-begin.tv_usec)/1e6 << endl;
#else
	double end = (double)clock();
	cout << "\nIndexing time: " << (end - begin)/CLOCKS_PER_SEC << endl;
#endif

	/* clean inverted lists */
	iter_graph temp;
	for (iter_graph it = path_graph.begin(), end = path_graph.end(); it != end;) {
		if (it->second.size() < 2) {
			temp = it;
			++ it;
			path_graph.erase(temp);
		} else {
			for (set<unsigned>::const_iterator is = it->second.begin(), end = it->second.end(); is != end; ++ is) {
				git[*is].push_back(it);
			}
			++ it;
		}
	}
	ECHO_INDEX
	/* joining phase */
	if (!local_lab_filter) { 
		cnt_filter_join();
	} else { 
		switch (vf_order)
		{
		case '0': rud_order_join();	break;
		case '1': opt_order_join(); break;
		//case '2': imp_order_join(); // not always recommended, need to tune to perform well
		}
	}
}

void run_cnt_prefix( void )
{
#ifdef UNIX
	timeval begin;
	gettimeofday(&begin, NULL);
#else
	double begin = (double)clock();		
#endif
	/* indexing phase */
	vectorize_label();
	generate_all_path();
	count_all_path();
	cnt_prefix_list();
	
#ifdef UNIX
	timeval end;
	gettimeofday(&end, NULL);
	cout << "\nIndexing time: " << setiosflags(ios::fixed) << setprecision(3)
		<< double(end.tv_sec-begin.tv_sec) + double(end.tv_usec-begin.tv_usec)/1e6 << endl;
#else
	double end = (double)clock();
	cout << "\nIndexing time: " << end - begin << endl;
#endif
	ECHO_INDEX
	cnt_filter_join();
}