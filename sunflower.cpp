#include <bits/stdc++.h>
using namespace std;

const int N = int(1e6);

// n C r
int choose (int n, int r) {
	int ans = 1;
	for (int i = r + 1; i <= n; ++i)
		ans *= i;
	for (int i = n - r; i >= 2; --i)
		ans /= i;
	return ans;
}

// returns iff True if |F| > d*d!*(k - 1)^d
bool premise (int d, int k, int modf) {
	int ans = d;
	int cpy = d;
	while (d) {
		if (modf <= ans) {
			return false;
		}
		ans *= d--;
	}
	d = cpy;
	for (int i = 1; i <= d; ++i) {
		if (modf <= ans) {
			return false;
		}
		ans *= (k - 1);
	}
	return modf > ans;
}


vector <vector <int> > subsets;
vector <vector <int> > subsetIndices;

// takes input of indices in 'subsets' directly
bool sunflower (vector <int> &subs) {
	int tot = subs.size();
	vector <int> core;

	for (int j = 0; j < tot; ++j) {
		for (int k = j + 1; k < tot; ++k) {
			vector <int> intersect;
			for (int el1 : subsets[subs[j]])
				for (int el2 : subsets[subs[k]])
					if (el1 == el2)
						intersect.push_back(el1);
			if (0 == j and 1 == k) {
				for (int el : intersect)
					core.push_back(el);
			}
			else {
				vector <int> hashOld(core.size(), 0);
				for (int newel : intersect) {
					bool safe = false;
					for (int i = 0; i < core.size(); ++i) {
						int old = core[i];
						if (old == newel) {
							safe = true;
							hashOld[i] = 1;
							break;
						}
					}
					// an element in new intersection not in older one, not a sunflower!!
					if (!safe) {
						// cout << newel << " new, not a sunflower\n";
						return false;
					}
				}
				int sz = int(core.size());
				for (int i = 0; i < sz; ++i) {
					if (0 == hashOld[i]) {
						// an extra element in intersection of two sets but NOT in intersection of others
						// petals should be disjoint, only core should be the intersection
						return false;
					}
				}
			}
		}
	}
	int coresz = core.size();
	// CAUTION : petals must be NON-EMPTY. Check sizes now that we know a core exists
	for (int idx : subs) {
		if (subsets[idx].size() == coresz)
			return false;		// petal CANNOT be EMPTY
	}
	cout << "core :\n";
	for (int c : core)
		cout << c << " ";
	cout << "\n";
	return true;
}

