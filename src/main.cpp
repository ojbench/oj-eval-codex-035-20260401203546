#include <bits/stdc++.h>
#include "MyString.hpp"
using namespace std;
static string trim_quotes(const string& s){
    if (s.size()>=2 && ((s.front()=='"' && s.back()=='"') || (s.front()=='\'' && s.back()=='\'')))
        return s.substr(1, s.size()-2);
    return s;
}
int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    MyString S; string line, cmd;
    while (getline(cin, line)){
        if(line.empty()) continue; istringstream iss(line); if(!(iss>>cmd)) continue;
        if(cmd=="set"||cmd=="assign"){ string rest; getline(iss,rest); if(!rest.empty()&&rest[0]==' ') rest.erase(rest.begin()); rest=trim_quotes(rest); MyString tmp(rest.c_str()); S=std::move(tmp); }
        else if(cmd=="append"){ string rest; getline(iss,rest); if(!rest.empty()&&rest[0]==' ') rest.erase(rest.begin()); rest=trim_quotes(rest); S.append(rest.c_str()); }
        else if(cmd=="plus"||cmd=="add"){ string rest; getline(iss,rest); if(!rest.empty()&&rest[0]==' ') rest.erase(rest.begin()); rest=trim_quotes(rest); MyString R=S+MyString(rest.c_str()); cout<<R.c_str()<<"\n"; }
        else if(cmd=="print"||cmd=="cstr"){ cout<<S.c_str()<<"\n"; }
        else if(cmd=="size"){ cout<<S.size()<<"\n"; }
        else if(cmd=="capacity"){ cout<<S.capacity()<<"\n"; }
        else if(cmd=="reserve"){ size_t n; if(iss>>n) S.reserve(n); }
        else if(cmd=="resize"){ size_t n; if(iss>>n) S.resize(n); }
        else if(cmd=="at"){ size_t i; if(iss>>i){ try{ cout<<S.at(i)<<"\n"; } catch(...){ cout<<"OUT_OF_RANGE\n"; } } }
        else if(cmd=="get"){ size_t i; if(iss>>i){ try{ cout<<S[i]<<"\n"; } catch(...){ cout<<"OUT_OF_RANGE\n"; } } }
        else if(cmd=="setc"){ size_t i; string ch; if(iss>>i>>ch){ if(!ch.empty()){ try{ S[i]=ch[0]; } catch(...){} } } }
        else { MyString tmp(line.c_str()); S=std::move(tmp); }
    }
    return 0;
}
