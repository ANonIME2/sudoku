#include "Sudoku.h"

int main(){
    Sudoku problem;
    problem.generate_solved_board();
    std::cout<<problem.state_string();
    return 0;
}