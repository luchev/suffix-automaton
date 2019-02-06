#include <iostream>
#include <fstream>
using namespace std;

class SuffixAutomaton {
public:
	SuffixAutomaton(int Size) {
		size = Size;
		input = new char[Size + 1];
		input[Size] = 0;

		length = new int[Size * 2];
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
		length[0] = 0;
		slink[0] = -1;
	}
	void Add(char C) {
		input[stateMainIt] = C;
		stateMainIt++;
		length[stateMainIt] = stateMainIt;
		int u = FindStem(C);
		if (u == -1) {
			slink[stateMainIt] = 0;
			return;
		}
		int v = getTransition(u, C);
		if (length[v] == length[u] + 1) {
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
	// bool Find(string input) {
	//	int state = 0;
	//	for (int i = 0; i < input.size(); i++) {
	//		int next = GetTransition(state, input[i]);
	//		if (next == -1) {
	//			return false;
	//		}
	//		state = next;
	//	}
	//	return true;
	//}
private:
	int size;
	int stateMainIt;	// Points current
	int stateSecIt;		// Points current
	int linkIt;			// Points empty
	char* input;		// x1
	int* start;			// x1
	int* length;		// x2
	int* slink;			// x2
	char* linkedChar;	// x3
	int* linkedIndex;	// x3
	int* linkedNext;	// x3

	int countPrefixVV() {
		int count = 0;
		for (int i = 2; i <= stateMainIt; i += 2) {
			int iter = slink[i];
			while (length[iter] > (i >> 1)) {
				iter = slink[iter];
			}
			if (length[iter] == (i >> 1) && iter < stateMainIt) {
				count++;
			}
		}
		return count;
	}
	int countAltStateVV() {
		int count = 0;
		for (int i = size + 1; i <= stateSecIt; i++) {
			if (length[i] < (size >> 1)) {
				int found = find(getStart(i), length[i], i);
				if (found > 0 && length[found] == (length[i] << 1)) {
					count++;
				}
			}
		}
		return count;
	}
	//void PrintState(int index) {
	//	for (int j = 0; j < length[index]; j++) {
	//		cout << input[j + getStart(index)];
	//	}
	//	cout << endl;
	//}
	//void PrintStateFull(int index) {
	//	cout << "State index: " << index << endl;
	//	cout << "State start: " << getStart(index) << endl;
	//	cout << "State length: " << length[index] << endl;
	//	cout << "State string: ";
	//	for (int j = 0; j < length[index]; j++) {
	//		cout << input[j + getStart(index)];
	//	}
	//	cout << endl;
	//}
	//int getLength(int index) {
	//	if (index <= size)
	//		return index;
	//	return length[index];
	//}
	//int CountAltStateVVOld() {
	//	int count = 0;
	//	for (int i = size + 1; i <= stateSecIt; i++) {
	//		if (length[i] < 2 || length[i] & 1) // Odd number or length 1 = skip
	//			continue;
	//		int iter = slink[i];
	//		while (length[iter] > (length[i] >> 1)) {
	//			iter = slink[iter];
	//		}
	//		if (length[iter] == (length[i] >> 1)){// && !isV[iter]) {
	//			bool increased = false;
	//			increased = find(getStart(iter), length[iter], iter);
	//			if (increased) {
	//				//isV[iter] = true;
	//				count++;
	//			}
	//		}
	//	}
	//	return count;
	//}
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
		start[stateSecIt - size - 1] = stateMainIt - (length[U] + 1);
		length[stateSecIt] = length[U] + 1;
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



	/*
	string input = "bbaacaa";
	SuffixAutomaton S(input.size());
	for (int i = 0; i < input.size(); i++) {
		S.Add(input[i]);
	}
	//cout << S.CountAltStateVV();
	ifstream filesize("alice.txt", ios::binary | ios::ate);
	long long Size = filesize.tellg();
	filesize.close();
	ifstream file("alice.txt");
	char ch;
	SuffixAutomaton S(Size);
	while (file.get(ch)) {
		S.Add(ch);
	}
	cout << S.GetNumberOfStates() << ' ';
	cout << S.GetNumberOfTransitions() << ' ';
	cout << S.GetNumberOfTerminalStates() << ' ';
	cout << S.GetNumberOfVV();
	cout << endl;
	
	TestKos();
	TestCounts();
	TestVV();
	*/

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

void TestKos() {
	for (int i = 0; i < 1; i++) {
		string input = "test100k/";
		input.append(to_string(i));
		input.append(".in");
		ifstream filesize(input, ios::binary | ios::ate);
		long long Size = filesize.tellg();
		filesize.close();
		ifstream file(input);
		char ch;
		SuffixAutomaton S(Size);
		while (file.get(ch)) {
			S.Add(ch);
		}
		string out = "test100k/";
		out.append(to_string(i));
		out.append(".in.out2");
		ifstream outfile(out);
		int num;
		outfile >> num;
		cout << S.GetNumberOfStates() << ' ';
		cout << S.GetNumberOfTransitions() << ' ';
		cout << S.GetNumberOfTerminalStates() << ' ';
		cout << S.GetNumberOfVV() << ' ' << num;
		cout << endl;
	}
}
*/