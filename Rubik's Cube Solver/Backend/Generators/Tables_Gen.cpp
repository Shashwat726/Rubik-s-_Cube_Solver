#include<iostream> 
#include<vector>
#include<fstream>
#include<queue>
#include<unordered_set>
#include <cstdint>   
#include <filesystem>
#include<cstring>
#include"../Solver/corners.h"
#include"../Solver/edges.h"
#include"../Solver/Cube_3.h"

using namespace std;

/*
==============================================================
    Move Table Generator

    This program builds the lookup tables used by the fast solver. Each entry
    stores the coordinate reached after one move, so search no longer rotates
    a 6x9 sticker matrix at every IDA* node.

    CO, EO and UDS use all 18 moves. CP and EP use the ten Phase 2 moves.
    Run it from Backend/Generators so it writes to Backend/Databases.
    Rebuild CO.bin whenever the corner-orientation convention changes.
==============================================================
*/
class Tables{
    Corners O;
    Edges E;
    string moves[18]={"U2","D2","R2","L2","F2","B2","U","U'","D","D'","R","R'","L","L'","F","F'","B","B'"};
    string inverse[18]={"U2","D2","R2","L2","F2","B2","U'","U","D'","D","R'","R","L'","L","F'","F","B'","B"};
    // int inverse[18]={0,1,2,3,4,5,7,6,9,8,11,10,13,12,15,14,17,16};
    public:
     void Build_CO(Cube3 &C);
     void Build_EO(Cube3 &C);
     void Build_UDS(Cube3 &C);
     void Build_CP(Cube3 &C);
     void Build_EP(Cube3 &C);
};
void Tables::Build_CO(Cube3 &C){
    int co[6561][18];
    memset(co,0,sizeof(co));
    queue<pair<Cube3, int>> states;
    unordered_set<int> check_set;
    O.Preprocess(C);
    int cur_co = O.Hash_Orientation();
    states.push({C,cur_co});
    cout<<endl<<"Initial CO = "<<cur_co;
    check_set.insert(cur_co);
    while(!states.empty()){
        Cube3 cur = states.front().first;
        cur_co=states.front().second;
        states.pop();
        for(int i=0;i<18;i++){
            cur.move(moves[i]);
            O.Preprocess(cur);
            int new_co = O.Hash_Orientation();
            co[cur_co][i]=new_co;
            if(!check_set.count(new_co)){
                check_set.insert(new_co);
                states.push({cur,new_co});
            }
            cur.move(inverse[i]);
        }
    }
    cout<<endl<<"States reached: "<<check_set.size();
    cout<<endl<<"CO ready, writing to file.";
    ofstream file("../Databases/CO.bin", ios::binary);
    file.write((char*)co, sizeof(co));
    uintmax_t size = filesystem::file_size("../Databases/CO.bin");
    file.close();
    cout<<endl<<"CO file ready.\nFile size: "<<size<<" bytes";
} 

void Tables::Build_EO(Cube3 &C){
    int eo[4096][18];
    memset(eo,0,sizeof(eo));
    queue<pair<Cube3, int>> states;
    unordered_set<int> check_set;
    E.Orientation(C);
    int cur_eo = E.Hash_Orientation();
    states.push({C,cur_eo});
    cout<<endl<<"Initial EO = "<<cur_eo;
    check_set.insert(cur_eo);
    while(!states.empty()){
        Cube3 cur = states.front().first;
        cur_eo=states.front().second;
        states.pop();
        for(int i=0;i<18;i++){
            cur.move(moves[i]);
            E.Orientation(cur);
            int new_eo = E.Hash_Orientation();
            eo[cur_eo][i]=new_eo;
            if(!check_set.count(new_eo)){
                check_set.insert(new_eo);
                states.push({cur,new_eo});
            }
            cur.move(inverse[i]);
        }
    }
    cout<<endl<<"States reached: "<<check_set.size();
    cout<<endl<<"EO ready, writing to file.";
    ofstream file("../Databases/EO.bin", ios::binary);
    file.write((char*)eo, sizeof(eo));
    uintmax_t size = filesystem::file_size("../Databases/EO.bin");
    file.close();
    cout<<endl<<"EO file ready.\nFile size: "<<size<<" bytes";
}

