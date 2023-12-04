#include<bits/stdc++.h>
#include <chrono>
using namespace std;
using namespace std::chrono;
map<char,int>mp;
string address(string a){
    while(a.length()<8)a="0"+a;
    string ans="";
    for(auto v:a){
        int x=mp[v];
        string temp="";
        for(int j=0;j<4;j++)temp+=to_string(x&1),x/=2;
        reverse(temp.begin(),temp.end());
        ans+=temp;
    }
    return ans;
}
int logg(int n){
    int ans=0;
    while(n>1)ans++,n/=2;
    return ans;
}
int num(string s){
    reverse(s.begin(),s.end());
    int ans=0,cu=1;
    for(auto v:s){
        if(v=='1')ans+=cu;
        cu*=2;
    }
    return ans;
}
string binr(int n){
    string s="";
    if(n==0)return "0";
    while(n>0){
        s+=to_string(n%2);
        n/=2;
    }
    return s;
}
int L1_rmiss=0,L2_rmiss=0,L1_read=0,L2_read=0,L1_write=0,L2_write=0,L1_wmiss=0,L2_wmiss=0,L1_wb=0,L2_wb=0;
void printstat(){
    cout<<"L1 Reads are "<<L1_read<<"\n";
    cout<<"L1 Reads miss "<<L1_rmiss<<"\n";
    cout<<"L1 Writes are "<<L1_write<<"\n";
    cout<<"L1 write miss are "<<L1_wmiss<<"\n";
    cout<<"L1 miss rate is "<<L1_rmiss+L1_wmiss<<"/"<<L1_read+L1_write<<"\n";
    cout<<"L1 writeback are "<<L1_wb<<"\n"; 
    cout<<"L2 Reads are "<<L2_read<<"\n";
    cout<<"L2 Reads miss "<<L2_rmiss<<"\n";
    cout<<"L2 Writes are "<<L2_write<<"\n";
    cout<<"L2 write miss are "<<L2_wmiss<<"\n";
    cout<<"L2 miss rate is "<<L2_rmiss+L2_wmiss<<"/"<<L2_read+L2_write<<"\n";
    cout<<"L2 writeback are "<<L2_wb<<"\n";
}

