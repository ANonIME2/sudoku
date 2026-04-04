#include "Sudoku.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, char *argv[]){
    Sudoku problem;
    if(argc != 2){
        std::cout<<"i need a command line argument. an input file would be nice :)";
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
    
    problem.solve();
    std::cout<<problem.state_string();    
    return 0;
}