void Tables::Build_UDS(Cube3 &C){
    int uds[495][18];
    memset(uds,0,sizeof(uds));
    queue<pair<Cube3,int>> states;
    unordered_set<int> check_set;
    E.Orientation(C);
    int cur_uds=E.UDS();
    cout<<endl<<"Initial UDS = "<<cur_uds;
    states.push({C,cur_uds});
    check_set.insert(cur_uds);
    while(!states.empty()){
        Cube3 cur=states.front().first;
        cur_uds = states.front().second;
            states.pop();
            for(int i=0;i<18;i++){
                cur.move(moves[i]);
                E.Orientation(cur);
                int new_uds = E.UDS();
                uds[cur_uds][i]=new_uds;
                if(!check_set.count(new_uds)){
                    check_set.insert(new_uds);
                    states.push({cur,new_uds});
                }
                cur.move(inverse[i]);
            }
    }
    cout<<endl<<"States reached: "<<check_set.size();
    cout<<endl<<"UDS ready, writing to file.";
    ofstream file("../Databases/UDS.bin", ios::binary);
    file.write((char*)uds, sizeof(uds));
    uintmax_t size = filesystem::file_size("../Databases/UDS.bin");
    file.close();
    cout<<endl<<"UDS file ready.\nFile size: "<<size<<" bytes";
}

void Tables::Build_CP(Cube3 &C){
    int(*cp)[10] = new int[40320][10];
    queue<pair<Cube3,int>> states;
    unordered_set<int> check_set;
    O.Preprocess(C);
    int cur_cp = O.Hash_Permutation();
    cout<<endl<<"Initial CP = "<<cur_cp;
    states.push({C,cur_cp});
    check_set.insert(cur_cp);
    while(!states.empty()){
        Cube3 cur=states.front().first;
        cur_cp = states.front().second;
            states.pop();
            for(int i=0;i<10;i++){
                cur.move(moves[i]);
                O.Preprocess(cur);
                int new_cp = O.Hash_Permutation();
                cp[cur_cp][i]=new_cp;
                if(!check_set.count(new_cp)){
                    check_set.insert(new_cp);
                    states.push({cur,new_cp});
                }
                cur.move(inverse[i]);
            }
    }
    cout<<endl<<"States reached: "<<check_set.size();
    cout<<endl<<"CP ready, writing to file.";
    ofstream file("../Databases/CP.bin", ios::binary);
    file.write((char*)cp, sizeof(int)*40320*10);
    uintmax_t size = filesystem::file_size("../Databases/CP.bin");
    file.close();
    cout<<endl<<"CP file ready.\nFile size: "<<size<<" bytes";

}

void Tables:: Build_EP(Cube3 &C){
    int(*ep)[10] = new int[967680][10];
    queue<pair<Cube3,int>> states;
    unordered_set<int> check_set;
    E.Orientation(C);
    int cur_ep = ((E.Hash_Phase2A() * 24) + E.Hash_Phase2B());
    cout<<endl<<"Initial EP = "<<cur_ep;
    states.push({C,cur_ep});
    check_set.insert(cur_ep);
    while(!states.empty()){
        Cube3 cur=states.front().first;
        cur_ep = states.front().second;
            states.pop();
            for(int i=0;i<10;i++){
                cur.move(moves[i]);
                E.Orientation(cur);
                int new_ep = ((E.Hash_Phase2A() * 24) + E.Hash_Phase2B());
                ep[cur_ep][i]=new_ep;
                if(!check_set.count(new_ep)){
                    check_set.insert(new_ep);
                    states.push({cur,new_ep});
                }
                cur.move(inverse[i]);
            }
    }
    cout<<endl<<"States reached: "<<check_set.size();
    cout<<endl<<"EP ready, writing to file.";
    ofstream file("../Databases/EP.bin", ios::binary);
    file.write((char*)ep, sizeof(int)*967680*10);
    uintmax_t size = filesystem::file_size("../Databases/EP.bin");
    file.close();
    cout<<endl<<"EP file ready.\nFile size: "<<size<<" bytes";

}

int main(){
    Tables T;
    Cube3 C;
    C.init();
    cout<<"Started";
    T.Build_CO(C);
    cout<<endl<<endl;
    T.Build_EO(C);
    cout<<endl<<endl;
    T.Build_UDS(C);
    cout<<endl<<endl;
    T.Build_CP(C);
    cout<<endl<<endl;
    T.Build_EP(C);
    cout<<endl<<"Done";
}
