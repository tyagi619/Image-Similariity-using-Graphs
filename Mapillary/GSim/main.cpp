#include "common.h"
#include "path_join.h"
#include "path_search.h"

// setting variables
char vf_order = '1', version = '0';
bool count_filter = true, min_edit_filter = true, local_lab_filter = true, deg_match_cond = true;
bool filter_only = false, print_ans = false, print_more = false;

// trivial data structure
int tau = 2, over_tau = 3, qs = 5, path_qs = 9, under_qs = 4;
unsigned max_vnum;
bool* done;

// main data structure
vector<Graph> gdb;
vector<Path_gram*>* pdb;		// path grams, sorted in frequency
Hash_gram** hdb;				// hash values only, sorted in value
vector<unsigned> uid;			// underflow: ids of un-indexed graphs
set<unsigned>** eds;			// edit effect: ids of paths controlled by each vertex
unsigned* glen;                 // prefix length
fmap path_freq;
gmap path_graph;

// assistant data structure
unsigned gdb_size;				// number of data graphs
unsigned qry_size;				// number of query graphs
int* status;
unsigned global_esize;
unsigned* global_elabs;
unsigned** card;			    // size of each edit set
unsigned** tau_card;			// size of top-tau set
vector<int>* elab;				// edge labels
vector<unsigned>* ecnt;			// e-label counts
vector<int>* vlab;				// vertex labels
vector<unsigned>* vcnt;			// v-label counts
vector<iter_graph>* git;
Freq_comp fcomp(&path_freq);


void usage() {
	cerr << "GSim - Graph similarity queries with path q-grams\n\n"
		<< "Usage: GSim [OPTION...] [DB]... [Q] [TAU]\n\n"
		<< "Examples:\n"
		<< "GSimJoin -m0 -o1 -t2 db        # Self-join on db with 4-grams & threshold 2, applying count, min-edit, local-label filters and optimized verification\n"
		<< "GSimJoin -m2 -o0 -t3 db q 3 1  # Similarity search on db using q with 3-grams & threshold 1, applying all filters\n" << endl
		<< "Options:\n"
		<< "  -f            execute filtering only without verification\n"
		<< "  -h            display this help information\n"
		<< "  -m[i]         specify work mode\n"
		<< "                [0 - self-join (default) | 1 - R-S join | 2 - search]\n"
		<< "  -o[i]         specify verification method\n"
		<< "                [0 - basic | 1 - optimized, available with -t2]\n"
		<< "  -t[i]         apply filter combination\n"
		<< "                [0 - count filter | 1 - + min-edit filter | 2 - + local-label filter | 3 - + degree-based match (default)]\n"
		<< "  -v            verbosely print results with cerr (may sacrifice runtime performance)\n" << endl
		<< "Arguments:\n"
		<< "  DB            data graph file (mandatory)\n"
		<< "  Q             q-gram size - positive number of hops (default 4)\n"
		<< "  TAU           non-negative GED threshold (default 2)\n\n"
		<< "*This* GSim defaults to:\n"
		<< "-m0 -o1 -t3 [gdb] 4 2\n\n"
		<< "Report bugs to <xiangzhao@nudt.edu.cn>." << endl;
	exit(0);
}

void parse_data(char* file) {
	ifstream fin(file, ifstream::in);
	if (!fin) {
		cerr << file << " is not open or found!" << endl;
		exit(1);
	} else {
		cout << "Loading...\n" << endl;
	}

	done = init_bool_array(true, LAG_SIZE);

	set<int> edge_label;
	char tag;
	fin >> tag;
	while (!fin.eof()) {
		gdb.resize(gdb.size() + 1);
		tag = gdb.back().read(fin, tag, edge_label);
		if (gdb.back().size() <= tau || !gdb.back().connectivity() 
			|| !gdb.back().simplicity()) {	// discard disconnect graphs
			cout<<gdb.back().name<<" ";
			if(gdb.back().size()<=tau){
				cout<<"a";
			}
			cout<<"\n";
			gdb.resize(gdb.size() - 1);
		}
	}
	fin.close();

	global_esize = edge_label.size();
	global_elabs = new unsigned[global_esize];
	unsigned i = 0;
	for (set<int>::const_iterator it = edge_label.begin(), end = edge_label.end(); it != end; ++ i, ++ it) {
		global_elabs[i] = *it;
	}

	sort(gdb.begin(), gdb.end(), Graph_comp());	// sort graphs in ascending order w.r.t. |G|=|v|+|E|
	gdb_size = gdb.size();
	max_vnum = gdb.back().vertex_num;

#ifdef UNIX
	path_freq.set_empty_key(0);
	path_graph.set_empty_key(0);
	path_graph.set_deleted_key(OVER_BOUND);
#endif
}

