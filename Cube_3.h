#pragma once

#include<iostream>
#include<map>

using namespace std;

class Cube3{
    /* 
    The "Cube" represents the cube with each row is a face as follows
    Row 0 : Top
    Row 1 : Bottom
    Row 2 : Left
    Row 3 : Right
    Row 4 : Front
    Row 5 : Back
    And each column represents the single square on each face.
    Each integer in the matrix can take values ranging from 0-5 representing colours as follows
    0 : Yellow
    1 : White
    2 : Red
    3 : Orange
    4 : Green
    5 : Blue

    Functions: 
    Rotate_Side : Takes care of the rotating sides when a move is made.
    Rotate_Face : Takes care of the rotating face when a move is made.
    */
    int Cube[6][9]; 
    void Rotate_Side(int &a1, int &a2, int &a3, int &b1, int &b2, int &b3, int &c1, int &c2, int &c3, int &d1, int &d2, int &d3, char x);
    void Rotate_Face(int f, char s, char x);   

    void R();                   //The R move of Rubik's cube
    void R_P();                 //The R' move of Rubik's cube
    void R_2();                 //The R2 move of Rubik's cube

    void L();                   //The L move of Rubik's cube
    void L_P();                 //The L' move of Rubik's cube
    void L_2();                 //The L2 move of Rubik's cube

    void U();                   //The U move of Rubik's cube
    void U_P();                 //The U' move of Rubik's cube
    void U_2();                 //The U2 move of Rubik's cube

    void D();                   //The D move of Rubik's cube
    void D_P();                 //The D' move of Rubik's cube
    void D_2();                 //The D2 move of Rubik's cube
    
    void F();                   //The F move of Rubik's cube
    void F_P();                 //The F' move of Rubik's cube
    void F_2();                 //The F2 move of Rubik's cube

    void B();                   //The B move of Rubik's cube
    void B_P();                 //The B' move of Rubik's cube
    void B_2();                 //The B2 move of Rubik's cube

    public:
        void move(string m);    //This function accepts the move to be made and calls the required function
        bool Is_solved();       //This function returns true if the cube is solved and false otherwise
        void init(int M[6][9]); //This function is used to initialize the cube
        void init();            //This function initializes the cube to a solved state
        void print();           //This function prints the cube in an open cube format
        int Get_num(int i, int j);  //This function returns the integer at Cube[i][j]
};

void Cube3::Rotate_Side(int &a1, int &a2, int &a3, int &b1, int &b2, int &b3, int &c1, int &c2, int &c3, int &d1, int &d2, int &d3, char x){
    //This function accepts a character ('h' or 'f') representing whether a half turn (90) is made or a full turn(180).
    int t1, t2, t3;                 //These are the temporary integeres used while changing values.
    if(x == 'h'){
        //Half turn(90)
        //A(ai) -> B(bi) -> C(ci) -> D(di) -> A(ai)     : i = 1 || 2 || 3
        t1 = d1; t2 = d2; t3 = d3;
        d1 = c1; d2 = c2; d3 = c3;
        c1 = b1; c2 = b2; c3 = b3;
        b1 = a1; b2 = a2; b3 = a3;
        a1 = t1; a2 = t2; a3 = t3;
    }else{
        //Full turn (180)
        //A(ai) <--> C(ci) and B(bi) <--> D(di)         : i = 1 || 2 || 3
        t1 = a1; t2 = a2; t3 = a3;
        a1 = c1; a2 = c2; a3 = c3;
        c1 = t1; c2 = t2; c3 = t3;

        t1 = b1; t2 = b2; t3 = b3;
        b1 = d1; b2 = d2; b3 = d3;
        d1 = t1; d2 = t2; d3 = t3;
    }
}

