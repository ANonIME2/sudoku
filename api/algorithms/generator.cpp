#include "Sudoku.h"

int main(){
    Sudoku problem;
    std::cout<<problem.generate_problem(40).state_string();
    return 0;
}