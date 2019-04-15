#include <iostream>
#include <fstream>
using namespace std;

class SuffixAutomaton {
public:
	SuffixAutomaton(int Size) {
		size = Size;
		input = new char[Size + 1];
		input[Size] = 0;

		length = new int[Size];
		slink = new int[Size * 2];

		linkedChar = new char[Size * 3];
		for (int i = 0; i < Size * 3; i++) { linkedChar[i] = 0; }
		linkedIndex = new int[Size * 3];
		linkedNext = new int[Size * 3];
		for (int i = 0; i < Size * 3; i++) { linkedNext[i] = -1; }
		start = new int[Size];

		linkIt = Size * 3 - 1;
		stateSecIt = Size;
		stateMainIt = 0;
		slink[0] = -1;
	}
	void Add(char C) {
		input[stateMainIt] = C;
		stateMainIt++;
		int u = FindStem(C);
		if (u == -1) {
			slink[stateMainIt] = 0;
			return;
		}
		int v = getTransition(u, C);
		if (getLength(v) == getLength(u) + 1) {
			slink[stateMainIt] = v;
			return;
		}
		int s = NewSlink(u, C);
		Redirect(u, s, C);
	}
	int GetNumberOfStates() {
		return stateSecIt + 1;
	}
	int GetNumberOfTerminalStates() {
		int counter = 1;
		int index = stateMainIt;
		while (slink[index] > -1) {
			counter++;
			index = slink[index];
		}
		return counter;
	}
	int GetNumberOfTransitions() {
		return size * 3 - 1 - linkIt + stateSecIt;
	}
	int GetNumberOfVV() {
		return countPrefixVV() + countAltStateVV() + 1;
	}
	bool Find(string input) {
		int state = 0;
		for (int i = 0; i < input.size(); i++) {
			int next = getTransition(state, input[i]);
			if (next == -1) {
				return false;
			}
			state = next;
		}
		return true;
	}
private:
	int size;			// Length of string
	int stateMainIt;	// Points current
	int stateSecIt;		// Points current
	int linkIt;			// Points empty
	char* input;		// x1
	int* start;			// x1
	int* length;		// x1
	int* slink;			// x2
	char* linkedChar;	// x3
	int* linkedIndex;	// x3
	int* linkedNext;	// x3

	int countPrefixVV() {
		int count = 0;
		for (int i = 2; i <= stateMainIt; i += 2) {
			int iter = slink[i];
			while (getLength(iter) > (i >> 1)) {
				iter = slink[iter];
			}
			if (getLength(iter) == (i >> 1) && iter < stateMainIt) {
				count++;
			}
		}
		return count;
	}
	int countAltStateVV() {
		int count = 0;
		for (int i = size + 1; i <= stateSecIt; i++) {
			if (getLength(i) < (size >> 1)) {
				int found = find(getStart(i), getLength(i), i);
				if (found > 0 && getLength(found) == (getLength(i) << 1)) {
					count++;
				}
			}
		}
		return count;
	}
	int getLength(int index) {
		if (index <= size)
			return index;
		return length[index - size];
	}
	int find(int PosFrom, int Length, int StartState) {
		for (; Length > 0; Length--) {
			int next = getTransition(StartState, input[PosFrom]);
			if (next == -1) {
				return false;
			}
			StartState = next;
			PosFrom++;
		}
		return StartState;
	}
	int getStart(int state) {
		if (state == 0)
			return -1;
		else if (state <= size)
			return 0;
		else
			return start[state - size - 1];
	}
	int FindStem(char C) {
		int u = stateMainIt - 1;
		while (u != -1 && getTransition(u, C) == -1) {
			setTransition(u, C, stateMainIt);
			u = slink[u];
		}
		return u;
	}
	int NewSlink(int U, char C) {
		int v = getTransition(U, C);
		stateSecIt++;
		start[stateSecIt - size - 1] = stateMainIt - (getLength(U) + 1);
		length[stateSecIt - size] = getLength(U) + 1;
		slink[stateSecIt] = slink[v];
		slink[v] = stateSecIt;
		slink[stateMainIt] = stateSecIt;
		copyTransitions(v, stateSecIt);
		return stateSecIt;
	}
	void Redirect(int U, int S, char C) {
		int v = getTransition(U, C);
		while (U != -1 && getTransition(U, C) == v) {
			setTransition(U, C, S);
			U = slink[U];
		}
	}
	void copyTransitions(int From, int To) {
		linkedChar[To] = linkedChar[From];
		linkedIndex[To] = linkedIndex[From];
		From = linkedNext[From];
		while (From != -1) {
			linkedNext[To] = linkIt;
			To = linkIt;
			linkedChar[To] = linkedChar[From];
			linkedIndex[To] = linkedIndex[From];
			From = linkedNext[From];
			linkIt--;
		}
	}
	void setTransition(int From, char With, int To) {
		if (linkedChar[From] == 0) {
			linkedChar[From] = With;
			linkedIndex[From] = To;
			return;
		}
		while (linkedChar[From] < With) {
			if (linkedNext[From] != -1) {
				From = linkedNext[From];
			}
			else {
				linkedNext[From] = linkIt;
				linkedChar[linkIt] = With;
				linkedIndex[linkIt] = To;
				linkIt--;
				return;
			}
		}
		if (linkedChar[From] == With) {
			linkedIndex[From] = To;
		}
		else {
			linkedChar[linkIt] = linkedChar[From];
			linkedIndex[linkIt] = linkedIndex[From];
			linkedNext[linkIt] = linkedNext[From];
			linkedNext[From] = linkIt;
			linkedChar[From] = With;
			linkedIndex[From] = To;
			linkIt--;
		}

	}
	int getTransition(int From, char With) {
		while (linkedChar[From] < With) {
			if (linkedNext[From] != -1)
				From = linkedNext[From];
			else
				return -1;
		}

		if (linkedChar[From] == With)
			return linkedIndex[From];
		else
			return -1;
	}
};