void Cube3::Rotate_Face(int f, char s, char x){
    /*
    This functions accepts an integer representing the face that is rotating.
    This functiion also accepts 2 characters: 
    's' to represent the sign of rotation (+ : Clockwise & - : Anti-clockwise) 
    'x' to represent the half turn or full 
    */
    int t;                  //Temporary integer used while changing values
    if(x == 'h'){
        //Half turn (90)
        if(s == '+'){
            //Clocwise
            t = Cube[f][1];
            Cube[f][1] = Cube[f][3];
            Cube[f][3] = Cube[f][7];
            Cube[f][7] = Cube[f][5];
            Cube[f][5] = t;

            t = Cube[f][0];
            Cube[f][0] = Cube[f][6];
            Cube[f][6] = Cube[f][8];
            Cube[f][8] = Cube[f][2];
            Cube[f][2] = t;
        }else{
            //Anti clockwise
            t = Cube[f][1];
            Cube[f][1] = Cube[f][5];
            Cube[f][5] = Cube[f][7];
            Cube[f][7] = Cube[f][3];
            Cube[f][3] = t;

            t = Cube[f][0];
            Cube[f][0] = Cube[f][2];
            Cube[f][2] = Cube[f][8];
            Cube[f][8] = Cube[f][6];
            Cube[f][6] = t;
        }
    }else{
        //Full turn (180)
        t = Cube[f][0]; Cube[f][0] = Cube[f][8]; Cube[f][8] = t;
        t = Cube[f][2]; Cube[f][2] = Cube[f][6]; Cube[f][6] = t;
        t = Cube[f][1]; Cube[f][1] = Cube[f][7]; Cube[f][7] = t;
        t = Cube[f][3]; Cube[f][3] = Cube[f][5]; Cube[f][5] = t;
    }
}

void Cube3::R(){
    //Front -> Top -> Back -> Bottom -> Front
    Rotate_Side(Cube[4][2], Cube[4][5], Cube[4][8], Cube[0][2], Cube[0][5], Cube[0][8], Cube[5][6], Cube[5][3], Cube[5][0], Cube[1][2], Cube[1][5], Cube[1][8], 'h');
    Rotate_Face(3, '+', 'h');
}
void Cube3::R_P(){
    //Front -> Bottom -> Back -> Top -> Front
    Rotate_Side(Cube[4][2], Cube[4][5], Cube[4][8], Cube[1][2], Cube[1][5], Cube[1][8], Cube[5][6], Cube[5][3], Cube[5][0], Cube[0][2], Cube[0][5], Cube[0][8], 'h');
    Rotate_Face(3, '-', 'h');
}
void Cube3::R_2(){
    //Front <--> Back and Top <--> Bottom
    Rotate_Side(Cube[4][2], Cube[4][5], Cube[4][8], Cube[0][2], Cube[0][5], Cube[0][8], Cube[5][6], Cube[5][3], Cube[5][0], Cube[1][2], Cube[1][5], Cube[1][8], 'f');
    Rotate_Face(3, '+', 'f');
}

void Cube3::L(){
    //Front -> Bottom -> Back -> Top -> Front
    Rotate_Side(Cube[4][0], Cube[4][3], Cube[4][6], Cube[1][0], Cube[1][3], Cube[1][6], Cube[5][8], Cube[5][5], Cube[5][2], Cube[0][0], Cube[0][3], Cube[0][6], 'h');
    Rotate_Face(2, '+', 'h');
}
void Cube3::L_P(){
    //Front -> Top -> Back -> Bottom -> Front
    Rotate_Side(Cube[4][0], Cube[4][3], Cube[4][6], Cube[0][0], Cube[0][3], Cube[0][6], Cube[5][8], Cube[5][5], Cube[5][2], Cube[1][0], Cube[1][3], Cube[1][6], 'h');
    Rotate_Face(2, '-', 'h');
}
void Cube3::L_2(){
    //Front <--> Back and Top <--> Bottom
    Rotate_Side(Cube[4][0], Cube[4][3], Cube[4][6], Cube[1][0], Cube[1][3], Cube[1][6], Cube[5][8], Cube[5][5], Cube[5][2], Cube[0][0], Cube[0][3], Cube[0][6], 'f');
    Rotate_Face(2, '+', 'f');
}

