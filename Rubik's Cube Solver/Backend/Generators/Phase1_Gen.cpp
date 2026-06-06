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
    Phase 1 Database Generator
    
    WARNING: This is a database generation file, NOT the solver.
    Run this file ONCE to generate the Phase 1 databases.
    Do NOT include this file in the main solver build.

    This program uses BFS starting from the solved cube state
    to precompute the minimum moves needed to reach G1 for every
    possible Phase 1 state configuration.

    Generates two binary database files:
    
    Phase1_A.bin : Corner orientation + UD slice database
                   Key   = Hash_Orientation() * 4096 + Hash_Slice()
                   Value = minimum moves to reach G1
                   Entries: ~3.2 million

    Phase1_B.bin : Edge orientation + UD slice database  
                   Key   = Hash_Phase1B()
                   Value = minimum moves to reach G1
                   Entries: ~1 million

    Output files must be placed in the databases/ folder.
    Expected generation time: 10-15 minutes.
==============================================================
*/

class Phase1{
    public:
        void Store(Cube3 C, Corners O, Edges E);   // Runs BFS and writes both databases to file
};

void Phase1::Store(Cube3 C, Corners O, Edges E){
    string moves[18] = { "R", "R'", "R2", "L", "L'", "L2", "U", "U'", "U2", "D", "D'", "D2", "F", "F'", "F2", "B", "B'", "B2"};
    string inverse[18] = { "R'", "R", "R2", "L'", "L", "L2", "U'", "U", "U2", "D'", "D", "D2", "F'", "F", "F2", "B'", "B", "B2"};
    unordered_map<int,int> Data_A, Data_B;
    int keyA, keyB;

    {
        queue<pair<Cube3,int>> states;
        O.Preprocess(C);
        E.Orientation(C);
        keyA = O.Hash_Orientation()*4096 + E.Hash_Slice(C);
        Data_A[keyA] = 0;
        states.push({C, 0});

        while(!states.empty()){
            Cube3 cur = states.front().first;
            int move_no = states.front().second;
            states.pop();
            if(move_no >= 12) 
                continue;
            for(int i = 0; i < 18; i++){
                cur.move(moves[i]);
                O.Preprocess(cur);
                E.Orientation(cur);
                keyA = O.Hash_Orientation()*4096 + E.Hash_Slice(cur);
                if(!Data_A.count(keyA)){
                    Data_A[keyA] = move_no+1;
                    states.push({cur, move_no+1});
                }
                cur.move(inverse[i]);
            }
        }
    }
    cout << endl << "Phase1_A BFS done. Entries: " << Data_A.size();

    {
        queue<pair<Cube3,int>> states;
        E.Orientation(C);
        keyB = E.Hash_Phase1B(C);
        Data_B[keyB] = 0;
        states.push({C, 0});

        while(!states.empty()){
            Cube3 cur = states.front().first;
            int move_no = states.front().second;
            states.pop();
            if(move_no >= 12) continue;
            for(int i = 0; i < 18; i++){
                cur.move(moves[i]);
                E.Orientation(cur);
                keyB = E.Hash_Phase1B(cur);
                if(!Data_B.count(keyB)){
                    Data_B[keyB] = move_no+1;
                    states.push({cur, move_no+1});
                }
                cur.move(inverse[i]);
            }
        }
    }
    cout << endl << "Phase1_B BFS done. Entries: " << Data_B.size();

    // Write Phase1_A
    ofstream fileA("Phase1_A.bin", ios::binary);
    for(auto& [key, moves_req] : Data_A){
        fileA.write((char*)&key, sizeof(int));
        fileA.write((char*)&moves_req, sizeof(int));
    }
    fileA.close();

    // Write Phase1_B
    ofstream fileB("Phase1_B.bin", ios::binary);
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
    Phase1 P;
    cout<<endl<<"Started";
    C.init();
    P.Store(C,O,E);
    cout<<endl<<"Done";
}