#include "Sudoku.h"

int main(){
    Sudoku problem;
    problem.generate_problem_with_guesses(40);
    std::cout<<problem.state_string();
    return 0;
}