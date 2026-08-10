/*
==============================================================
    Rubik's Cube Solver - Console Entry Point

    Reads a 54-sticker cube, converts it to the table-based coordinates, and
    runs the two-phase solver. The measured time begins after input so typing
    the cube values is not included in the solver timing.

    Face order: Up, Down, Left, Right, Front, Back
    Colours: 0=Yellow, 1=White, 2=Red, 3=Orange, 4=Green, 5=Blue
==============================================================
*/

#include <chrono>
#include <iostream>
#include <vector>
#include<chrono>
#include "Cube_3.h"
#include "Solver.h"

using namespace std;

int main(){
    Cube3 C;
    PathFinder P;
    vector<string> solution;
    int cube[6][9];

    cout << "Enter the 54 values of the cube:\n";
    for(int i = 0; i < 6; i++)
        for(int j = 0; j < 9; j++){
            cin >> cube[i][j];
            if(cube[i][j]>5 || cube[i][j]<0){
                cout<<endl<<"Invalid Cube Colour entered.\nProgram terminated";
                exit(EXIT_FAILURE);
            }
        }

    auto start = chrono::steady_clock::now();

    C.init(cube);
    cout << "Current state of the scrambled cube:\n";
    C.print();

    solution = P.Path_Finder(C);

    if (solution.size() == 1 && solution[0][0] == 'T') {
        cout << "\n" << solution[0] << "\n";
        exit(EXIT_FAILURE);
    } else if(solution.empty())
        cout<<"\nCube already Solved.";
    else{

    cout << "\nMoves to solve the cube:\n\n";
    for(const auto& move : solution)
        cout << move << " ";

    cout << "\nCube solved. Moves required: " << solution.size();
    }
    cout << "\nSolved cube:\n"; 
    C.print();

    auto end = chrono::steady_clock::now();
    chrono::duration<double, milli> elapsed = end - start;
    cout << "\nTime taken to solve the cube: " << elapsed.count() << " ms\n";
}
