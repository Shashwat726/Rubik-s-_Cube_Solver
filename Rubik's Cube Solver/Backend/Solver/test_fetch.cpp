#include "Cube_3.h"
#include "Phase1_Fetch.h"
#include <iostream>
using namespace std;
int main() {
    Cube3 C; C.init(); C.move("R");
    Fetcher_1 F1;
    Corners O; Edges E;
    O.Preprocess(C); E.Orientation(C);
    cube c;
    // We need to convert C to cube format.
    // Actually just look at Phase1_Fetch's Fetch.
    return 0;
}
