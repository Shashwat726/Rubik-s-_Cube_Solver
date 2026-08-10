#pragma once

#include<iostream>
#include<map>
#include<vector>
#include<unordered_map>
#include"Cube_3.h"

using namespace std;

class Edges{
    /*
    Each edge of the cube is identified by 2 stickers.
    Edges are numbered such that the lower color value is always listed first.
    This is intentional — it allows orientation to be detected by a simple comparison.

    curr_O[13]     : Orientation of whatever edge piece is in each slot (1-12).
                     0 = correctly oriented (lower color value faces its correct direction)
                     1 = flipped
                     Index 0 unused (1-based indexing).

    curr_piece[13] : Which edge piece (1-12) is currently in each slot.
                     Filled by Orientation() alongside curr_O[].
                     Index 0 unused (1-based indexing).

    edge_map : Maps each edge piece's unique integer key to its piece number (1-12).
               Key = min(c1,c2)*6 + max(c1,c2) where c1,c2 are the two color values.
               Edge 1  (Up-Left)    : Y,R = 0,2 → key 2
               Edge 2  (Up-Back)    : Y,B = 0,5 → key 5
               Edge 3  (Up-Right)   : Y,O = 0,3 → key 3
               Edge 4  (Up-Front)   : Y,G = 0,4 → key 4
               Edge 5  (Left-Front) : R,G = 2,4 → key 16
               Edge 6  (Left-Back)  : R,B = 2,5 → key 17
               Edge 7  (Right-Back) : O,B = 3,5 → key 23
               Edge 8  (Right-Front): O,G = 3,4 → key 22
               Edge 9  (Down-Left)  : W,R = 1,2 → key 8
               Edge 10 (Down-Back)  : W,B = 1,5 → key 11
               Edge 11 (Down-Right) : W,O = 1,3 → key 9
               Edge 12 (Down-Front) : W,G = 1,4 → key 10

    edges : Cube[i][j] indices of the 2 stickers for each edge slot (1-12).
            First index always corresponds to the lower color value sticker.

    Functions (private):
    Hash_Orientation() : Encodes all 12 edge orientations as a base-2 number → int (0 to 4095). Used for Phase1_B.
    Hash_NonSlice()    : Lehmer code for the 8 non-slice edges (1,2,3,4,9,10,11,12) → int (0 to 40319). Used for Phase2_A.
    Hash_Slice_P()     : Lehmer code for the 4 slice edges (5,6,7,8) → int (0 to 23). Used for Phase2_B.

    Functions (public):
    Orientation(C)   : Fills curr_O[] and curr_piece[] for all 12 edge slots. Must be called before any Hash function.
    Hash_Slice(C)    : Encodes which of the 12 slots contain slice pieces as a binary number → int (0 to 4095). Used for Phase1_A and Phase1_B.
    Out_of_Slice(C)  : Returns count of slice pieces outside the middle layer slots (5-8). Range 0-4. Used for isG1() check.
    Hash_Phase1B(C)  : Combines edge orientation + slice → int. Key for Phase1_B database.
    Hash_Phase2A()   : Returns Hash_NonSlice(). Key component for Phase2_B database.
    Hash_Phase2B()   : Returns Hash_Slice_P(). Key component for Phase2_B database.
    is_valid()       : Checks if the sum of edge orientations is divisible by 2.
    get_parity()     : Counts the number of inversions in the edge permutation and returns its parity (0 or 1).
    */
    int curr_O[13];
    int curr_piece[13];
    unordered_map<int,int> edge_map = {{2,1},{5,2},{3,3},{4,4},{16,5},{17,6},{23,7},{22,8},{8,9},{11,10},{9,11},{10,12}};
    map<int, vector<pair<int,int>>> edges = {
        {1,  {{0,3},{2,1}}},
        {2,  {{0,1},{5,1}}},
        {3,  {{0,5},{3,1}}},
        {4,  {{0,7},{4,1}}},
        {5,  {{2,5},{4,3}}},
        {6,  {{2,3},{5,5}}},
        {7,  {{3,5},{5,3}}},
        {8,  {{3,3},{4,5}}},
        {9,  {{1,3},{2,7}}},
        {10, {{1,7},{5,7}}},
        {11, {{1,5},{3,7}}},
        {12, {{1,1},{4,7}}}
    };

    
    int Hash_NonSlice();
    int Hash_Slice_P();
    public:
        int Hash_Orientation();
        void Orientation(Cube3 &C);
        int Hash_Phase2A();
        int Hash_Phase2B();
        int UDS();
<<<<<<<< HEAD:Backend/Solver/Edges.h
        bool is_valid();
        int get_parity();
========
>>>>>>>> 9173473365a7b4fe299ca4dc65e455ca3223ca8f:Rubik's Cube Solver/Backend/Solver/Edges.h
};

