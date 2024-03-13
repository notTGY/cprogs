#include <iostream>
#include <vector>

using namespace std;

int main() {
    vector<int> stack;

    while (true) {
        string command;
        cin >> command;

        if (command == "push") {
            int value;
            cin >> value;
            stack.push_back(value);
            cout << "ok" << endl;
        } else if (command == "pop") {
            if (stack.empty()) {
                cout << "error" << endl;
            } else {
                int value = stack.back();
                stack.pop_back();
                cout << value << endl;
            }
        } else if (command == "back") {
            if (stack.empty()) {
                cout << "error" << endl;
            } else {
                cout << stack.back() << endl;
            }
        } else if (command == "size") {
            cout << stack.size() << endl;
        } else if (command == "clear") {
            stack.clear();
            cout << "ok" << endl;
        } else if (command == "exit") {
            cout << "bye" << endl;
            break;
        } else {
            cout << "unknown command" << endl;
        }
    }

    return 0;
}