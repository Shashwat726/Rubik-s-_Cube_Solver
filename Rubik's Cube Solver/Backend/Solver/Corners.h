#pragma once

#include<iostream>
#include<map>
#include<unordered_map>
#include<vector>
#include<algorithm>
#include"Cube_3.h"

using namespace std; 

class Corners{
    /* 
    Corner orientation follows the Y/W sticker, not colour precedence. A
    corner is orientation 0 when its Y/W sticker is on U/D, orientation 1
    when it is on F/B, and orientation 2 when it is on L/R. This is important
    because the resulting CO value must be independent of corner permutation
    before it can be used as a move-table coordinate.

    Each corner of the cube is identified by 3 stickers.
    
    map_int : Each corner piece has a unique integer key computed as:
              key = c[0]*36 + c[1]*6 + c[2]  (c[] = sorted color values)
              Maps key → corner number (1-8)
              Corner 1 (Top-Left-Back)    : Y,R,B → key 17
              Corner 2 (Top-Right-Back)   : Y,O,B → key 23
              Corner 3 (Top-Left-Front)   : Y,R,G → key 16
              Corner 4 (Top-Right-Front)  : Y,O,G → key 22
              Corner 5 (Down-Left-Back)   : W,R,B → key 53
              Corner 6 (Down-Right-Back)  : W,O,B → key 59
              Corner 7 (Down-Left-Front)  : W,R,G → key 52
              Corner 8 (Down-Right-Front) : W,O,G → key 58

    corners : Cube[i][j] indices of the 3 stickers for each corner slot.
              First index of each corner is always the U/D face sticker.

    current : State of each corner slot after Preprocess() is called.
              Stores pair<piece, orientation> for each slot (1-8).
              Orientation : 0 = Y/W faces Up/Down (correct)
                            1 = Y/W faces Front/Back
                            2 = Y/W faces Left/Right

    Functions:
    Preprocess()      : Fills current[] with piece and orientation for all 8 corners.
    Preprocess(arr[]) : Same but only updates the 4 corners in arr[]. Used during solving.
    Hash()            : Encodes full corner state (position + orientation) → long long
    Hash_Orientation(): Encodes only corner orientations → int (0 to 2186)
    Hash_Permutation(): Encodes only corner positions using Lehmer code → int (0 to 40319)
    Right_call()      : Calls incremental Preprocess() for the 4 corners affected by a face move.
                        U→{1,2,3,4} D→{5,6,7,8} L→{1,3,5,7} R→{2,4,6,8} F→{3,4,7,8} B→{1,2,5,6}
    is_valid()        : Checks if the sum of corner orientations is divisible by 3.
    get_parity()      : Counts the number of inversions in the corner permutation and returns its parity (0 or 1).
    */
    unordered_map<int, int> map_int = {{17,1},{23,2},{16,3},{22,4},{53,5},{59,6},{52,7},{58,8}};
    map<int, vector<pair<int,int>>> corners = {
        {1, {{0,0},{2,0},{5,2}}},
        {2, {{0,2},{3,2},{5,0}}},
        {3, {{0,6},{2,2},{4,0}}},
        {4, {{0,8},{3,0},{4,2}}},
        {5, {{1,6},{2,6},{5,8}}},
        {6, {{1,8},{3,8},{5,6}}},
        {7, {{1,0},{2,8},{4,6}}},
        {8, {{1,2},{3,6},{4,8}}}
    };
    map<int, pair<int,int>> current;

    public:
        void Preprocess(Cube3 &C);
        int Hash_Orientation();
        int Hash_Permutation();
        bool is_valid();
        int get_parity();
};

bool Corners::is_valid(){
    int co_sum = 0;
    for(int i = 1; i <= 8; i++)
        co_sum += current[i].second;
    return (co_sum % 3 == 0);
}

int Corners::get_parity(){
    int inversions = 0;
    int pieces[8];
    for(int i = 1; i <= 8; i++)
        pieces[i-1] = current[i].first;
    for(int i = 0; i < 8; i++){
        for(int j = i+1; j < 8; j++){
            if(pieces[j] < pieces[i])
                inversions++;
        }
    }
    return inversions % 2;
}

void Corners::Preprocess(Cube3 &C){
    int arr[3]; 
    for(auto &[corner_num,indices] : corners){
        int k = 0;
        for(auto &[i,j] : indices){
            arr[k]=C.Get_num(i, j);
            k++;
        }
        // Corner orientation is determined by the face holding this cubie's
        // Y/W sticker, not by the numerical rank of the U/D-slot sticker.
        int orientation = 0;
        for(int k = 0; k < 3; k++){
            if(arr[k] == 0 || arr[k] == 1){
                int face = indices[k].first;
                if(face == 0 || face == 1)       // U/D
                    orientation = 0;
                else if(face == 4 || face == 5)  // F/B
                    orientation = 1;
                else                              // L/R
                    orientation = 2;
                break;
            }
        }
        sort(arr, arr+3);
        int key = arr[0] * 36 + arr[1] * 6 + arr[2];
        int piece = map_int[key];
        current[corner_num].first = piece;
        current[corner_num].second = orientation;
    }
}

int Corners::Hash_Orientation(){
    int key=0;
    for(int i = 1; i <= 8; i++)
        key=key*3+current[i].second;
    return key;
}

int Corners::Hash_Permutation(){
    int pieces[8];
    for(int i = 1; i <= 8; i++)
        pieces[i-1] = current[i].first;
    
    int key=0;
    for(int i=0;i<8;i++){
        int c=0;
        for(int j=i+1;j<8;j++)
            if(pieces[j]<pieces[i])
                c++;
        int fact=1;
        for(int k=1;k<=7-i;k++)
            fact*=k;
        key+=c*fact;
    }
    return key;
}