int main () {
	int n, d, m;
	cout << "Enter n (number of elements in U) and d\n";
	cin >> n >> d;
	cout << "Enter m (number of subsets of U in F)\n";
	int total = 0;
	for (int dd = 0; dd <= d; ++dd)
		total += choose(n, dd);
	cout << "Number of (<= d)-sized subsets of " << n << " is " << total << "\n";
	cin >> m;

	// computing k
	int low = 1, high = N, mid;		
	while (low <= high) {
		mid = (low + high + 1)/2;
		if (low == mid) break;
		if (premise(d, mid, m)) {
			low = mid;
		}
		else {
			high = mid - 1;
		}
		assert(high >= low);
	}
	// ans is at low
	int k = low;
	cout << "biggest k := " << k << "\n";

	int choice;
	do {
		cout << "Enter 0 for randomly choosing " << m << " d-sized subsets from U\n";
		cout << "Enter 1 for entering the family\n";

		cin >> choice;

		subsets.assign(m, vector <int> ());
		subsetIndices.assign(n + 1, vector <int> ());

		if (0 == choice) {
			vector <int> dsized;
			for (int i = 1; i < (1 << n); ++i) {
				int set = 0;
				for (int bit = 0; bit < n; ++bit) {
					if (i & (1 << bit))
						++set;
					if (set > d) break;
				}
				if (set <= d)
					dsized.push_back(i);
			}
			// http://www.cplusplus.com/reference/algorithm/shuffle/
			// obtain a time-based seed:
			unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

			shuffle (dsized.begin(), dsized.end(), std::default_random_engine(seed));

			cout << "Subsets generated :\n";
			for (int i = 0; i < m; ++i) {
				cout << "subset " << i << ":: ";
				int num = dsized[i];
				for (int bit = 0; bit < n; ++bit) {
					if (num & (1 << bit)) {
						subsets[i].push_back(bit + 1);
						subsetIndices[bit + 1].push_back(i);
						cout << bit + 1 << " ";
					}
				}
				cout << "\n\n";
			}
		}
		else if (1 == choice) {
			for (int i = 0; i < m; ++i) {
				cout << "Enter elements of the " << i + 1 << "th subset in F\n";
				if (!i)
					cout << "Enter a 0 when done for the current set\n";
				int x;
				cin >> x;
				while (x != 0) {
					subsets[i].push_back(x);
					subsetIndices[x].push_back(i);
					cin >> x;
				}
			}
		}
		else {
			cout << "Wrong input. Enter again\n";
			continue;
		}

		cout << "Computing a sunflower with " << k << " petals\n";
		cout << "Computing G, a maximal set of disjoint subsets from F\n";

		int seen[n];
		memset(seen, 0, sizeof(seen));

		set <int> g;	// indices from F which are in G
		g.insert(0);
		// iterating over all subsets in F
		int gtotal = subsets[0].size();
		for (int i = 1; i < m; ++i) {
			bool take = true;
			for (int el : subsets[i]) {
				for (int taken : g) {
					for (int present : subsets[taken]) {
						if (present == el) {
							take = false; break;
						}
					}
					if (!take) break;
				}
				if (!take) break;
			}
			if (take) {
				g.insert(i);
				gtotal += subsets[i].size();
			}
		}
		// got G
		cout << "Indices of subsets from F which are in G:\n";
		for (int i : g)
			cout << i << " ";
		cout << "\n Number of subsets each element is present in ::\n";

		for (int elem = 1; elem <= n; ++elem)
			cout << subsetIndices[elem].size() << " ";
		cout << "\n";

		for (int elem = 1; elem <= n; ++elem)
			cout << elem << " ";
		cout << "\n";

		if (g.size() >= k)
			cout << "done, take " << k << " subsets from G\n";
		else {
			int f_by_s = m/gtotal;

			int u[n + 1];
			for (int i = 0; i <= n; ++i)
				u[i] = i;
			// http://www.cplusplus.com/reference/algorithm/shuffle/
			// obtain a time-based seed:
			unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
			shuffle (u, u + n + 1, std::default_random_engine(seed));

			for (int ii = 0; ii <= n; ++ii) {
				int i = u[ii];
				if (subsetIndices[i].size() >= f_by_s) {
					cout << i << " is the 'u' in the proof!\n";
					cout << f_by_s << " needed. Degree of u is " << subsetIndices[i].size() << "\n";
					int haveU = subsetIndices[i].size();
					for (int r = (1 << haveU) - 1; r >= 1; --r) {
						int set = 0;
						vector <int> on;
						for (int bit = 0; bit < haveU; ++bit) {
							if (r & (1 << bit)) {
								on.push_back(bit);
								++set;
							}
						} // set = on.size()
						if (set >= k) {
							bool fine = true;
							vector <int> input;
							for (int place : on) {
								int realIndex = subsetIndices[i][place];
								input.push_back(realIndex);
							}
							if (sunflower(input)) {
								cout << set << " petals found\n";
								for (int place : on) {
									int realIndex = subsetIndices[i][place];
									for (int el : subsets[realIndex])
										cout << el << " ";
									cout << "\n";
								}
								break;
							}
						}
					}
					break;
				}
			}
		}
	} while (!(choice == 0 or choice == 1));
	return 0;
}

