#include<iostream>
#include<unordered_map>
#include<queue>
#include<fstream>
#include <cstdint>   
#include <filesystem>
#include"../Solver/Cube_3.h"
#include"../Solver/Corners.h"
#include"../Solver/Edges.h"
#include"../Solver/Tables_Fetch.h"

using namespace std;

/*
==============================================================
    Phase 2 Database Generator

    WARNING: This is a database generation file, NOT the solver.
    Run this file ONCE to generate the Phase 2 databases.
    Do NOT include this file in the main solver build.

    This program uses BFS starting from the solved cube state
    using only Phase 2 moves (U, D, R2, L2, F2, B2) to precompute
    the minimum moves needed to fully solve any G1 state.

    Generates two binary database files:

    Phase2_A.bin : Corner permutation database
                   Key   = Hash_Permutation() — Lehmer code of corner positions
                   Value = minimum Phase 2 moves to solve corners
                   Entries: 40,320 (= 8!)

    Phase2_B.bin : Edge permutation database
                   Key   = Hash_Phase2A() * 24 + Hash_Phase2B()
                   Value = minimum Phase 2 moves to solve edges
                   Entries: ~967,680

    Output files must be placed in the databases/ folder.
    Expected generation time: under 1 minute.
==============================================================
*/

class Phase2{
    /*
    Builds the two Phase 2 pattern databases from the solved G1 state. Only
    the ten legal Phase 2 moves are used, so CP and EP stay inside G1 while
    their minimum distances to solved are recorded.
    */
    public:
        void Store(cube c, Corners O, Edges E);   // Runs BFS and writes both databases to file
};

void Phase2::Store(cube c, Corners O, Edges E){
    Fetcher F;
    int inverse[18]={0,1,2,3,4,5,7,6,9,8,11,10,13,12,15,14,17,16};
    unordered_map<int, int> Data_A,Data_B;
    int keyA,keyB;

    {
        queue<pair<cube,int>> states;
        keyA=O.Hash_Permutation();
        Data_A[keyA]=0;
        states.push({c,0});

        while(!states.empty()){
            cube cur=states.front().first;
            int move_no = states.front().second;
            states.pop();
            if(move_no>=18)
                continue;
            for(int i=0; i< 10; i++){
                F.apply_move_p2(cur, i);
                keyA=cur.cp;
                if(!Data_A.count(keyA)){
                    Data_A[keyA]=move_no+1;
                    states.push({cur, move_no+1});
                }
                F.apply_move_p2(cur,inverse[i]);
            }
        }
    }
    cout << endl << "Phase2_A BFS done. Entries: " << Data_A.size();

    {
        queue<pair<cube,int>> states;
        keyB=E.Hash_Phase2A()*24+E.Hash_Phase2B();
        Data_B[keyB] = 0;
        states.push({c,0});

        while(!states.empty()){
            cube cur = states.front().first;
            int move_no =states.front().second;
            states.pop();
            if(move_no>=18)
                continue;
            for(int i = 0; i<10; i++){
                F.apply_move_p2(cur,i);
                keyB=cur.ep;
                if(!Data_B.count(keyB)){
                    Data_B[keyB]=move_no+1;
                    states.push({cur, move_no+1});
                }
                F.apply_move_p2(cur,inverse[i]);
            }
        }
    }
     cout << endl << "Phase2_B BFS done. Entries: " << Data_B.size();

     // Write Phase2_A
    ofstream fileA("../Databases/Phase2_A.bin", ios::binary);
    for(auto& [key, moves_req] : Data_A){
        fileA.write((char*)&key, sizeof(int));
        fileA.write((char*)&moves_req, sizeof(int));
    }
    uintmax_t size = filesystem::file_size("../Databases/Phase2_A.bin");
    fileA.close();
    cout<<endl<<"Phase2_A file ready.\nFile size: "<<size<<" bytes";

    // Write Phase2_B
    ofstream fileB("../Databases/Phase2_B.bin", ios::binary);
    for(auto& [key, moves_req] : Data_B){
        fileB.write((char*)&key, sizeof(int));
        fileB.write((char*)&moves_req, sizeof(int));
    }
    uintmax_t size_2 = filesystem::file_size("../Databases/Phase2_B.bin");
    fileB.close();
    cout<<endl<<"Phase2_B file ready.\nFile size: "<<size_2<<" bytes";

    cout << endl << "Files ready";
}


int main(){
    Cube3 C;
    Corners O;
    Edges E;
    Phase2 P;
    Fetcher F;
    cout<<endl<<"Started";
    C.init();
    O.Preprocess(C);
    E.Orientation(C);
    cube c=F.to_cube(C);
    P.Store(c,O,E);
    cout<<endl<<"Done";
}