void load_query(char* file) {
	ifstream fin(file, ifstream::in);
	if (!fin) {
		cerr << file << " is not open or found!" << endl; exit(1);
	}

	char tag;
	fin >> tag;
	while (!fin.eof()) {
		gdb.resize(gdb.size() + 1);
		tag = gdb.back().read(fin, tag);
		/*if (!gdb.back().connectivity()) {
			gdb.resize(gdb.size()-1);
		} else {*/
			if (gdb.back().vertex_num > max_vnum) max_vnum = gdb.back().vertex_num; 
		//}
	}
	fin.close();
}

void init_mem()
{
	unsigned total_size = gdb.size();
	qry_size = total_size - gdb_size;
	pdb = new vector<Path_gram*>[total_size];
	hdb = new Hash_gram*[total_size];
	glen = new unsigned[total_size];
	memset(glen, 0, sizeof(unsigned)*total_size);
	eds = new set<unsigned>*[total_size];
	card = new unsigned*[total_size];
	tau_card = new unsigned*[total_size];
	for (unsigned i = 0; i != total_size; ++ i) {
		eds[i] = new set<unsigned>[gdb[i].vertex_num];
		card[i] = new unsigned[gdb[i].vertex_num];
		tau_card[i] = new unsigned[over_tau];
		memset(tau_card[i], 0, sizeof(unsigned)*over_tau);
		gdb[i].vlabel_list = new int[gdb[i].vertex_num];
		gdb[i].vid_to_vlabel = new int[gdb[i].vertex_num];
		gdb[i].eid_to_elabel = new vector<int>[gdb[i].vertex_num];
		gdb[i].elabel_list = new int[gdb[i].edge_num];
		for (unsigned j = 0; j != gdb[i].vertex_num; ++ j) {
			gdb[i][j].elabs = new unsigned[global_esize];
			memset(gdb[i][j].elabs, 0, global_esize*sizeof(unsigned));
			gdb[i].eid_to_elabel[j].resize(gdb[i][j].degree);
		}
	}
	status = new int[total_size];
	memset(status, 0, sizeof(int)*total_size);	// default 0 (class-1), can be 1 (class-2), -1 (class-3)
	elab = new vector<int>[total_size];
	ecnt = new vector<unsigned>[total_size];
	vlab = new vector<int>[total_size];
	vcnt = new vector<unsigned>[total_size];
	git = new vector<iter_graph>[gdb_size];	// only used in join
}

void clean() {
	cout << "\nDone..." << endl;
	delete[] done;
	for (unsigned i = 0; i != gdb.size(); ++ i) {
		for (unsigned j = 0; j != pdb[i].size(); ++ j) {
			delete pdb[i][j];
		}
		for (unsigned j = 0; j != gdb[i].vertex_num; ++ j) {
			delete[] gdb[i][j].elabs;
		}
		delete[] eds[i];
		delete[] hdb[i];
		delete[] card[i];
		delete[] tau_card[i];
	}
	delete[] global_elabs;
	delete[] pdb;
	delete[] eds;
	delete[] hdb;
	delete[] glen;
	delete[] status;
	delete[] card;
	delete[] tau_card;
	delete[] elab;
	delete[] ecnt;
	delete[] vlab;
	delete[] vcnt;
	delete[] git;
}

