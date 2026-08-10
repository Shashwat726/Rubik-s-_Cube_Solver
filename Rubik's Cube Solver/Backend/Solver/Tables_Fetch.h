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
    // Keep large move tables on the heap. Fetcher is created per solve and
    // inline arrays overflow WebAssembly's small fixed stack.
    int(*co)[18] = new int[6561][18];
    int(*eo)[18] = new int[4096][18];
    int(*uds)[18] = new int[495][18];
    int(*cp)[10] = new int[40320][10];
    int(*ep)[10] = new int[967680][10];
    Corners O;
    Edges E;
    public:
        Fetcher();
        ~Fetcher();
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

static void check(int co,int eo,int uds,int cp,int ep,int move,const char* where){
    if(co<0||co>=6561||eo<0||eo>=4096||uds<0||uds>=495||cp<0||cp>=40320||ep<0||ep>=967680||move<0||move>=18){
        cerr<<"OUT OF RANGE in "<<where<<": co="<<co<<" eo="<<eo<<" uds="<<uds
            <<" cp="<<cp<<" ep="<<ep<<" move="<<move<<"\n";
        exit(1);
    }
} 

Fetcher::Fetcher(){
    #ifdef __EMSCRIPTEN__
    const string DB = "/Databases/";
    #else
    const string DB = "../Databases/";
    #endif

    ifstream file_co(DB + "CO.bin", ios::binary);
    if(!file_co.is_open()) cerr << "Failed to open " << DB + "CO.bin" << "\n";
    file_co.read((char*)co, sizeof(int) * 6561 * 18);
    file_co.close();

    ifstream file_eo(DB + "EO.bin", ios::binary);
    if(!file_eo.is_open()) cerr << "Failed to open " << DB + "EO.bin" << "\n";
    file_eo.read((char*)eo, sizeof(int) * 4096 * 18);
    file_eo.close();

    ifstream file_uds(DB + "UDS.bin", ios::binary);
    if(!file_uds.is_open()) cerr << "Failed to open " << DB + "UDS.bin" << "\n";
    file_uds.read((char*)uds, sizeof(int) * 495 * 18);
    file_uds.close();

    ifstream file_cp(DB + "CP.bin", ios::binary);
    if(!file_cp.is_open()) cerr << "Failed to open " << DB + "CP.bin" << "\n";
    file_cp.read((char*)cp, sizeof(int)*40320*10);
    file_cp.close();

    ifstream file_ep(DB + "EP.bin", ios::binary);
    if(!file_ep.is_open()) cerr << "Failed to open " << DB + "EP.bin" << "\n";
    file_ep.read((char*)ep, sizeof(int)*967680*10);
    file_ep.close();
}

Fetcher::~Fetcher(){
    delete[] co;
    delete[] eo;
    delete[] uds;
    delete[] cp;
    delete[] ep;
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
    check(c.co,c.eo,c.uds,c.cp,c.ep,move,"apply_move");
    c.co=co[c.co][move];
    c.eo=eo[c.eo][move];
    c.uds=uds[c.uds][move];
    c.cp=cp[c.cp][move];
    c.ep=ep[c.ep][move];
}

void Fetcher::apply_move_p1(cube &c, int move){
    check(c.co,c.eo,c.uds,c.cp,c.ep,move,"apply_move");
    c.co = co[c.co][move];
    c.eo=eo[c.eo][move];
    c.uds=uds[c.uds][move];
}

void Fetcher::apply_move_p2(cube &c, int move){
    check(c.co,c.eo,c.uds,c.cp,c.ep,move,"apply_move");
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
