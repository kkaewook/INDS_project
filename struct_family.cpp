#include <iostream>
#include <vector>
#include <queue>
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
    int gender;
    int nodenum; // 이름 -> int로 저장했다가 인덱싱으로 이름 찾기
    string name;
public:
    vector<pair<int,int>> relation; //연결되어있는 노드의 주소 및 관계 값
    man(string name,int _gender);
    void insertMan(pair<int,int> addr_and_rel){
        relation.push_back(addr_and_rel);
    };
    int getGender(){return gender;};
    int getNodeno(){return nodenum;};
    string getName(){return name;};
    pair<int,int> getLastMember();
    void updateName(string _name){name = _name;};
    
};
pair<int,int> man::getLastMember(){

    pair<int,int> result = make_tuple(relation.back().first,relation.back().second);
    return result;
}
man::man(string _name,int _gender){
    nodenum = ++_index;
    gender = _gender;
    name = _name;
}
class Graph{
private:
    vector<man> family;
public:
    Connection con;
    Graph();    //사용자(자신) 추가
    void addMember();
    void updateMember();
    void findMember();
    void sql_Member();
    int find(int nodenum, queue<int>que, int* visited);
    void show_Member();
};
void Graph::addMember(){
    show_Member();
    
    cout<<"input Standard of new family member : ";
    int n; cin>>n;
    int rel;
    int k = family[n-1].getNodeno();
    
    for(vector<man>::iterator iter = family.begin(); iter != family.end(); iter++){
        int temp = iter->getNodeno();
        
        if(temp == k){
            cout<<"input name : ";
            string _name;cin>>_name;         //to enroll name
            cout<<"input relation(1:dad,2:mom,3:son,4:daughter,5:spouse) : ";
            int relation;cin>>relation;     //test case 1
            int gender = relation%2;        //0 is male, 1 is female
            
            if(relation == 5)
                gender = !(iter->getGender());
            man member = man(_name,gender);       //관계가 5면 1. 현재 성별과비교, 현재 성별이 0이면 1, 1이면 0
            
            int target_index = _index;
            pair<int,int> _addr_rel = make_tuple(target_index,relation);
            iter->insertMan(_addr_rel);
            
            if(relation <=2 && relation >=1){
                rel = 3+iter->getGender();
                _addr_rel = make_tuple(iter->getNodeno(),3+iter->getGender());
            }else if(relation<=4 && relation >=1){
                rel = 3+iter->getGender();
                _addr_rel = make_tuple(iter->getNodeno(),1+iter->getGender());
            }else if(relation == 5){
                rel = 3+iter->getGender();
                _addr_rel = make_tuple(iter->getNodeno(),5);
            }else{
                cout<<"Error : input 1~5"<<endl;
                continue;
            }
            
            member.insertMan(_addr_rel);
            
            cout<<"기준이 되는 구성원: "<<iter->getNodeno()<<endl;
            cout<<"추가 구성원의 노드 번호 : "<<member.getNodeno()<<endl;
            cout<<"기준 구성원 -> 추가 구성원의 관계 : "<<get<1>(iter->getLastMember())<<endl;
            cout<<"추가 구성원 -> 기준 구성원의 관계 : "<<get<1>(member.getLastMember())<<endl;
            
            family.push_back(member);
            
            //====================================================================
            // 구성원 추가
            //====================================================================
            Query query = con.query();
            query << "insert into member (name,gender) values ('"<<_name<<"'"<<","<<gender<<")";
            query.store();
            
            //====================================================================
            // 관계 추가
            //====================================================================
            query << "insert into relation (origin_idx,new_idx,origin_rel,new_rel) values ("<<member.getNodeno()<<","<<iter->getNodeno()<<","<<relation<<","<<rel<<")";
            query.store();
            
            query << "insert into relation (origin_idx,new_idx,origin_rel,new_rel) values ("<<iter->getNodeno()<<","<<member.getNodeno()<<","<<rel<<","<<relation<<")";
            query.store();
            
            return;
        }
    }
    cout<<"Error : doesn't exist member that"<<endl;
}
Graph::Graph(){

    try{
        con.connect(DB_NAME,SERVER,USER,PWD);
        cout<< "Connected"<<endl;
    }catch(Exception &e){
        cout<< e.what()<<endl;
    }
    
    //======수정할부분
    Query query = con.query();
    query <<"select * ordered by idx from member";
    StoreQueryResult init_index = query.store();
    _index = (int)init_index.num_rows();
    
    query <<"select count(*) from member";
    StoreQueryResult res = query.store();
    if(res){
        
    }else{
        cout<<"input your name : ";
        string _name;
        cin>>_name;
        int g;
        cout<<"input your gender(0:male, 1:female) : ";
        cin>>g;
        man me(_name,g);
        family.push_back(me);
    }
}
void Graph::show_Member(){
    vector<man>::iterator iter;
    int num = 1;
    cout<<"========= Family Member ========="<<endl;
    for(iter = family.begin(); iter!=family.end(); iter++){
        cout<<num++<<". "<<iter->getName()<<endl;
    }
}

