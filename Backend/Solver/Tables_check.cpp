#include <iostream>
#include "Tables_Fetch.h"
using namespace std;

int main(){
    Fetcher F;
    // states reached after each first move, and the move index needed to undo it
    int state_co[10]  = {2440,840,820,820,984,2296,658,5842,882,818};
    int state_eo[10]  = {2098,568,562,562,674,594,306,1074,564,561};
    int state_uds[10] = {268,406,266,266,237,256,265,267,350,146};
    int undo_move[10] = {0,1,2,3,4,5,7,6,9,8};
    string names[10] = {"U2","D2","R2","L2","F2","B2","U","U'","D","D'"};

    cout << "Root should be: co=820 eo=562 uds=266\n\n";
    for(int i = 0; i < 10; i++){
        int co  = F.get_co(state_co[i], undo_move[i]);
        int eo  = F.get_eo(state_eo[i], undo_move[i]);
        int uds = F.get_uds(state_uds[i], undo_move[i]);
        cout << "undo " << names[i]
             << "  co=" << co  << (co==820  ? "" : "  <<< SHOULD BE 820")
             << "  eo=" << eo  << (eo==562  ? "" : "  <<< SHOULD BE 562")
             << "  uds=" << uds << (uds==266 ? "" : "  <<< SHOULD BE 266")
             << "\n";
    }
}