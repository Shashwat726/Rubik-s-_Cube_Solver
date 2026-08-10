#include <iostream>
#include <fstream>
#include <unordered_map>
using namespace std;
int main() {
    ifstream file("Databases/Phase1_A.bin", ios::binary);
    int key, val;
    bool found_425 = false;
    while(file.read((char*)&key, sizeof(int))) {
        file.read((char*)&val, sizeof(int));
        if (key == 425) {
            cout << "Found key 425 with val " << val << "\n";
            found_425 = true;
            break;
        }
    }
    if (!found_425) cout << "Key 425 NOT FOUND!\n";
    return 0;
}
