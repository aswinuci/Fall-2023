#include<iostream>
using namespace std;

vector<vector<int> > matrix_multiply_v1(vector<vector<int> > v1 , vector<vector<int> >v2){
    vector<vector<int> > v3(v1.size(),vector<int>(v2[0].size(),0));
    for(int i=0;i<v1.size();i++){
        for(int j=0;j<v1.size();j++){
            for(int k=0;k<v2.size();k++){
                v3[i][j] += v1[i][k]*v2[k][j];
            }
        }
    }
    return v3;
}

vector<vector<int> > matrix_multiply_v2(vector<vector<int> > v1 , vector<vector<int> >v2){
    vector<vector<int> > v3(v1.size(),vector<int>(v2[0].size(),0));
    for(int i=0;i<v1.size();i++){
        for(int j=i;j<v1.size();j++){
            for(int k=0;k<v2.size();k++){
                v3[i][j] += v1[i][j]*v2[j][k];
            }
        }
    }
    return v3;
}

void debug(vector<vector<int> > v3){
    for(auto row:v3){
        for(auto col:row){
            cout<<col<<" ";
        }
        cout<<endl;
    }
}

int main(int argc, char const *argv[])
{
    vector<vector<int> > v1 = {{1,2,3},{4,5,6},{7,8,9}};
    vector<vector<int> > v2 = {{2,0,0},{0,2,0},{0,0,2}};
    debug(matrix_multiply_v1(v1,v2));
    debug(matrix_multiply_v2(v1,v2));
    return 0;
}