void Graph::sql_Member(){
    
    Query query = con.query();
    query <<"select * from member";
    StoreQueryResult res = query.store();
    
    for(size_t i = 0;i<res.num_rows();i++){
        man member = man(string(res[i]["name"]),res[i]["gender"]);
        cout<<"num : "<<res[i]["idx"] <<" name : "<<res[i]["name"]<<endl;
        Query query1 = con.query();
        query1 << "select * from relation where origin_idx = "<<res[i]["idx"];
        StoreQueryResult res_1 = query1.store();
        
        for (size_t j =0 ;j < res_1.num_rows();j++){
            pair<int,int> temp_pair;
            temp_pair = make_tuple(int(res_1[j]["origin_rel"]),int(res_1[j]["new_rel"]));
            member.relation.push_back(temp_pair);
        }
        family.push_back(member);
    }
}
void Graph::updateMember(){
    show_Member();
    cout<<"input member num to update name : ";
    int n;
    cin>>n;
    cout<<"input new name : ";
    string new_name;
    cin>>new_name;
    
    family[n-1].updateName(new_name);
    n = family[n-1].getNodeno();
    
    Query query1 = con.query();
    query1 << "update member set name='"<<new_name<<"' where idx="<<n;
    query1.store();
    
    cout<<"== update completed =="<<endl;
}

int Graph::find(int nodenum,queue<int>que,int* visited){
    //nodenum : 찾아야할 노드번호
    int _count = 0;
    
    while(true){
        if(que.size() == 0)
            return 0;
        
        int cur_node = que.front();
        que.pop();
        
        if(cur_node == nodenum) //노드를 찾음
            return _count+1;
        if(visited[cur_node-1] == 1)    //이미 방문한 노드
            continue;
        else
            visited[cur_node-1] = 1;    //방문 표시
        
        if(family[cur_node-1].relation.size()==0)
            return 0;
        else{
            _count++;
            for(int i = 0; i<family[cur_node-1].relation.size(); i++){
                que.push(get<1>(family[cur_node-1].relation[i]));
            }
        }
    }
}
void Graph::findMember(){
    
    string input_name;
    cout<<"input member name to find : ";
    cin>>input_name;
    
    int me = family.begin()->getNodeno();
    
    for(vector<man>::iterator iter = family.begin(); iter != family.end(); iter++){
        string temp = iter->getName();
        if(temp == input_name){
            int size = (int)family.size()+1;
            int nodenum = iter->getNodeno();
            
            queue<int> que;
            int* visited = new int[size];
            
            for(int i = 0; i<size+1;i++){
                visited[i] = 0;
            }
            //노드번호 인덱스1부터시작, visited배열은 0부터 시작
            //큐와 방문배열에 첫번째 원소(나)의 정보 넣기
            visited[me-1] = 1;
            que.push(me);
            cout<<find(nodenum, que, visited)<<"촌 관계입니다."<<endl;
        }
    }
}

int main(int argc, const char * argv[]){
    
    Graph* Grp = new Graph;

    int select;
    Grp->sql_Member();
    
    Grp->show_Member();
    while(true){
        cout<<"========= MENU ========="<<endl;
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
