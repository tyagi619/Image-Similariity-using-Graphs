#include "misc.h"


unsigned bitwise_hash( char* key, const int size )
{
	unsigned h = 1159241;
	int i = 0;
	while (i != size) {
		h ^= (h << 5) + (h >> 2) + key[i ++];
	}
	return h & 0x7fffffff;
}

bool* init_bool_array( const bool val, const unsigned size )
{
	bool* p = new bool[size];
	for (unsigned i = 0; i != size; ++ i){
		p[i] = val;
	}
	return p;
}

unsigned bin_search( unsigned* array, const unsigned size, const unsigned target )
{	
	if (array[0] > target) return size;
	if (array[size-1] < target) return size;
	unsigned left = 0, right = size, mid;
	while (left < right) {
		mid = (left + right) >> 1;
		array[mid] <= target? left = mid + 1: right = mid;
	}
	return (array[-- left] == target? left: size);
}

unsigned bin_search( vector<int>& array, const int target )
{
	if (array.front() > target) return array.size();
	if (array.back() < target) return array.size();
	unsigned left = 0, right = array.size(), mid;
	while (left < right) {
		mid = (left + right) >> 1;
		array[mid] <= target? left = mid + 1: right = mid;
	}
	return (array[-- left] == target? left: array.size());
}

unsigned bin_search( vector<unsigned>* array, const unsigned start, const unsigned target )
{
	int left = start, right = array->size() - 1, mid;
	while (left <= right) {
		mid = (left + right) >> 1;
		if ((*array)[mid] < target) {
			left = mid + 1;	
		} else if ((*array)[mid] > target) {
			right = mid - 1;
		} else {
			return mid;
		}
	}
	return array->size();
}

unsigned bin_search( vector<int>& array, const unsigned start, const int target )
{
	unsigned left = start, right = array.size(), mid;
	while (left < right) {
		mid = (left + right) >> 1;
		if (array[mid] <= target) {
			left = mid + 1;
		} else {
			right = mid;
		}
	}
	return left;
}


bool normalize_string( char* chars, int right, bool& backward )
{
	int left = 0;
	bool normal = true;
	-- right;
	while (right > left) {
		if (chars[left] == chars[right]) {
			++ left, -- right;
		} else if (chars[left] > chars[right]) { 
			normal = false;
			backward = true;
			break;
		} else return false;
	}
	char temp;
	while (right > left) {
		temp = chars[left];
		chars[left] = chars[right];
		chars[right] = temp;
		++ left, -- right;
	}
	return normal;
}

void merge_vertex_set( unsigned* vpath, set<unsigned>& s )
{
	for (unsigned i = 0; i != qs; ++ i) {
		s.insert(vpath[i]);
	}
}

void reduce_vec( vector<unsigned>** st, unsigned& size, const unsigned bit_size )
{
	sort(st, st + size, Container_comp()); 
	unsigned long long** bitv = new unsigned long long*[size];
	for (unsigned i = 0; i != size; ++ i) {
		bitv[i] = new unsigned long long[bit_size];
		memset(bitv[i], 0, bit_size*sizeof(unsigned long long));
		for (unsigned k = st[i]->size(), j = 0; j != k; ++ j) {
			bitv[i][(*st[i])[j] >> 5] |= (1ULL << ((*st[i])[j] & ((2 << 5) - 1)));
		}
	}
	
	bool* contained = new bool[size];
	memset(contained, 0, size*sizeof(bool));
	int i = 0, j;
	for (; i != size; ++ i) {
		if (contained[i]) continue;

		for (j = i + 1; j != size; ++ j) {
			if (contained[j]) continue;

			if (bit_vec_containment(bitv[j], bitv[i], bit_size)) {
				contained[j] = true;
			}
		}
	}

	for (i = 0; i != size; ++ i) {
		delete[] bitv[i];
	}
	delete[] bitv;

	i = 0, j = size - 1;
	vector<unsigned>* temp;
	while (i < j) {
		if (contained[i]) {
			if (!contained[j]) {
				temp = st[i];
				st[i] = st[j];
				st[j] = temp;
				++ i, -- j;
			} else {
				-- j;
			}
		} else {
			++ i;
		}
	}
	if (i == j && !contained[i]) {
		size = ++ i;
	} else {
		size = i;
	}

	sort(st, st + size, Container_comp());
	delete[] contained;
}

void reduce_vec( vector<unsigned>** st, unsigned& size )
{
	sort(st, st + size, Container_comp());
	bool* contained = new bool[size];
	memset(contained, 0, sizeof(bool)*size);
	int i = 0, j;
	for (; i != size; ++ i) {
		if (contained[i]) continue;

		for (j = i + 1; j != size; ++ j) {
			if (contained[j]) continue;

			if (vec_containment(st[j], st[i])) {
				contained[j] = true;
			}
		}
	}
	i = 0, j = size-1;
	vector<unsigned>* temp;
	while (i < j) {
		if (contained[i]) {
			if (!contained[j]) {
				temp = st[i];
				st[i] = st[j];
				st[j] = temp;
				++ i, -- j;
			} else {
				-- j;
			}
		} else {
			++ i;
		}
	}
	if (i == j && !contained[i]) {
		size = ++ i;
	} else {
		size = i;
	}

	sort(st, st + size, Container_comp());
	delete[] contained;
}

void copy_vec( set<unsigned>& src, vector<unsigned>* dst, const unsigned size )
{
	for (set<unsigned>::const_iterator it = src.begin(); it != src.end(); ++ it) {
		if (*it < size) {
			dst->push_back(*it);
		} else return;
	}
}

void copy_vec( set<unsigned>& src, vector<unsigned>* dst, unsigned* pid, const unsigned size )
{
	set<unsigned>::const_iterator it = src.begin();
	unsigned i = 0;
	while (it != src.end() && i != size) {
		if (*it == pid[i]) {
			dst->push_back(i);
			++ i, ++ it;
		} else if (*it < pid[i]) {
			++ it;
		} else {
			++ i;
		}
	}
}

bool vec_containment( vector<unsigned>* lhs, vector<unsigned>* rhs )
{
	unsigned last = 0;
	for (unsigned j = lhs->size(), i = 0; i != j; ++ i) {
		last = bin_search(rhs, last, (*lhs)[i]);
		if (last == rhs->size()) return false;
	}
	return true;
}

bool bit_vec_containment( unsigned long long* lhs, unsigned long long* rhs, const unsigned bit_size )
{
	for (unsigned i = 0; i != bit_size; ++ i) {
		if ((lhs[i] & rhs[i]) != lhs[i]) return false;
	}
	return true;
}

unsigned in_list( unsigned* list, const unsigned len, const unsigned target )
{
	for (unsigned i = 0; i != len; ++ i) {
		if (list[i] == target) {
			return i;
		}
	}
	return len;
}

unsigned in_list( vector<unsigned>& list, const unsigned target )
{
	for (unsigned i = 0, j = list.size(); i != j; ++ i) {
		if (list[i] == target) {
			return i;
		}
	}
	return list.size();
}