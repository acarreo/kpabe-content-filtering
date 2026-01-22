/**
 * @file bench.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-09-27
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __BENCH_HPP__
#define __BENCH_HPP__

#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

#include "kpabe.hpp"

typedef struct {
  int nwl;
  int nbl;
  std::string policy;
} policy_params;


// Generate a list of attributes with a given prefix and a number of attributes
std::vector<std::string> generateAttributesList(std::string prefix, int n);

// Generate a string of attributes separated by '|'
std::string generateAttributes(int n, int start = 1);

// Get the number of attributes in a policy
size_t get_number_of_attributes_in_policy(const std::string& policy);

// Print the number of attributes in white list, black list, and policy
void print_number_of_attributes(const std::vector<std::string>& wl,
                                const std::vector<std::string>& bl,
                                const std::string& policy);

static void __relic_print_params() {
  pc_param_print();
  printf("-- Security level: %d bits\n\n", pc_param_level());
}

#endif // __BENCH_HPP__