void Cube3::U(){
    //Front -> Left -> Back -> Right -> Front
    Rotate_Side(Cube[4][0], Cube[4][1], Cube[4][2], Cube[2][0], Cube[2][1], Cube[2][2], Cube[5][0], Cube[5][1], Cube[5][2], Cube[3][0], Cube[3][1], Cube[3][2], 'h');
    Rotate_Face(0, '+', 'h');
}
void Cube3::U_P(){
    //Front -> Right -> Back -> Left -> Front
    Rotate_Side(Cube[4][0], Cube[4][1], Cube[4][2], Cube[3][0], Cube[3][1], Cube[3][2], Cube[5][0], Cube[5][1], Cube[5][2], Cube[2][0], Cube[2][1], Cube[2][2], 'h');
    Rotate_Face(0, '-', 'h');
}
void Cube3::U_2(){
    //Front <--> Back and Left <--> Right
    Rotate_Side(Cube[4][0], Cube[4][1], Cube[4][2], Cube[2][0], Cube[2][1], Cube[2][2], Cube[5][0], Cube[5][1], Cube[5][2], Cube[3][0], Cube[3][1], Cube[3][2], 'f');
    Rotate_Face(0, '+', 'f');
}

void Cube3::D(){
    //Front -> Right -> Back -> Left -> Front
    Rotate_Side(Cube[4][6], Cube[4][7], Cube[4][8], Cube[3][6], Cube[3][7], Cube[3][8], Cube[5][6], Cube[5][7], Cube[5][8], Cube[2][6], Cube[2][7], Cube[2][8], 'h');
    Rotate_Face(1, '+', 'h');
}
void Cube3::D_P(){
    //Front -> Left -> Back -> Right -> Front
    Rotate_Side(Cube[4][6], Cube[4][7], Cube[4][8], Cube[2][6], Cube[2][7], Cube[2][8], Cube[5][6], Cube[5][7], Cube[5][8], Cube[3][6], Cube[3][7], Cube[3][8], 'h');
    Rotate_Face(1, '-', 'h');
}
void Cube3::D_2(){
    //Front <--> Back and Left <--> Right
    Rotate_Side(Cube[4][6], Cube[4][7], Cube[4][8], Cube[3][6], Cube[3][7], Cube[3][8], Cube[5][6], Cube[5][7], Cube[5][8], Cube[2][6], Cube[2][7], Cube[2][8], 'f');
    Rotate_Face(1, '+', 'f');
}

void Cube3::F(){
    //Top -> Right -> Bottom -> Left  -> Top
    Rotate_Side(Cube[0][6], Cube[0][7], Cube[0][8], Cube[3][0], Cube[3][3], Cube[3][6], Cube[1][2], Cube[1][1], Cube[1][0], Cube[2][8], Cube[2][5], Cube[2][2], 'h');
    Rotate_Face(4, '+', 'h');
}
void Cube3::F_P(){
    //Top -> Left -> Bottom -> Right -> Top
    Rotate_Side(Cube[0][6], Cube[0][7], Cube[0][8], Cube[2][8], Cube[2][5], Cube[2][2], Cube[1][2], Cube[1][1], Cube[1][0], Cube[3][0], Cube[3][3], Cube[3][6], 'h');
    Rotate_Face(4, '-', 'h');
}
void Cube3::F_2(){
    //Front <--> Back and Top <--> Bottom
    Rotate_Side(Cube[0][6], Cube[0][7], Cube[0][8], Cube[3][0], Cube[3][3], Cube[3][6], Cube[1][2], Cube[1][1], Cube[1][0], Cube[2][8], Cube[2][5], Cube[2][2], 'f');
    Rotate_Face(4 , '+', 'f');
}

void Cube3::B(){
    //Top -> Left -> Bottom -> Right -> Top
    Rotate_Side(Cube[0][0], Cube[0][1], Cube[0][2], Cube[2][6], Cube[2][3], Cube[2][0], Cube[1][8], Cube[1][7], Cube[1][6], Cube[3][2], Cube[3][5], Cube[3][8], 'h');
    Rotate_Face(5, '+', 'h');
}
void Cube3::B_P(){
    //Top -> Right -> Left -> Right -> Top
    Rotate_Side(Cube[0][0], Cube[0][1], Cube[0][2], Cube[3][2], Cube[3][5], Cube[3][8], Cube[1][8], Cube[1][7], Cube[1][6], Cube[2][6], Cube[2][3], Cube[2][0], 'h');
    Rotate_Face(5, '-', 'h');
}
void Cube3::B_2(){
    //Front <--> Back and Left <--> Right
    Rotate_Side(Cube[0][0], Cube[0][1], Cube[0][2], Cube[2][6], Cube[2][3], Cube[2][0], Cube[1][8], Cube[1][7], Cube[1][6], Cube[3][2], Cube[3][5], Cube[3][8], 'f');
    Rotate_Face(5, '+', 'f');
}

