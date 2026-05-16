#include "Sudoku.h"
#include <chrono>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <vector>
int main(){
    Sudoku sudoku;  
    auto start = std::chrono::high_resolution_clock::now();

    sudoku.solved = sudoku.generate_problem_using_hints(0.3);

    for(int i = 0; i<50; i++)
        std::cout<<i<<" "<<sudoku.state_string()<<std::endl;

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;    
    std::cout<<duration.count();
    return 0;
}