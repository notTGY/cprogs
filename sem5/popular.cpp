#include <iostream>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <cctype>
#include <iomanip>


using namespace std;

string lower_str(string s) {
  std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){return tolower(c);});
  return s;
}

int main() {
    int n;
    cin >> n;

    unordered_map<string, int> hashTable;

    for (int i = 0; i < n; i++) {
        string word;
        cin >> word;
        // convert word to lowercase
        word = lower_str(word);

        if (hashTable.find(word) == hashTable.end()) {
            hashTable[word] = 1;
        } else {
            hashTable[word]++;
        }
    }

    auto it = hashTable.begin();
    int maxCount = 0;
    string mostPopularWord;

    for (const auto& [key, value] : hashTable) {
        if (value > maxCount) {
            maxCount = value;
            mostPopularWord = key;
        }
    }

    cout << mostPopularWord << " " << maxCount << endl;

    return 0;
}