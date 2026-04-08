#include "Sudoku.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, char *argv[]){
    Sudoku problem;
    // if(argc != 2){
    //     std::cout<<"i need a command line argument with an absolute path to the input file";
    //     return 1;
    // }
    // const string INPUT_FILE = argv[1];
    const string INPUT_FILE = "D:\\coding_shit\\projects\\szkola\\algorytmika\\3klasa\\sudoku\\api\\algorithms\\input_state.txt"; //this is only here for debugging
    int n, y, x, s;
    ifstream in(INPUT_FILE);

    if(!in.good()){
        cout<<"problem while opening the file :("<<std::endl;
        return 1;
    }

    if(in.peek() != EOF){//check if the file is empty
        while(!in.eof()){
            in>>x>>y>>s;
            problem.fill_tile(x, y, s);
        }
    }
    
    ofstream out("dupa.json");

    Sudoku::SolveReturnType solve_msg = problem.solve();
    string state_json = "";
    
    state_json += "{";
        //state
        state_json += "\"state\":\"";
        state_json += problem.state_string();
        state_json += "\"";
    
        //solvable
        state_json += ", \"solvable\":";
        
        if(solve_msg.solvable){
            state_json += "true";
        }else{
            state_json += "false";
        }

        //steps
        string tmp_state_json = ", \"steps\":[";
        for(auto step : solve_msg.steps){
            //fill-ins
            tmp_state_json += "{\"fillIns\":\"";
                for(auto fill_in : step.fill_ins){
                    tmp_state_json += to_string(fill_in.x) + " ";
                    tmp_state_json += to_string(fill_in.y) + " ";
                    tmp_state_json += to_string(fill_in.solution) + " ";
                }
                tmp_state_json += "\"";
                //msg
                tmp_state_json += ",\"msg\":\"" + step.comment;  
            tmp_state_json += "\"}";
        }
        tmp_state_json += "]";


    state_json += tmp_state_json;

    state_json += "}";


    // std::cout<<problem.state_json();    
    out<<state_json;
    return 0;
}
