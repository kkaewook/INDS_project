#include <iostream>
#include <vector>
#include <cstdio>
#include <tuple>
#include "mysql.h"
#include "mysql++.h"

#define dad 1;
#define mom 2;
#define son 3;
#define dht 4;
#define spo 5;

#define DB_NAME "konkuk_tree"
#define SERVER "222.239.251.76"
#define USER "konkuk"
#define PWD "Konkuk1212!@!@"

using namespace std;
using namespace mysqlpp;

int _index = 0; //전체 멤버의 인덱스(추가시 1증가)

class man{
private:
    bool gender;
    int nodenum; // 이름 -> int로 저장했다가 인덱싱으로 이름 찾기
    vector<pair<man*,int>> addr; //연결되어있는 노드의 주소 및 관계 값
    string name;
public:
    man(string name,int sex);
    void insertMan(pair<man*,int> addr_and_rel){
        addr.push_back(addr_and_rel);
    };
    bool getGender(){return gender;};
    int getNodeno(){return nodenum;};
    man* getmyAddr(){return this;};
    pair<man*,int> getLastMember();
};
pair<man*,int> man::getLastMember(){
    //vector<pair<man*,int>>::iterator iter = addr.end();
    //pair<man*,int>result = make_tuple(iter->first,iter->second);
    
    pair<man*,int> result = make_tuple(addr.back().first,addr.back().second);
    return result;
}
man::man(string _name,int sex){
    nodenum = ++_index;
    gender = sex;
    name = _name;
}
class Graph{
    vector<man> family;
public:
    Graph();    //사용자(자신) 추가
    void addMember();
    void updateMember();
    void findMember();
    void sql_Member();
};
Graph::Graph(){
    Connection con(true);
    try{
        con.connect(DB_NAME,SERVER,USER,PWD);
        cout<< "Connected"<<endl;
    }catch(Exception &e){
        cout<< e.what()<<endl;
    }
    
    cout<<"input your name : ";
    string _name;
    cin>>_name;
    int g;
    cout<<"input your gender(0:male, 1:female) : ";
    cin>>g;
    man me(_name,g);
    family.push_back(me);
}
void Graph::addMember(){
    cout<<"input Standard of new family member : ";
    int n;
    cin>>n;
    
    for(vector<man>::iterator iter = family.begin(); iter != family.end(); iter++){
        int temp = iter->getNodeno();
        
        if(temp == n){
            cout<<"input name : ";
            string _name;cin>>_name;         //to enroll name
            cout<<"input relation(1:dad,2:mom,3:son,4:daughter,5:spouse) : ";
            int relation;cin>>relation;     //test case 1
            int gender = relation%2;        //0 is male, 1 is female
            
            if(relation == 5)
                gender = !(iter->getGender());
            man member = man(_name,gender);       //관계가 5면 1. 현재 성별과비교, 현재 성별이 0이면 1, 1이면 0
            
            pair<man*,int> _addr_rel = make_tuple(&member,relation);
            
            (iter->getmyAddr())->insertMan(_addr_rel);
            
            if(relation <=2 && relation >=1)
                _addr_rel = make_tuple(iter->getmyAddr(),3+iter->getGender());
            else if(relation<=4 && relation >=1)
                _addr_rel = make_tuple(iter->getmyAddr(),1+iter->getGender());
            else if(relation == 5)
                _addr_rel = make_tuple(iter->getmyAddr(),5);
            else{
                cout<<"Error : input 1~5"<<endl;
                continue;
            }
            member.insertMan(_addr_rel);
            /*
            cout<<"기준이 되는 구성원: "<<(iter->getmyAddr())->getNodeno()<<endl;
            cout<<"추가 구성원의 노드 번호 : "<<member.getNodeno()<<endl;
            cout<<"기준 구성원 -> 추가 구성원의 관계 : "<<get<1>((iter->getmyAddr())->getLastMember())<<endl;
            cout<<"추가 구성원 -> 기준 구성원의 관계 : "<<get<1>(get<0>((iter->getmyAddr())->getLastMember())->getLastMember())<<endl;
            */
            family.push_back(member);
            return;
        }
    }
    cout<<"Error : doesn't exist member that"<<endl;
}
void Graph::sql_Member(){
    Connection con(true);
    try{
        con.connect(DB_NAME,SERVER,USER,PWD);
        cout<< "Connected"<<endl;
    }catch(Exception &e){
        cout<< e.what()<<endl;
    }
    
    Query query = con.query();
    query <<"select * from member";
    StoreQueryResult res = query.store();
    for(size_t i = 0;i<res.num_rows();i++)
        cout<<"num : "<<res[i]["idx"] <<" name : "<<res[i]["name"]<<endl;
}
void Graph::updateMember(){
}
void Graph::findMember(){
    string input_name;
    cout<<"input member name : ";
    cin>>input_name;
    
    
}
int main(int argc, const char * argv[]){
    
    Graph* Grp = new Graph;
    //=================
    //=====DB TEST=====
    

    /*
    Query query = con.query();
    query <<"insert into test values(0,'jaehyuk');";
    query.execute();
    
    query <<"select * from test;";
    StoreQueryResult res = query.store();
    for(size_t i = 0;i<res.num_rows();i++)
        cout<<"num : "<<res[i]["num"] <<" name : "<<res[i]["name"]<<endl;
    */
    int select;
    while(true){
        cout<<"=== MENU ===="<<endl;
        cout<<"1. add Member"<<endl;
        cout<<"2. update Member"<<endl;
        cout<<"3. find Member"<<endl;
        cout<<"4. EXIT"<<endl;
        cin>>select;
        
        switch(select){
            case 1:
                Grp->addMember();
                break;
            case 2:
                Grp->updateMember();
                break;
            case 3:
                Grp->findMember();
                break;
            default:
                exit(1);
                break;
        }
    }
}
