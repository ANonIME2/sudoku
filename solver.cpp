#include "Sudoku.h"

int main(){
    Sudoku problem;
    int n, y, x, s;
    ifstream in("state.txt");
    
    while(!in.eof()){
        // problem.output_solved();     
        in>>x>>y>>s;
        problem.fill_tile(x, y, s);
    }

    if(!problem.correct()){
        problem.print_solution_to_file();
    }

    problem.solve();
    problem.print_solution_to_file();    
    return 0;
}
