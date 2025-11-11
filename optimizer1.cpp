#include <bits/stdc++.h>
using namespace std;

string path;
vector<string> branch_predictors = {"nottaken","taken","perfect","bimod","2lev"};
vector<string> replacement_policies = {"l", "f", "r"};

int get_cache_size(const string key){
    ifstream fin("default.cfg");
    string line;
    int size = 0;
    while(getline(fin, line)) {
        if(line.find(key) == 0){
            stringstream ss(line);
            string token;
            while (getline(ss, token, ':')){
                if(all_of(token.begin(), token.end(), ::isdigit))
                    size += __builtin_ctz(stoi(token));
            }
            break;
        }
    }
    fin.close();
    return size;
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
    string cmd = "/home/goku/installn_files/simplesim-3.0/sim-outorder -config " + config + ".cfg void > out.txt 2>&1";
    system(cmd.c_str());
    system("awk '/sim_num_insn/ {insn=$2} /sim_cycle/ {cycle=$2} END {print insn, cycle}' out.txt > tmp && mv tmp out.txt");

    ifstream fin("out.txt");
    int base_inst, base_cycle;
    fin>>base_inst>>base_cycle;
    fin.close();

    // True IPC of code
    cmd = "/home/goku/installn_files/simplesim-3.0/sim-outorder -config " + config + ".cfg " + 
                  path + " > out.txt 2>&1";
    system(cmd.c_str());
    system("awk '/sim_num_insn/ {insn=$2} /sim_cycle/ {cycle=$2} END {print insn, cycle}' out.txt > tmp && mv tmp out.txt");

    fin.open("out.txt");
    int inst, cycle;
    fin>>inst>>cycle;
    float IPC = (float)(inst-base_inst)/(cycle-base_cycle);
    fin.close();
    return IPC;
}

int main(){
    cout<<"Enter the relative path to your code binary: ";
    cin>>path; cout<<endl;
    // Compile the code
    string cmd = "/home/goku/installn_files/bin/sslittle-na-sstrix-gcc -O3 -o "+path+' '+path+".c";
    system(cmd.c_str());

    cout<<fixed<<setprecision(4);
    float baseline_IPC = run_simulator("default");
    cout<<"Baseline IPC = "<<baseline_IPC<<endl<<endl;

    int L1 = get_cache_size("-cache:dl1");
    float best_IPC = 0;
    string best_predictor, best_policy;
    int best_associativity, best_block_size, best_nsets;
    cout<<"Grid Search IPC:-\n"<<endl;
    
    auto start=chrono::high_resolution_clock::now();
    for(string &predictor: branch_predictors){
        update_config("-bpred", predictor);
        printf("%-10s: ",predictor.c_str());
        for(string &policy: replacement_policies){
            update_config("-cache:il1", "il1:512:32:1:" + policy);
            update_config("-cache:dl2", "ul2:1024:64:4:" + policy);
            float cur_ipc = 0;
            for(int a = 0; a<=3; a++){
                for(int b = 4; b<=7; b++){
                    int associativity = 1<<a;
                    int block_size = 1<<b;
                    int nsets=1<<(L1-a-b);
                    string cache = "dl1:"+to_string(nsets)+':'+to_string(block_size)+':'+to_string(associativity)+':';
                    update_config("-cache:dl1",cache + policy);
                    float ipc = run_simulator();
                    cur_ipc = max(cur_ipc,ipc);
                    if(ipc > best_IPC){
                        best_IPC = ipc;
                        best_predictor = predictor;
                        best_policy = policy;
                        best_associativity = associativity;
                        best_block_size = block_size;
                        best_nsets = nsets;
                    }
                }
            }
            cout<<policy<<" = "<<cur_ipc<<" | ";
            fflush(stdout);
        }
        cout<<endl;
    }
    auto end=chrono::high_resolution_clock::now();

    cout<<"\nBest IPC: "<<best_IPC<<endl;
    cout<<"\nOptimal conditions:-\n";
    cout<<"Predictor - "<<best_predictor<<"\n";
    cout<<"Policy - "<<best_policy<<"\n";
    cout<<"Associativity - "<<best_associativity<<"\n";
    cout<<"Block size - "<<best_block_size<<"\n";
    cout<<"nsets - "<<best_nsets<<"\n"<<endl;

    cout<<"Simulation Time (s): "<<chrono::duration_cast<chrono::seconds>(end - start).count()<<endl<<endl;
    return 0;
}