#include <iostream>
#include <vector>
#include <random>
#include "fastterm.h"
using namespace std;

vector<int> vals;

FastSubst testFastSubst() {
  FastSubst ans;
  for (int i = 0; i < (int)vals.size(); i += 2) {
    ans.addToSubst(vals[i], vals[i + 1]);
  }
  return ans;
}

vector<unsigned int> testVector() {
  vector<unsigned int> ans;
  for (int i = 0; i < (int)vals.size(); ++i) {
    ans.push_back(vals[i]);
  }
  return ans;
}

vector<pair<unsigned int, unsigned int>> testPairVector() {
  vector<pair<unsigned int, unsigned int>> ans;
  for (int i = 0; i < (int)vals.size(); i += 2) {
    ans.emplace_back(vals[i], vals[i + 1]);
  }
  return ans;
}

bool compareVectors(FastSubst &fs, vector<unsigned int> &v) {
  if (fs.count != v.size()) return false;
  for (int i = 0; i < (int)fs.count; ++i) if (fs.data[i] != v[i]) return false;
  return true;
}

bool compareVectors(FastSubst &fs, vector<pair<unsigned int, unsigned int>> &v) {
  if (fs.count != 2 * v.size()) return false;
  for (int i = 0; i < (int)fs.count; i += 2) if (fs.data[i] != v[i / 2].first || fs.data[i + 1] != v[i / 2].second) return false;
  return true;
}

int main() {
  #define rand() dis(gen)
  mt19937 gen(chrono::steady_clock::now().time_since_epoch().count());
  uniform_int_distribution<int> dis(0, 1e4);
  
  const int N = 1e7;
  double fst = 0, vt = 0, pvt = 0;
  for (int cnt = 0; cnt <= 50; ++cnt) {
    vals.clear();
    for (int i = 0; i < N; ++i) vals.push_back(rand());

    chrono::high_resolution_clock::time_point t1, t2;
    t1 = chrono::high_resolution_clock::now();
    FastSubst fs = testFastSubst();
    t2 = chrono::high_resolution_clock::now();
    fst += chrono::duration_cast<chrono::microseconds>(t2 - t1).count() / 1e6;

    t1 = chrono::high_resolution_clock::now();
    auto v = testVector();
    t2 = chrono::high_resolution_clock::now();
    vt += chrono::duration_cast<chrono::microseconds>(t2 - t1).count() / 1e6;

    t1 = chrono::high_resolution_clock::now();
    auto pv = testPairVector();
    t2 = chrono::high_resolution_clock::now();
    pvt += chrono::duration_cast<chrono::microseconds>(t2 - t1).count() / 1e6;

    if (!compareVectors(fs, v)) {
      cout << "TRIST v! " << cnt << '\n';
    }
    if (!compareVectors(fs, pv)) {
      cout << "TRIST pv! " << cnt << '\n';
    }
  }
  cout << fst << '\n';
  cout << vt << '\n';
  cout << pvt << '\n';
  return 0;
}
