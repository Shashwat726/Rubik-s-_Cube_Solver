#include "Solver/Cube_3.h"
#include "Solver/Corners.h"
#include "Solver/Edges.h"
#include "Solver/Phase1_Fetch.h"
#include <iostream>
using namespace std;
int main() {
    Cube3 C; C.init(); C.move("R");
    Corners O; Edges E;
    O.Preprocess(C); E.Orientation(C);
    
    int co = O.Hash_Orientation();
    int eo = E.Hash_Orientation();
    int uds = E.UDS();
    cout << "R move CO: " << co << "\n";
    cout << "R move EO: " << eo << "\n";
    cout << "R move UDS: " << uds << "\n";
    
    Fetcher_1 F1;
    cout << "F1 Fetch: " << F1.Fetch(co, eo, uds) << "\n";
    return 0;
}
