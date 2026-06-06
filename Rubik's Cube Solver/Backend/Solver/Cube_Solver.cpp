/*
==============================================================
    Rubik's Cube Solver — Main Program
    
    This is the entry point of the solver.
    It ties together all components of Kociemba's Two-Phase Algorithm:

    Phase 1 : PathFinder1 finds a sequence of moves to reach G1 state.
              G1 = all corners untwisted, all edges unflipped,
                   all middle layer edges in the middle layer.

    Phase 2 : PathFinder2 finds a sequence of moves to fully solve
              the cube from G1 using only U, D, R2, L2, F2, B2.

    Input  : 54 integers representing the cube state (6 faces × 9 stickers)
             Color encoding: 0=Yellow 1=White 2=Red 3=Orange 4=Green 5=Blue
             Face order: Top(0) Bottom(1) Left(2) Right(3) Front(4) Back(5)
             Sticker order per face (reading order):
             0 1 2
             3 4 5
             6 7 8

    Output : Two sequences of moves (Phase 1 and Phase 2) that together
             solve the cube, followed by the solved cube state.

    Note   : The databases (Phase1_A.bin, Phase1_B.bin, Phase2_A.bin,
             Phase2_B.bin) must exist in the databases/ folder before
             running this program. If missing, run the generator files first.
==============================================================
*/

#include<iostream>
#include<vector>
#include<chrono>
#include"Cube_3.h"
#include"Phase1_Solver.h"
#include"Phase2_Solver.h"
#include"Phase2_Fetch.h"

using namespace std;

int main(){
    Cube3 C;
    PathFinder1 Pf1;
    PathFinder2 Pf2;
    vector<string> solution1,solution2;
    int cube[6][9];
    cout<<" Enter the 54 values of the cube: \n";
    auto start = chrono::high_resolution_clock::now();
    for(int i = 0; i < 6; i++)
        for(int j = 0; j < 9; j++)
            cin>>cube[i][j];
    C.init(cube);
    cout<<"Current state of the scrambled cube: \n";
    C.print();
    solution1 = Pf1.Path_Finder_1(C);
    solution2 = Pf2.Path_Finder_2(C);
    cout<<endl<<"Moves to solve the cube: \n";
    cout<<endl;
    for(const auto& s : solution1){
        cout<<s<<" "; 
    }
    for(const auto& s : solution2){
        cout<<s<<" "; 
    }
    cout<<endl<<"Cube Solved: Moves required:"<<solution1.size()+solution2.size();
    cout<<endl<<"Solved cube: \n";
    C.print();
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end-start);
    cout<<endl<<"Time taken to solve the cube: "<<duration.count()/1000.0<<"s";
}