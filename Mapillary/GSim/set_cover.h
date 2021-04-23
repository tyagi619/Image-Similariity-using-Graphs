#pragma once

#include "common.h"


int threshold_set_cover(const int threshold, int depth, const int start, const unsigned num_remain, bool* coverage, const unsigned size, vector<unsigned>** edset, const unsigned len);
int exact_set_cover(const int threshold, int depth, const int start, const unsigned num_remain, bool* coverage, const unsigned size, vector<unsigned>** edset, const unsigned len);
unsigned update_coverage(bool* coverage, vector<unsigned>* site, unsigned num_remain);
int thresh_ext_err(const unsigned g, const unsigned size);
int mismat_cot_err(const unsigned g, const unsigned h, vector<unsigned>& compo, unsigned* pid, const unsigned pid_size, const int threshold);
int mismat_cot_err(const unsigned g, const unsigned h, set<unsigned>& compo, unsigned* pid, const unsigned pid_size, const int threshold);