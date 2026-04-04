#include "Sudoku.h"
using namespace std;

int main(int argc, char* argv[]){
    Sudoku problem;
    // if(argc != 2){
    //     std::cout<<"i need a command line argument. an input file would be nice.";
    //     return 1;
    // }
    // const string INPUT_FILE = argv[1];
    const string INPUT_FILE = "D:\\coding_shit\\projects\\szkola\\algorytmika\\3klasa\\sudoku\\api\\algorithms\\input_state.txt";
    int n, y, x, s;
    ifstream in(INPUT_FILE);

    if(!in.good()){
        cout<<"problem while opening the file :("<<std::endl;
        return 1;
    }
    
    while(!in.eof()){
        // problem.output_solved();     
        in>>x>>y>>s;
        problem.fill_tile(x, y, s);
    }

    //get the hint
    Sudoku::FillIn hint = problem.hint();
    //if the board is unsolvable
    if(hint.solution == Sudoku::EMPTY_TILE){
        std::cout<<problem.to_json();
        return 0;
    }

    
    Sudoku tmp = problem;
    tmp.fill_tile(&hint);
    std::cout<<tmp.to_json();

    return 0;
}