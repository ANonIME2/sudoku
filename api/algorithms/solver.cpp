#include "Sudoku.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, char *argv[]){
    Sudoku problem;
    if(argc != 2){
        std::cout<<"i need a command line argument with an absolute path to the input file";
        return 1;
    }
    const string INPUT_FILE = argv[1];
    // const string INPUT_FILE = "D:\\coding_shit\\projects\\szkola\\algorytmika\\3klasa\\sudoku\\api\\algorithms\\input_state.txt"; //this is only here for debugging
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
        state_json += Sudoku::steps_json(solve_msg.steps);

    state_json += "}";


    std::cout<<state_json;    

    return 0;
}