/*

void TestString(const string& input, int states, int links, int terminals) {
	SuffixAutomaton S(input.size());
	for (int i = 0; i < input.size(); i++) {
		S.Add(input[i]);
	}
	cout << (states == S.GetNumberOfStates());
	cout << (links == S.GetNumberOfTransitions());
	cout << (terminals == S.GetNumberOfTerminalStates());
}

void TestCounts() {
	TestString("", 1, 0, 1);
	TestString("b", 2, 1, 2);
	TestString("aa", 3, 2, 3);
	TestString("abcbc", 8, 9, 3);
	TestString("bbacbba", 8, 10, 3);
	TestString("bbacbbaa", 10, 14, 3);
	TestString("cpalgorithmspageauthorssuffixautomatontableofcontentsdefinition", 88, 144, 4);
	TestString("abcbcbcbcbcbcbcbc", 32, 33, 9);
}

bool TestForVV(string input, int vv) {
	vv;
	SuffixAutomaton S(input.size());
	for (int i = 0; i < input.size(); i++) {
		S.Add(input[i]);
	}
	return (S.GetNumberOfVV() == vv);
}

void TestVV() {
	cout << TestForVV("abcbcbcbcefefefef", 3);
	cout << TestForVV("cacaac", 2);
	cout << TestForVV("bbaacaa", 3);
	cout << TestForVV("bcbcc", 2);
	cout << TestForVV("bcbcbcbc", 3);
	cout << TestForVV("abcabc", 2);
	cout << TestForVV("abbcbb", 2);
	cout << TestForVV("abbb", 2);
	cout << TestForVV("abcbcbcbc", 2);
	cout << TestForVV("abcdbcdebcdbcd", 2);
	cout << TestForVV("abbbbb", 3);
	cout << TestForVV("ebcbcbabc", 1);
	cout << TestForVV("ebcbcbcbcebabcbabc", 2);
	cout << TestForVV("", 1);
	cout << TestForVV("baacaaddeddeddedd", 4);
}

*/

int main(int argc, char* argv[]) {
	std::ios_base::sync_with_stdio(false);

	if (argc < 2) {
		cout << "No file provided. Terminating program.";
		return 1;
	}
	ifstream filesize(argv[1], ios::binary | ios::ate);
	int Size = filesize.tellg();
	filesize.close();
	ifstream file(argv[1]);
	if (!file.is_open()) {
		cout << "Failed to open file. Terminating program.";
		return 2;
	}
	char ch;
	int i = 0;
	SuffixAutomaton S(Size);
	while (file.get(ch)) {
		S.Add(ch);
	}
	cout << S.GetNumberOfStates() << ' ';
	cout << S.GetNumberOfTransitions() << ' ';
	cout << S.GetNumberOfTerminalStates() << ' ';
	cout << S.GetNumberOfVV();

}