int main(int argc, char** argv) 
{
	if (argc == 1) usage();
	if (argc < 2) {
		cerr << "Mandatory argument missing!\n" << endl; 
		usage();
	}

	char* pchar;
	bool file_set = false, q_set = false, tau_set = false;
	char dfile[256] = "gdb", qfile[256], comb;

	for (int ix = 1; ix < argc; ++ ix) {
		pchar = argv[ix];
		switch (pchar[0]) {
		case '-': 
			switch (pchar[1]) {
				case 'f':
					filter_only = true; break;
				case 'h': usage();
				case 'm':
					if (strlen(argv[ix]) > 2) version = argv[ix][2];
					else version = argv[++ ix][0];
					if (version > '2' || version < '0') {
						cerr << "Work mode not available!\n" << endl; usage();
					} break;
				case 't':
					if (strlen(argv[ix]) > 2) comb = argv[ix][2];
					else comb = argv[++ ix][0];
					switch (comb) {
						case '0':
							count_filter = true; deg_match_cond = local_lab_filter = min_edit_filter = false; break;	
						case '1':
							count_filter = min_edit_filter = true; deg_match_cond = local_lab_filter = false; break;
						case '2':
							count_filter = min_edit_filter = local_lab_filter = true; deg_match_cond = false; break;
						case '3':
							count_filter = min_edit_filter = local_lab_filter = deg_match_cond = true; break;
						default:
							cerr << "Filter combination not available!\n" << endl; 
							usage();
					} break;	
				case 'o':
					if (strlen(argv[ix]) > 2) vf_order = argv[ix][2];
					else vf_order = argv[++ ix][0];
					if (vf_order > '2' || vf_order < '0') {
						cerr << "Verification option not available!\n" << endl; 
						usage();
					} break;
				case 'v':
					print_ans = true; 
					print_more = false; break;
				case 'V':
					print_more = true; 
					print_ans = false; break;
			} break; 
		default: 	// options must precede the follows: DB, [Query], q, tau
			strcpy(dfile, pchar);	// data graphs
			if (version > '0') {	// for search queries
				if (++ ix == argc) {
					cerr << "Query file not set!\n" << endl; 
					usage();
				}
				pchar = argv[ix];
				strcpy(qfile, pchar);
			}
			file_set = true;
			if (++ ix < argc) {
				pchar = argv[ix];
				under_qs = atoi(pchar);
				if (under_qs < 1) {
					cerr << "q-gram size non-positive!\n" << endl; 
					usage();
				} else {
					qs = under_qs + 1;
					path_qs = (qs << 1) - 1;
				}
				q_set = true;
			}
			if (++ ix < argc) {
				pchar = argv[ix];
				tau = atoi(pchar);
				if (tau < 0) {
					cerr << "GED threshold negative!\n" << endl; 
					usage();
				} else {
					over_tau = tau + 1;
				}
				tau_set = true;
			} 
			break;
		}	
	}

	if (!file_set) { 
		cerr << "Mandatory argument missing!\n" << endl; 
		usage(); 
	}
	if (!filter_only && vf_order > '0' && !local_lab_filter) {	
		cerr << "Verification option not available under current setting!\n" << endl; 
		usage();
	}
	if (file_set) {
		parse_data(dfile); 
	} else { 
		cerr << "Data file not provided!\n" << endl; 
		usage();
	}
	
	cout << "Setting:";
	switch (version) {
		case '0': 
			cout << " Self-join -"; break;
		case '1': 
			if (strlen(qfile) == 0) { cerr << "The other data file not provided!\n" << endl; usage(); }
			cout << " R-S join -"; break;
		case '2': 
			if (strlen(qfile) == 0) { cerr << "Query file not set!\n" << endl; usage(); }
			cout << " Full-search -"; break;
		default:
			cerr << "No such outlet!\n" << endl;
			usage();
	}
	if (count_filter) cout << " count ";
	if (min_edit_filter) cout << "min-edit ";
	if (local_lab_filter) cout << "label ";
	if (deg_match_cond) cout << "degree ";
	cout << "ft.";  // filters
	
	if (filter_only) cout << " ONLY";
	else {
		switch (vf_order) {
			case '0': cout << " + naive vf."; break;
			case '1': cout << " + improve vf."; break;  // verification
		}
	}

	switch(version) {
		case '0':
			init_mem();
			cout << "\n         Num = " << gdb_size << " Q = " << under_qs << " TAU = " << tau << " DB = " << dfile << endl;
			if (min_edit_filter) {
				run_min_prefix(); 
			} else {
				run_cnt_prefix();
			}
			break;
		case '1':
			load_query(qfile);
			init_mem();
			cout << "\n         Num = " << gdb_size << "|" << qry_size << " Q = " << under_qs << " TAU = " << tau << " R = " << dfile << " S = " << qfile << endl;
			run_rs_join(); break;
		case '2':	
			load_query(qfile);
			init_mem();
			cout << "\n         Num = " << gdb_size << "|" << qry_size << " Q = " << under_qs << " TAU = " << tau << " DB = " << dfile << " Query = " << qfile << endl;
			if (min_edit_filter) {
				run_sim_search(); 
			} else {
				run_cnt_search(); 
			}
			break;
	}
	
	clean();
	return 0;
}
