#include<iostream>
#include<unordered_map>
#include<queue>
#include<fstream>
#include"Cube_3.h"
#include"Corners.h"
#include"Edges.h"

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
    public:
        void Store(Cube3 C, Corners O, Edges E);   // Runs BFS and writes both databases to file
};

void Phase2::Store(Cube3 C, Corners O, Edges E){
    string moves[6] = {"U", "D", "R2", "L2", "F2", "B2"};
    string inverse[6] ={"U'", "D'", "R2", "L2", "F2", "B2"};
    unordered_map<int, int> Data_A,Data_B;
    int keyA,keyB;

    {
        queue<pair<Cube3,int>> states;
        O.Preprocess(C);
        keyA=O.Hash_Permutation();
        Data_A[keyA]=0;
        states.push({C,0});

        while(!states.empty()){
            Cube3 cur=states.front().first;
            int move_no = states.front().second;
            states.pop();
            if(move_no>=18)
                continue;
            for(int i=0; i< 6; i++){
                cur.move(moves[i]);
                O.Preprocess(cur);
                keyA=O.Hash_Permutation();
                if(!Data_A.count(keyA)){
                    Data_A[keyA]=move_no+1;
                    states.push({cur, move_no+1});
                }
                cur.move(inverse[i]);
            }
        }
    }
    cout << endl << "Phase2_A BFS done. Entries: " << Data_A.size();

    {
        queue<pair<Cube3,int>> states;
        E.Orientation(C);
        keyB=E.Hash_Phase2A()*24+E.Hash_Phase2B();
        Data_B[keyB] = 0;
        states.push({C,0});

        while(!states.empty()){
            Cube3 cur = states.front().first;
            int move_no =states.front().second;
            states.pop();
            if(move_no>=18)
                continue;
            for(int i = 0; i<6; i++){
                cur.move(moves[i]);
                E.Orientation(cur);
                keyB=E.Hash_Phase2A()*24+E.Hash_Phase2B();
                if(!Data_B.count(keyB)){
                    Data_B[keyB]=move_no+1;
                    states.push({cur, move_no+1});
                }
                cur.move(inverse[i]);
            }
        }
    }
     cout << endl << "Phase2_B BFS done. Entries: " << Data_B.size();

     // Write Phase2_A
    ofstream fileA("Phase2_A.bin", ios::binary);
    for(auto& [key, moves_req] : Data_A){
        fileA.write((char*)&key, sizeof(int));
        fileA.write((char*)&moves_req, sizeof(int));
    }
    fileA.close();

    // Write Phase2_B
    ofstream fileB("Phase2_B.bin", ios::binary);
    for(auto& [key, moves_req] : Data_B){
        fileB.write((char*)&key, sizeof(int));
        fileB.write((char*)&moves_req, sizeof(int));
    }
    fileB.close();

    cout << endl << "Files ready";
}


int main(){
    Cube3 C;
    Corners O;
    Edges E;
    Phase2 P;
    cout<<endl<<"Started";
    C.init();
    P.Store(C,O,E);
    cout<<endl<<"Done";
}