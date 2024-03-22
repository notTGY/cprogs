#include <iostream>
#include <unordered_map>

using namespace std;

int main() {
    int n;
    cin >> n;

    unordered_map<int, int> hash;

    for (int i = 0; i < n; ++i) {
        int num;
        cin >> num;
        if (hash.find(num) == hash.end()) {
            hash[num] = 1;
        } else {
            hash[num]++;
        }
    }

    cout << hash.size() << endl;

    return 0;
}