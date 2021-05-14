#include <bits/stdc++.h>
using namespace std;

int n, d, m, k;
const int N = int(1e6);
int u[N];

int choose (int n, int r) {			// n C r
	int ans = 1;
	for (int i = r + 1; i <= n; ++i)
		ans *= i;
	for (int i = n - r; i >= 2; --i)
		ans /= i;
	return ans;
}

bool premise (int d, int k, int modf) { 	// returns iff True if |F| > d!*(k - 1)^d
	int ans = 1;
	int cpy = d;
	while (d) {
		if (modf <= ans)
			return false;
		ans *= d--;
	}
	d = cpy;
	for (int i = 1; i <= d; ++i) {
		if (modf <= ans)
			return false;
		ans *= (k - 1);
	}
	return modf > ans;
}

vector <vector <int> > subsets;
vector <vector <int> > subsetIndices;

void disjoint (
		const vector <int> &setIndices,		// indices from 'subsets' to be checked
		vector <int> &indices,			// o/p indices pushed in this vector (allocated by caller)
		const vector <bool> &excluded,		// excluded elems NOT to be checked for checking disjoint
		int &total) {				// number of elems in the pairwise disjoint sets

	vector <int> g;					// indices from F which are in G
	total = 0;
	if (setIndices.size() == 0)
		return;
	set <int> ind;
	for (int index : setIndices) {
		assert(index >= 0 and index < m);
		ind.insert(index);
	}
	assert(ind.size() == setIndices.size());

	g.push_back(setIndices[0]);		// first set taken
						// iterating over all subset indices represented by setIndices
	for (int fi : subsets[setIndices[0]])	// NOT to count excluded elements
		total += excluded[fi] ? 0 : 1;
	for (int i : setIndices) {
		bool take = true;
		int reduce = 0;
		for (int el : subsets[i]) {
			if (excluded[el]) {
				++reduce;			// el to be ignored
				continue;
			}
			for (int takenIndices : g) {
				for (int present : subsets[takenIndices]) {
					if (excluded[present]) continue;	// to be ignored
					if (present == el) {
						take = false; break;
					}
				}
				if (!take) break;
			}
			if (!take) break;
		}
		if (take) {
			g.push_back(i);
			total += (int(subsets[i].size()) - reduce);
		}
	}
	ind.clear();
	cout << "\nIndices of disjoint subsets \n";		// got G
	for (int i : g) {
		cout << i << " ";
		indices.push_back(i);
		ind.insert(i);
	}
	cout << total << " (total) \n";
	assert(ind.size() == indices.size());
}

// before calling the function, subsetIndices is computed!
void sunflower (
		const vector <int> &setIndices,			// indices to be checked
		vector <bool> &excluded,			// elements to be excluded are true
		const int k,					// k in sunflower lemma expression, same throughout
		int f,						// number of families in the input
		vector <int> &sunflowerIndices) {		// o/p indices pushed here, allocated in 'main'

	assert(setIndices.size() == f);
	if (setIndices.size() == 0)
		return;
	int s = 0;
	cout << "INPUT to sunflower " << k << " " << f << "\n";
	for (int ip : setIndices)
		cout << ip << " ";
	cout << "\n";
	vector <int> indices;					// trying to find disjoint subsets from F
	disjoint (setIndices, indices, excluded, s);
	if (int(indices.size()) >= k)				// already a sunflower with >=k petals
		for (int i : indices)
			sunflowerIndices.push_back(i);
	else {
		cout << "Number of subsets each element is present in ::\n";

		for (int elem = 1; elem <= n; ++elem)
			cout << subsetIndices[elem].size() << " ";	// set initially in 'main'
		cout << "\n";

		for (int elem = 1; elem <= n; ++elem)
			cout << elem << " ";
		cout << "\n";

		for (int i = 0; i < n; u[i] = ++i);

		// http://www.cplusplus.com/reference/algorithm/shuffle/
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		shuffle (u, u + n, std::default_random_engine(seed));

		/* was easier to find max element, but we insist on going with randomness
		int maxdeg = 0, element = -1;
		for (int ii = 0; ii < n; ++ii) {
			int i = u[ii];
			if (excluded[i]) continue;			// ignore this element

			if (subsetIndices[i].size() > maxdeg) {
				maxdeg = subsetIndices[i].size();
				element = i;
			}
		}

		assert(element != -1);
			// best to choose maximum degree element
		*/

		int l = int(indices.size());
		int needed = (f + s - l)/s;				// precise calculation of minimizing the max
									// number of subsets an element can be in
		cout << f << " " << s << " Needed deg = " << needed << "\n";

		for (int ii = 0; ii < n; ++ii) {
			int i = u[ii];
			if (excluded[i]) continue;			// ignore this element
			if (subsetIndices[i].size() >= needed) {	// a bit-risky
				cout << "\nu = " << i << "\n";
				excluded[i] = true;			// exclude this element
				// fix subsetIndices
				// remember to store next set indices to iterate on
				vector <int> setsContainingI;
				for (int idx : subsetIndices[i])
					setsContainingI.push_back(idx);

				for (int i = 0; i <= n; ++i)		// now clear subsetIndices
					subsetIndices[i].clear();
				int new_f = 0;
				for (auto it = setsContainingI.begin(); it != setsContainingI.end(); ++it) {
					int idx = *it;
					bool taken = false;		// a fully excluded element set WON'T be taken 
					for (int elem : subsets[idx]) {
						if (excluded[elem]) continue;
						subsetIndices[elem].push_back(idx);
						taken = true;
					}
					if (taken)
						++new_f;
					else 					// else remove 
						setsContainingI.erase(it--);
				}
				sunflower(setsContainingI, excluded, k, new_f, sunflowerIndices);
				break;
			}
		}
	}
}

int main () {
	cout << "Enter n (number of elements in U) and d\n";
	cin >> n >> d;
	cout << "Enter m (number of subsets of U in F)\n";
	cout << "Remember " << n << " choose " << d << " = " << choose(n, d) << "\n";
	cin >> m;

	int low = 1, high = N, mid;		// computing k using binary search
	while (low <= high) {
		mid = (low + high + 1)/2;
		if (low == mid) break;
		if (premise(d, mid, m))
			low = mid;
		else
			high = mid - 1;
		assert(high >= low);
	}
	k = low;				// ans is at low as low is always in the solution
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
			for (int i = 0; i < (1 << n); ++i) {
				int set = 0;
				for (int bit = 0; bit < n; ++bit) {
					if (i & (1 << bit))
						++set;
					if (set > d) break;
				}
				if (set == d)
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

		vector <int> sunflowerIndices;
		vector <bool> excluded(n + 1, false);
		vector <int> subs;
		for (int i = 0; i < m; ++i)
			subs.push_back(i);

		sunflower(subs, excluded, k, m, sunflowerIndices);

		if (sunflowerIndices.size() > 1) {
			cout << "\nCore : \n";
			for (int el1 : subsets[sunflowerIndices[0]])
				for (int el2 : subsets[sunflowerIndices[1]])
					if (el1 == el2)
						cout << el1 << " ";
			cout << "\n";
		}
		cout << sunflowerIndices.size() << "-sized sunflower\n";
		set <int> ind;
		for (int i : sunflowerIndices) {
			ind.insert(i);
			for (int elem : subsets[i])
				cout << elem << " ";
			cout << "\n";
		}
		assert(ind.size() == sunflowerIndices.size());
		for (bool b : excluded)
			b ? cout << "1 " : cout << "0 ";
		cout << "\n";

	} while (!(choice == 0 or choice == 1));
	return 0;
}
