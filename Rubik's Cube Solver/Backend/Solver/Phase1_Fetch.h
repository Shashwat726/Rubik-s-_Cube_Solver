#pragma once

#include<iostream>
#include<fstream>
#include<unordered_map>
#include<cstdlib>
#include"Corners.h"
#include"Edges.h"
#include"Cube_3.h"

using namespace std;

class Fetcher_1{
    /*
    Current table format:
    Phase1_A.bin is indexed by co * 495 + uds.
    Phase1_B.bin is indexed by eo * 495 + uds.
    The older 4096/slice-bitmap wording below is retained as a note from the
    first matrix implementation; the Fetch() function uses the current format.

    Loads the Phase 1 databases and provides the heuristic for Phase 1 IDA*.
    
    Data_A : Loaded from Phase1_A.bin
             Key   = co * 495 + uds
             Value = minimum moves to reach G1 from this corner orientation + slice state
             
    Data_B : Loaded from Phase1_B.bin
             Key   = eo * 495 + uds
             Value = minimum moves to reach G1 from this edge orientation + slice state

    O, E   : Used internally to compute hash keys from a given cube state.

    Functions:
    Fetcher_1() : Constructor. Loads Phase1_A.bin and Phase1_B.bin into Data_A and Data_B.
    Fetch(C)    : Returns max(Data_A[keyA], Data_B[keyB]) — the Phase 1 heuristic.
                  Higher value = farther from G1.
    isG1(C)     : Returns true if Fetch(C) == 0, meaning cube is in G1 state.
    */
    unordered_map<int,int> Data_A, Data_B;
    Corners O;
    Edges E;
    public:
        Fetcher_1();
        int Fetch(int co, int eo, int uds);
};

Fetcher_1::Fetcher_1(){
    #ifdef __EMSCRIPTEN__
    const string DB = "/Databases/";
    #else
    const string DB = "../Databases/";
    #endif

    int key,moves_req;
    ifstream fileA(DB + "Phase1_A.bin", ios::binary);
    if(!fileA.is_open()) cerr << "Failed to open " << DB + "Phase1_A.bin" << "\n";
    while(fileA.read((char*)&key, sizeof(int))){
        fileA.read((char*)&moves_req, sizeof(int));
        Data_A[key]=moves_req;
    }
    fileA.close();
    ifstream fileB(DB + "Phase1_B.bin", ios::binary);
    if(!fileB.is_open()) cerr << "Failed to open " << DB + "Phase1_B.bin" << "\n";
    while(fileB.read((char*)&key, sizeof(int))){
        fileB.read((char*)&moves_req, sizeof(int));
        Data_B[key]=moves_req;
    }
    fileB.close();
}

int Fetcher_1::Fetch(int co, int eo, int uds){
    int keyA = co * 495 + uds;
    int keyB = eo * 495 + uds;
    auto itA = Data_A.find(keyA);
    auto itB = Data_B.find(keyB);

if (itA == Data_A.end() || itB == Data_B.end()) {
    return -1;
}

return max(itA->second, itB->second);
}
