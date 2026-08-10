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
    Phase 1 Database Generator
    
    WARNING: This is a database generation file, NOT the solver.
    Run this file ONCE to generate the Phase 1 databases.
    Do NOT include this file in the main solver build.

    This program uses BFS starting from the solved cube state
    to precompute the minimum moves needed to reach G1 for every
    possible Phase 1 state configuration.

    Generates two binary database files:
    
    Phase1_A.bin : Corner orientation + UD slice database
                   Key   = co * 495 + uds
                   Value = minimum moves to reach G1
                   Entries: ~3.2 million

    Phase1_B.bin : Edge orientation + UD slice database  
                   Key   = eo * 495 + uds
                   Value = minimum moves to reach G1
                   Entries: ~1 million

    Output files must be placed in the databases/ folder.
    Expected generation time: 10-15 minutes.
==============================================================
*/

class Phase1{
    /*
    Builds the two Phase 1 pattern databases by walking CO, EO and UDS move
    tables from the solved state. The current keys are co * 495 + uds and
    eo * 495 + uds. Rebuild Phase1_A.bin whenever the CO convention changes.
    */
    public:
        void Store(cube &c, Corners O, Edges E);   // Runs BFS and writes both databases to file
};

void Phase1::Store(cube &c, Corners O, Edges E){
    Fetcher F;
    int inverse[18]={0,1,2,3,4,5,7,6,9,8,11,10,13,12,15,14,17,16};
    unordered_map<int,int> Data_A, Data_B;
    int keyA, keyB;

    {
        queue<pair<cube,int>> states;
        keyA = O.Hash_Orientation()*495 + E.UDS();
        Data_A[keyA] = 0;
        states.push({c, 0});

        while(!states.empty()){
            
            cube cur = states.front().first;
            int move_no = states.front().second;
            states.pop();
            if(move_no >= 12) 
                continue;
            for(int i = 0; i < 18; i++){
                F.apply_move_p1(cur, i);
                keyA = cur.co*495 + cur.uds;
                if(!Data_A.count(keyA)){
                    Data_A[keyA] = move_no+1;
                    states.push({cur, move_no+1});
                }
                F.apply_move_p1(cur,inverse[i]);
            }
        }
    }
    

    {
        queue<pair<cube,int>> states;
        keyB =E.Hash_Orientation()*495 + E.UDS();
        Data_B[keyB] = 0;
        states.push({c, 0});

        while(!states.empty()){
            cube cur = states.front().first;
            int move_no = states.front().second;
            states.pop();
            if(move_no >= 12) continue;
            for(int i = 0; i < 18; i++){
                F.apply_move_p1(cur,i);
                keyB = cur.eo*495+cur.uds;
                if(!Data_B.count(keyB)){
                    Data_B[keyB] = move_no+1;
                    states.push({cur, move_no+1});
                }
                F.apply_move_p1(cur, inverse[i]);
            }
        }
    }

    cout << "\nData_A size = " << Data_A.size();
cout << "\nData_B size = " << Data_B.size();

    // Write Phase1_A
    ofstream fileA("../Databases/Phase1_A.bin", ios::binary);
    for(auto& [key, moves_req] : Data_A){
        fileA.write((char*)&key, sizeof(int));
        fileA.write((char*)&moves_req, sizeof(int));
    }
    uintmax_t size = filesystem::file_size("../Databases/Phase1_A.bin");
    fileA.close();
    cout<<endl<<"Phase1_A file ready.\nFile size: "<<size<<" bytes";

    // Write Phase1_B
    ofstream fileB("../Databases/Phase1_B.bin", ios::binary);
    for(auto& [key, moves_req] : Data_B){
        fileB.write((char*)&key, sizeof(int));
        fileB.write((char*)&moves_req, sizeof(int));
    }
    uintmax_t size_2 = filesystem::file_size("../Databases/Phase1_B.bin");
    fileB.close();
    cout<<endl<<"Phase1_B file ready.\nFile size: "<<size_2<<" bytes";

    cout << endl << "Files ready";
}

    
int main(){
    Cube3 C;
    Corners O;
    Edges E;
    Phase1 P;
    Fetcher F;
    cout<<endl<<"Started";
    C.init();
    O.Preprocess(C);
    E.Orientation(C);
    cube c = F.to_cube(C);
    P.Store(c,O,E);
    cout<<endl<<"Done";
}
