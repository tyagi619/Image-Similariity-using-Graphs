#pragma once

//#define UNIX
#define _CRT_SECURE_NO_WARNINGS

// libraries
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <iterator>
#include <algorithm>

#ifdef _DEBUG
#include <cassert>
#endif

#ifdef UNIX
#include <sys/time.h>
#include <google/dense_hash_map>
#else
#include <map>
#include <ctime>
#pragma  warning(disable: 4018)
#endif

// namespaces
#ifdef UNIX
using google::dense_hash_map;
using __gnu_cxx::hash;
#endif
using namespace std;

// macros
#ifdef UNIX
#define fmap dense_hash_map<unsigned, int>
#define gmap dense_hash_map<unsigned, set<unsigned> >
#define iter_graph dense_hash_map<unsigned, set<unsigned> >::iterator
#define const_iter_graph dense_hash_map<unsigned, set<unsigned> >::const_iterator
#else
#define fmap map<const unsigned, int> 
#define gmap map<const unsigned, set<unsigned> >
#define iter_graph map<const unsigned, set<unsigned> >::iterator 
#define const_iter_graph map<const unsigned, set<unsigned> >::const_iterator
#define INFINITY OVER_BOUND
#endif

#define UNMAPPED OVER_BOUND
const unsigned OVER_BOUND = 0x7FFFFFFF;
const int DUMMY_LABEL = -1;
const int KEY_INITIAL = 10000019;
const int PRIME_FACTOR = 199;
const unsigned LAG_SIZE = 1000;
const unsigned QUE_SIZE = 100;

extern int tau;
extern int over_tau;
extern int qs;
extern int path_qs;
extern int under_qs;
extern bool* done;