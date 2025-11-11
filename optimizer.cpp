#include <bits/stdc++.h>
using namespace std;

string path;

vector<int> get(string key){
    ifstream fin("default.cfg");
    string line;
    int prod = 1;
    vector<int> values;
    while(getline(fin, line)) {
        if(line.find(key) == 0){
            stringstream ss(line);
            string token;
            while (getline(ss, token, ':')){
                if(all_of(token.begin(), token.end(), ::isdigit)){
                    values.push_back(stoi(token));
                    prod *= values.back();
                }
                else if(values.size()==3) values.push_back(token[0]);
            }
            break;
        }
    }
    fin.close();
    values[0] = prod;
    swap(values[1],values[2]);
    return values;
}

void update_config(string key, const string &value){
    ifstream fin("config.cfg");
    stringstream buffer;
    string line;
    key.push_back(' ');
    while(getline(fin, line)) {
        if(line.find(key) == 0) buffer << key << "\t" << value << "\n";
        else buffer << line << "\n";
    }
    fin.close();
    ofstream fout("config.cfg");
    fout << buffer.str();
    fout.close();
}

float run_simulator(const string config = "config"){
    // Get baseline from void
    string cmd = "../installn_files/simplesim-3.0/sim-outorder -config " + config + ".cfg void > out.txt 2>&1";
    if(system(cmd.c_str())){
        cout<<"Unable to run sim-outorder on 'void' with "<<config<<".cfg"<<endl;;
        exit(1);
    }
    if(system("awk '/sim_num_insn/ {insn=$2} /sim_cycle/ {cycle=$2} END {print insn, cycle}' out.txt > tmp && mv tmp out.txt")){
        cout<<"awk 'sim_num_insn' and 'sim_cycle' to out.txt failed"<<endl;
        exit(2);
    }

    ifstream fin("out.txt");
    int base_inst, base_cycle;
    fin>>base_inst>>base_cycle;
    fin.close();

    // True IPC of code
    cmd = "../installn_files/simplesim-3.0/sim-outorder -config " + config + ".cfg " + path + " > out.txt 2>&1";
    if(system(cmd.c_str())){
        cout<<"Unable to run sim-outorder on '"<<path<<"' with "<<config<<".cfg"<<endl;;
        exit(3);
    }
    if(system("awk '/sim_num_insn/ {insn=$2} /sim_cycle/ {cycle=$2} END {print insn, cycle}' out.txt > tmp && mv tmp out.txt")){
        cout<<"awk 'sim_num_insn' and 'sim_cycle' to out.txt failed"<<endl;
        exit(4);
    }

    fin.open("out.txt");
    int inst, cycle;
    fin>>inst>>cycle;
    float IPC = (float)(inst-base_inst)/(cycle-base_cycle);
    fin.close();
    return IPC;
}

// <key> ,  <set of values>
vector<pair<string,vector<string>>> variables;

// Columns  = <Assoc>  ,    <block_size>  ,     <policy> 
// Rows     = dl1      il1      dl2      dtlb      itlb
vector<int> cache[5][3];
string mp[5] = {"dl1", "il1", "dl2", "dtlb", "itlb"};
int product[5];

void Parameters(string &params){
    ifstream fin(params.c_str());
    string line;
    while(getline(fin, line) && (!line.empty())){
        stringstream ss(line);
        string token;
        ss>>token;
        variables.push_back({token,{}});
        while(ss>>token) variables.back().second.push_back(token);
    }
    int i;
    for(i=0;i<5 && getline(fin,line);i++){
        line.push_back(' ');
        vector<int> def = get(((i<=2)? "-cache:":"-tlb:") + mp[i]);
        product[i] = def[0];
        int j=0;
        string token = "";
        for(char c:line){
            if(c==' '){
                if(token.empty()) j++;
                else if(token=="0") cache[i][j]={def[j+1]};
                else if(j==2) cache[i][j].push_back(token[0]);
                else cache[i][j].push_back(stoi(token));
                token="";
            }
            else token.push_back(c);
        }
    }
    for(;i<5;i++){
        vector<int> def = get(((i<=2)? "-cache:":"-tlb:") + mp[i]);
        product[i] = def[0];
        for(int j=0;j<3;j++) cache[i][j] = {def[j+1]};
    }
    fin.close();
}

