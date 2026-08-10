#pragma once

#include<iostream>
#include<fstream>
#include"Edges.h"
#include"Corners.h"
#include"Cube_3.h"

using namespace std;

struct cube{
    // Compact state used during the table-based search. The sticker matrix is
    // only needed when reading input or applying the final sequence.
    int co,eo,uds,cp,ep;
};

class Fetcher{
    /*
    Fetcher loads the precomputed move tables and bridges the matrix cube with
    the fast solver. Once a Cube3 is converted with to_cube(), a move is only
    a table lookup instead of a 6x9 sticker-matrix rotation.

    co, eo and uds are used in Phase 1. cp and ep are used in Phase 2.
    CO, EO and UDS contain all 18 moves; CP and EP contain the first ten
    legal Phase 2 moves from Solver.h.
    */
    int co[6561][18];
    int eo[4096][18];
    int uds[495][18];
    int(*cp)[10] = new int[40320][10];
    int(*ep)[10] = new int[967680][10];
    Corners O;
    Edges E;
    public:
        Fetcher();
        cube to_cube(Cube3 &C);
        bool is_G1(cube &c);
        bool is_Solved(cube &c);
        void apply_move(cube &c, int move);
        void apply_move_p1(cube &c, int move);
        void apply_move_p2(cube &c, int move);
        int get_co(int state, int move);
        int get_eo(int state, int move);
        int get_uds(int state, int move);
        int get_cp(int state, int move);
        int get_ep(int state, int move);
};

Fetcher::Fetcher(){
    ifstream file_co("../Databases/CO.bin", ios::binary);
    file_co.read((char*)co, sizeof(co));
    file_co.close();
     
    ifstream file_eo("../Databases/EO.bin", ios::binary);
    file_eo.read((char*)eo, sizeof(eo));
    file_eo.close();

    ifstream file_uds("../Databases/UDS.bin", ios::binary);
    file_uds.read((char*)uds, sizeof(uds));
    file_uds.close();
    
    ifstream file_cp("../Databases/CP.bin", ios::binary);
    file_cp.read((char*)cp, sizeof(int)*40320*10);
    file_cp.close();

    ifstream file_ep("../Databases/EP.bin", ios::binary);
    file_ep.read((char*)ep, sizeof(int)*967680*10);
    file_ep.close();
}

int Fetcher::get_co(int state, int move){
    return co[state][move];
}

int Fetcher::get_eo(int state, int move){
    return eo[state][move];
}

int Fetcher::get_uds(int state, int move){
    return uds[state][move];
}

int Fetcher::get_cp(int state, int move){
    return cp[state][move];
}

int Fetcher::get_ep(int state, int move){
    return ep[state][move];
}

void Fetcher::apply_move(cube &c, int move){
    c.co=co[c.co][move];
    c.eo=eo[c.eo][move];
    c.uds=uds[c.uds][move];
    c.cp=cp[c.cp][move];
    c.ep=ep[c.ep][move];
}

void Fetcher::apply_move_p1(cube &c, int move){
    c.co=co[c.co][move];
    c.eo=eo[c.eo][move];
    c.uds=uds[c.uds][move];
}

void Fetcher::apply_move_p2(cube &c, int move){
    c.cp=cp[c.cp][move];
    c.ep=ep[c.ep][move];
}

cube Fetcher::to_cube(Cube3 &C){
    cube c;
    O.Preprocess(C);
    E.Orientation(C);
    c.co=O.Hash_Orientation();
    c.eo=E.Hash_Orientation();
    c.uds=E.UDS();
    c.cp=O.Hash_Permutation();
    c.ep=((E.Hash_Phase2A() * 24) + E.Hash_Phase2B());
    return c;
}

bool Fetcher::is_G1(cube &c){
    return (c.co==0 && c.eo == 0 && c.uds == 425 )? true : false;
}

bool Fetcher::is_Solved(cube &c){
    return (c.co==0 && c.eo == 0 && c.uds == 425 && c.cp == 0 && c.ep == 0)? true : false;
}