bool Edges::is_valid(){
    int eo_sum = 0;
    for(int i = 1; i <= 12; i++)
        eo_sum += curr_O[i];
    return (eo_sum % 2 == 0);
}

int Edges::get_parity(){
    int inversions = 0;
    int pieces[12];
    for(int i = 1; i <= 12; i++)
        pieces[i-1] = curr_piece[i];
    for(int i = 0; i < 12; i++){
        for(int j = i+1; j < 12; j++){
            if(pieces[j] < pieces[i])
                inversions++;
        }
    }
    return inversions % 2;
}

void Edges::Orientation(Cube3 &C){
    for(auto &[edge_num, indices] : edges){
        int first = C.Get_num(indices[0].first, indices[0].second);
        int second = C.Get_num(indices[1].first, indices[1].second);
        curr_O[edge_num] = first < second ? 0 : 1;
        int key = min(first,second)*6 + max(first,second);
        curr_piece[edge_num] = edge_map[key];
    }
}

int Edges::Hash_Orientation(){
        int key = 0; 
        for(int i = 1; i <= 12; i++)
            key = key * 2 + curr_O[i];
        return key;
}

int Edges::Hash_NonSlice(){
    int pieces[8];
    int non_slice[8]={1,2,3,4,9,10,11,12};
    for(int i=0;i<8;i++)
        pieces[i]=curr_piece[non_slice[i]];
    for(int i=0;i<8;i++)
        if(pieces[i]>4)
            pieces[i]-=4;
    int key=0;
    for(int i =0;i<8;i++){
        int c=0;
        for(int j=i+1;j<8;j++)
            if(pieces[j]<pieces[i])
                c++;
        int fact = 1;
        for(int k=1;k<=7-i;k++)
            fact*=k;
        key+=c*fact;
    }
    return key;
}

int Edges::Hash_Slice_P(){
    int pieces[4];
    int slice[4]={5,6,7,8};
    for(int i=0;i<4;i++)
        pieces[i]=curr_piece[slice[i]];
    for(int i = 0; i < 4; i++)
        pieces[i] -= 4; 
    int key=0;
    for(int i=0;i<4;i++){
        int c=0;
        for(int j=i+1;j<4;j++)
            if(pieces[j]<pieces[i])
                c++;
        int fact=1;
        for(int k=1;k<=3-i;k++)
            fact*=k;
        key+=c*fact;
    }
    return key;
}

int Edges::Hash_Phase2A(){
    return Hash_NonSlice();
}

int Edges::Hash_Phase2B(){
    return Hash_Slice_P();
}

int Edges::UDS() {
    int rank = 0;
    int r = 4;  // number of slice edges left to find

    for(int pos = 12; pos >= 1; pos--) {

        bool isSlice =
            (curr_piece[pos] == 5 ||
             curr_piece[pos] == 6 ||
             curr_piece[pos] == 7 ||
             curr_piece[pos] == 8);

        if(isSlice) {
            r--;
        }
        else if(r > 0) {
            // rank += C(pos-1, r-1)
            int n = pos - 1;
            int k = r - 1;

            int comb = 1;
            for(int i = 1; i <= k; i++)
                comb = comb * (n - k + i) / i;

            rank += comb;
        }
    }

    return rank;
}