vector<string> current_state, optimal_state, worst_state;
float best_IPC = 0, worst_IPC = 2e9;

float optimal_cache(int i){
    if(i==5){
        float ipc = run_simulator();
        if(ipc > best_IPC){
            best_IPC = ipc;
            optimal_state = current_state;
        }
        if(ipc < worst_IPC){
            worst_IPC = ipc;
            worst_state = current_state;
        }
        return ipc;
    }
    else{
        string key = ((i<=2)? "-cache:":"-tlb:") + mp[i];
        float max1 = 0;
        bool state1 = cache[i][0].size()>1;
        bool state  = ((cache[i][1].size()>1) || cache[i][2].size()>1);
        for(char policy:cache[i][2]){
            if(state1) current_state.push_back(mp[i]+":Policy  =   " + policy);
            float ipc = 0;
            for(int &associativity:cache[i][0]){
                if(state) current_state.push_back(mp[i]+":Associativity   =   " + to_string(associativity));
                for(int &block_size:cache[i][1]){
                    int nsets = product[i]/(associativity*block_size);
                    if(state){
                        current_state.push_back(mp[i]+":Block_size   =   " + to_string(block_size));
                        current_state.push_back(mp[i]+":nsets   =   " + to_string(nsets));
                    }
                    string value = mp[i]+":"+to_string(nsets)+':'+to_string(block_size)+':'+to_string(associativity)+':'+policy;
                    if(i==2) value[0]='u';
                    update_config(key, value);
                    ipc = max(ipc, optimal_cache(i+1));
                    if(state){
                        current_state.pop_back();
                        current_state.pop_back();
                    }
                }
                if(state) current_state.pop_back();
            }
            if(state1){
                if(variables.size()<2 && (i==0)){
                    cout<<policy<<" = "<<ipc<<" | ";
                    fflush(stdout);
                }
                current_state.pop_back();
            }
            max1 = max(max1, ipc);
        }
        return max1;
    }
}

float find_best_IPC(int i){
    if(i==variables.size()) return optimal_cache(0);
    else{
        string key = variables[i].first;
        float mx = 0;
        for(string &value:variables[i].second){
            current_state.push_back(value);
            update_config(key, value);
            if(i==0) printf("\n%-10s: ",value.c_str());
            float ipc = find_best_IPC(i+1);
            mx = max(mx, ipc);
            if(i==1){
                cout<<value<<"   =   "<<ipc<<" |   ";
                fflush(stdout);
            }
            current_state.pop_back();
        }
        return mx;
    }

}

int main(){
    cout<<"\nEnter the relative path to your .c code: ";
    cin>>path; cout<<endl;
    // Compile the code
    string cmd = "../installn_files/bin/sslittle-na-sstrix-gcc -O3 -o "+path+' '+path+".c";
    if(system(cmd.c_str())){
        cout<<"Cross-compilation of "<<path<<".c failed"<<endl;
        return 5;
    }
    if(system("cp default.cfg config.cfg")){
        cout<<"File copy from default.cfg to config.cfg failed"<<endl;
        return 6;
    }
    // Get range of parameters
    cout<<"Enter the relative path to parameters file: ";
    string params;
    cin>>params;
    Parameters(params);

    cout<<fixed<<setprecision(4);
    float baseline_IPC = run_simulator("default");
    cout<<"\nBaseline IPC = "<<baseline_IPC<<endl<<endl;

    cout<<"Grid Searching for best IPC:-"<<endl;
    auto start=chrono::high_resolution_clock::now();
    find_best_IPC(0);
    auto end=chrono::high_resolution_clock::now();

    cout<<"\n\nBest IPC: "<<best_IPC<<endl;
    cout<<"\nOptimal conditions:-\n";
    int i=0;
    for(;i<variables.size();i++) if(variables[i].second.size()>1) cout<<variables[i].first<<" =   "<<optimal_state[i]<<endl;
    for(;i<optimal_state.size();i++) cout<<optimal_state[i]<<endl;

    cout<<"\nWorst IPC: "<<worst_IPC<<endl;
    
    cout<<"\nSimulation Time (s): "<<chrono::duration_cast<chrono::seconds>(end - start).count()<<endl<<endl;
    return 0;
}