int main(int argc,char* argv[]){
    if(argc!=7){
        cout<<"Invalid input\n";
        return 1;
    }
    auto start = high_resolution_clock::now();
    int BLOCKSIZE=atoi(argv[1]);
    int L1_SIZE=atoi(argv[2]);
    int L1_ASSOC=atoi(argv[3]);
    int L2_SIZE=atoi(argv[4]);
    int L2_ASSOC=atoi(argv[5]);
    //16384, 32768, 65536,131072, 262144, 
    L1_SIZE/=(BLOCKSIZE*L1_ASSOC);
    L2_SIZE/=(BLOCKSIZE*L2_ASSOC);
    int L1_index_bit=logg(L1_SIZE);
    int L2_index_bit=logg(L2_SIZE);
    int blk_offset=logg(BLOCKSIZE);
    string L1_Tag[L1_SIZE][L1_ASSOC];
    int    L1_use[L1_SIZE][L1_ASSOC];
    bool   dirty1[L1_SIZE][L1_ASSOC];
    string L2_Tag[L2_SIZE][L2_ASSOC];
    int    L2_use[L2_SIZE][L2_ASSOC];
    bool dirty2[L2_SIZE][L2_ASSOC];
    for(int i=0;i<L1_SIZE;i++)for(int j=0;j<L1_ASSOC;j++)L1_Tag[i][j]="",dirty1[i][j]=false,L1_use[i][j]=-1;
    for(int i=0;i<L2_SIZE;i++)for(int j=0;j<L2_ASSOC;j++)L2_Tag[i][j]="",dirty2[i][j]=false,L2_use[i][j]=-1;
    ifstream filer(argv[6]);
    for(int i=0;i<=9;i++)mp[to_string(i)[0]]=i;
    for(int i=0;i<6;i++)mp['a'+i]=10+i;
    int aa=0;
    string a,b;
    while(filer>>a>>b){
        string addr=address(b);
        string blk_num=addr;
        for(int i=0;i<blk_offset;i++)blk_num.pop_back();
        string L1_ind=blk_num.substr(blk_num.size()-L1_index_bit,L1_index_bit);
        string L1_tag=blk_num.substr(0,blk_num.size()-L1_index_bit);
        string L2_ind=blk_num.substr(blk_num.size()-L2_index_bit,L2_index_bit);
        string L2_tag=blk_num.substr(0,blk_num.size()-L2_index_bit);

        if(a=="r"){
            L1_read++;
            int find=0,old=0;
            int L1_intind=num(L1_ind);
            for(int j=0;j<L1_ASSOC;j++){
                if(L1_use[L1_intind][j]<L1_use[L1_intind][old])old=j;
                if(L1_Tag[L1_intind][j]==L1_tag){
                    L1_use[L1_intind][j]=aa;
                    find=1;
                    break;
                }
            }
            if(find==0){
                L1_rmiss++;
                L2_read++;
                if(dirty1[L1_intind][old]){
                    L1_wb++;
                    L2_write++;
                    string abt=L1_Tag[L1_intind][old]+L1_ind;
                    string xind=abt.substr(abt.size()-L2_index_bit,L2_index_bit),xtag=abt.substr(0,abt.size()-L2_index_bit);
                    for(int j=0;j<L2_ASSOC;j++){
                        if(L2_Tag[num(xind)][j]==xtag){
                            L2_use[num(xind)][j]=aa;
                            dirty2[num(xind)][j]=1;
                            find=1;
                            break;
                        }
                    }
                }
                find=0;
                L1_Tag[L1_intind][old]=L1_tag;
                L1_use[L1_intind][old]=aa;
                dirty1[L1_intind][old]=0;
                old=0;
                for(int j=0;j<L2_ASSOC;j++){
                    if(L2_use[num(L2_ind)][j]<L2_use[num(L2_ind)][old])old=j;
                    if(L2_Tag[num(L2_ind)][j]==L2_tag){
                        L2_use[num(L2_ind)][j]=aa;
                        find=1;
                        break;
                    }
                }
                if(find==0){
                    if(L2_use[num(L2_ind)][old]!=-1){
                        string abt=L2_Tag[num(L2_ind)][old]+L2_ind;
                        string xind=abt.substr(abt.size()-L1_index_bit,L1_index_bit),xtag=abt.substr(0,abt.size()-L1_index_bit);
                        for(int j=0;j<L1_ASSOC;j++){
                            if(L1_Tag[num(xind)][j]==xtag){
                                L1_Tag[num(xind)][j]="";
                                L1_use[num(xind)][j]=-1;
                                dirty1[num(xind)][j]=0;
                            }
                        }
                    }
                    L2_rmiss++;
                    if(dirty2[num(L2_ind)][old])L2_wb++;
                    L2_Tag[num(L2_ind)][old]=L2_tag;
                    L2_use[num(L2_ind)][old]=aa;
                    dirty2[num(L2_ind)][old]=0;
                }
            }
        }
        else{
            L1_write++;
            int find=0,old=0;
            int L1_intind=num(L1_ind);
            for(int j=0;j<L1_ASSOC;j++){
                if(L1_use[L1_intind][j]<L1_use[L1_intind][old])old=j;
                if(L1_Tag[L1_intind][j]==L1_tag){
                    L1_use[L1_intind][j]=aa;
                    dirty1[L1_intind][j]=1;
                    find=1;
                    break;
                }
            }
            if(find==0){
                L1_wmiss++;
                L2_read++;
                // int addr=L1_CACHE_Tag[L1_ind][old].first*L1_SIZE+L1_ind;
                if(dirty1[L1_intind][old]){
                    L1_wb++;
                    L2_write++;
                    string abt=L1_Tag[L1_intind][old]+L1_ind;
                    string xind=abt.substr(abt.size()-L2_index_bit,L2_index_bit),xtag=abt.substr(0,abt.size()-L2_index_bit);
                    for(int j=0;j<L2_ASSOC;j++){
                        if(L2_Tag[num(xind)][j]==xtag){
                            L2_use[num(xind)][j]=aa;
                            dirty2[num(xind)][j]=1;
                            break;
                        }
                    }
                }
                L1_Tag[L1_intind][old]=L1_tag;
                L1_use[L1_intind][old]=aa;
                dirty1[L1_intind][old]=1;
                old=0;
                for(int j=0;j<L2_ASSOC;j++){
                    if(L2_use[num(L2_ind)][j]<L2_use[num(L2_ind)][old])old=j;
                    if(L2_Tag[num(L2_ind)][j]==L2_tag){
                        L2_use[num(L2_ind)][j]=aa;
                        find=1;
                        break;
                    }
                }
                if(find==0){
                    if(L2_use[num(L2_ind)][old]!=-1){
                        string abt=L2_Tag[num(L2_ind)][old]+L2_ind;
                        string xind=abt.substr(abt.size()-L1_index_bit,L1_index_bit),xtag=abt.substr(0,abt.size()-L1_index_bit);
                        for(int j=0;j<L1_ASSOC;j++){
                            if(L1_Tag[num(xind)][j]==xtag){
                                L1_Tag[num(xind)][j]="";
                                L1_use[num(xind)][j]=-1;
                                dirty1[num(xind)][j]=0;
                            }
                        }
                    }
                    L2_rmiss++;
                    if(dirty2[num(L2_ind)][old])L2_wb++;
                    L2_Tag[num(L2_ind)][old]=L2_tag;
                    L2_use[num(L2_ind)][old]=aa;
                    dirty2[num(L2_ind)][old]=0;
                }
            }
        }

        aa++;
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    printstat();
    long long pp=L1_read+L1_write,qq=(L2_read+L2_write),rr=(L2_wmiss+L2_rmiss+L2_wb),twoh=200,twe=20;
    long long total=pp+qq*twe+rr*twoh;
    cout<<total<<"\n";
    cout << "Time taken by function: "<< duration.count() << " microseconds" << endl;
    return 0;
}