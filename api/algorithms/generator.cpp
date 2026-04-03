#include "Sudoku.h"

int main(){
    Sudoku problem;
    problem.generate_solved_board();
    std::cout<<problem.to_json();
    return 0;
}