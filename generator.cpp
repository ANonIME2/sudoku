#include "Sudoku.h"

int main(){
    Sudoku problem;
    problem.generate_solved_board();
    problem.print_solution_to_file();
    return 0;
}