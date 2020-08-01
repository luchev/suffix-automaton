# Suffix automaton using the minimum possible memory

Memory efficient Suffix automaton in c++, which allocates 500MB~ memory per 10 million character string.

## How to use
```c++
  	std::string fileName = "MyFile.txt";
	
// First find file size (how many characters it has)
  	ifstream filesizeCheck(fileName, ios::binary | ios::ate);
	int size = filesizeCheck.tellg();
	filesizeCheck.close();
  
  // Read the file
	ifstream file(fileName);
	if (!file.is_open()) {
		cout << "Failed to open file. Terminating program.";
		return 2;
	}
  
  // Create the suffix automaton with fixed size
	SuffixAutomaton S(Size);
  
  // Read each character and add it to the automaton
  	char ch;
	while (file.get(ch)) {
		S.Add(ch);
	}
  
  // Get automaton status
	cout << S.GetNumberOfStates() << ' ';
	cout << S.GetNumberOfTransitions() << ' ';
	cout << S.GetNumberOfTerminalStates() << ' ';
	cout << S.GetNumberOfVV();
		
```