void Cube3::print(){
    map<int, char> m = {{0,'Y'}, {1,'W'}, {2,'R'}, {3,'O'}, {4,'G'}, {5,'B'}};
    cout<<endl<<"    "<<m[Cube[0][0]]<<m[Cube[0][1]]<<m[Cube[0][2]];
    cout<<endl<<"    "<<m[Cube[0][3]]<<m[Cube[0][4]]<<m[Cube[0][5]];
    cout<<endl<<"    "<<m[Cube[0][6]]<<m[Cube[0][7]]<<m[Cube[0][8]];
    cout<<endl<<m[Cube[2][0]]<<m[Cube[2][1]]<<m[Cube[2][2]]<<" "<<m[Cube[4][0]]<<m[Cube[4][1]]<<m[Cube[4][2]]<<" "<<m[Cube[3][0]]<<m[Cube[3][1]]<<m[Cube[3][2]]<<" "<<m[Cube[5][0]]<<m[Cube[5][1]]<<m[Cube[5][2]];
    cout<<endl<<m[Cube[2][3]]<<m[Cube[2][4]]<<m[Cube[2][5]]<<" "<<m[Cube[4][3]]<<m[Cube[4][4]]<<m[Cube[4][5]]<<" "<<m[Cube[3][3]]<<m[Cube[3][4]]<<m[Cube[3][5]]<<" "<<m[Cube[5][3]]<<m[Cube[5][4]]<<m[Cube[5][5]];
    cout<<endl<<m[Cube[2][6]]<<m[Cube[2][7]]<<m[Cube[2][8]]<<" "<<m[Cube[4][6]]<<m[Cube[4][7]]<<m[Cube[4][8]]<<" "<<m[Cube[3][6]]<<m[Cube[3][7]]<<m[Cube[3][8]]<<" "<<m[Cube[5][6]]<<m[Cube[5][7]]<<m[Cube[5][8]];
    cout<<endl<<"    "<<m[Cube[1][0]]<<m[Cube[1][1]]<<m[Cube[1][2]];
    cout<<endl<<"    "<<m[Cube[1][3]]<<m[Cube[1][4]]<<m[Cube[1][5]];
    cout<<endl<<"    "<<m[Cube[1][6]]<<m[Cube[1][7]]<<m[Cube[1][8]];
    cout<<endl;
}

void Cube3::move(string m){
    if(m == "R") R();
    else if(m == "R'") R_P();
    else if(m == "R2") R_2();

    else if(m == "L") L();
    else if(m == "L'") L_P();
    else if(m == "L2") L_2();

    else if(m == "U") U();
    else if(m == "U'") U_P();
    else if(m == "U2") U_2();

    else if(m == "D") D();
    else if(m == "D'") D_P();
    else if(m == "D2") D_2();

    else if(m == "F") F();
    else if(m == "F'") F_P();
    else if(m == "F2") F_2();

    else if(m == "B") B();
    else if(m == "B'") B_P();
    else if(m == "B2") B_2();
}

bool Cube3::Is_solved(){
    for(int i = 0; i < 6; i++)
        for(int j = 0; j < 9; j++)
            if(Cube[i][j] != i)
                return false;
    return true;
}

void Cube3::init(int M[6][9]){
    for(int i = 0; i < 6; i++)
        for(int j = 0; j < 9; j++)
            Cube[i][j] = M[i][j];
}

void Cube3::init(){
    for(int i = 0; i < 6; i++)
        for(int j = 0; j < 9; j++)
            Cube[i][j] = i;
}

int Cube3::Get_num(int i, int j){
    return Cube[i][j];
}