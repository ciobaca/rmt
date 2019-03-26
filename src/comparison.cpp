#include <iostream>
#include <vector>
#include <cstring>
#include <fstream>
#include "ldeslopesalg.h"
#include "ldegraphalg.h"
#include "ldealg.h"

using namespace std;

int main(int argc, char **argv)
{
  ifstream input("tests.txt");
  int countTests;
  input >> countTests;
  for (int test = 0; test < countTests; ++test) {
    int n, m;
    cout << "Test #" << test << endl;
    input >> n >> m;
    cout << "n = " << n << " m = " << m << endl;
    vector<int> l(n);
    for (auto &x : l) {
      input >> x;
    }
    vector<int> r(m);
    for (auto &x : r) {
      input >> x;
    }
    if (test == atoi(argv[2])) {
      if (strchr(argv[1], 'g')) {
	LDEGraphAlg Alg1(l, r);
	auto ans1 = Alg1.solve();
	cout << "C&F: " << ans1.size() << endl;
      }

      if (strchr(argv[1], 's')) {
	LDESlopesAlg Alg2(l, r);
	auto ans2 = Alg2.solve();
	cout << "Slopes: " << ans2.size() << endl;
      }
    }
  }
  
  return 0;
}

