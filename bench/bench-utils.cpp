#include "kpabe.hpp"

using namespace std;

vector<string> generateAttributesList(string prefix, int n) {
  vector<string> list;
  for (int i = 1; i <= n; i++) {
    list.push_back(prefix + to_string(i));
  }
  return list;
}

string generateAttributes(int n, int start = 1) {
  string attributes;
  for (int i = start; i < n+start; i++) {
    attributes += "Attr_" + to_string(i) + "|";
  }
  return attributes.substr(0, attributes.size() - 1);
}

size_t get_number_of_attributes_in_policy(const string& policy) {
  auto pol_tree = createPolicyTree(policy);
  auto attr_set = pol_tree->getAttrCompleteSet();

  return attr_set.size();
}

void print_number_of_attributes(const vector<string>& wl, const vector<string>& bl, const string& policy) {
  cout << "Number of attributes in White list ---> " << wl.size() << endl;
  cout << "Number of attributes in Black list ---> " << bl.size() << endl;
  cout << "Number of attributes in Policy -------> " << get_number_of_attributes_in_policy(policy) << endl;
}

