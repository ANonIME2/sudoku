#include "Sudoku.h"

int main(int argc, char*argv[]){
    Sudoku problem;
    if(argc == 1){
        return -1;
    }
    
    std::cout<<problem.generate_problem(problem.size_2 * problem.size_2 - stoi(string(argv[1]))).state_string();
    return 0;
}