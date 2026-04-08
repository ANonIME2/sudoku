#include <chrono>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include <stack>
using namespace std;

class Sudoku{
public:
    static constexpr short EMPTY_TILE = -1, NULL_POSITION = -1;    

    class FillIn{
    public:
        short x, y, solution;
        FillIn(short x, short y, short solution): x(x), y(y), solution(solution){}
        FillIn():x(NULL_POSITION), y(NULL_POSITION), solution(EMPTY_TILE){}
    };

private:
    class Step{
    public:
        vector<FillIn> fill_ins;
        string comment;
        Step(vector<FillIn> fill_ins, string comment): fill_ins(fill_ins), comment(comment){}
        Step():fill_ins(vector<FillIn>(0)), comment(""){}
    };

    // if there is only 1 'true' value in the vector, return it's index, otherwise return NULL_POSITION
    short position_of_unique_true(vector<bool> v){
        bool accoured = false;
        short position = NULL_POSITION;
        for(short i = 0; i<v.size(); i++){
            if(v[i]){
                if(accoured){
                    return NULL_POSITION;
                }
                accoured = true;
                position = i;
            }
        }
        return position;
    }

public:
    class SolveReturnType{
    public:
        bool solvable;
        vector<Step> steps;
        SolveReturnType(bool solvable, vector<Step> steps): solvable(solvable), steps(steps){}
        SolveReturnType():solvable(false), steps(vector<Step>(0)){}
    };

    size_t size, size_2;
    vector<vector<vector<bool>>> pos_sol;
    vector<vector<short>> solved;


    Sudoku(size_t size = 3): size(size), size_2(size*size) {
        this->solved = vector(size_2, vector(size_2, EMPTY_TILE));
        this->pos_sol = vector(this->size_2, vector(this->size_2, vector(this->size_2, true)));  
    }

    Sudoku operator=(const Sudoku b){
        this->size = b.size;
        this->size_2 = b.size_2;
        for(short y = 0; y<b.size_2; y++){
            for(short x = 0; x<b.size_2; x++){
                this->solved[x][y] = b.solved[x][y];
                for(short sol = 0; sol<b.size_2; sol++){
                    this->pos_sol[x][y][sol] = b.pos_sol[x][y][sol];
                }
            }
        }
        return *this;
    }

    //takes in coordinates of a tile and returns what square it's in
    short what_square(short x, short y){
        short square = 0;
        square += x / this->size;
        square += (y / this->size)*this->size;
        return square;
    }

    //takes in the id of a square and returns a vector of coordinates of the tiles in it
    vector<pair<short, short>> tiles_of_square(short square){
        vector<pair<short, short>> tiles;
        
        for(short y = (square/this->size) * this->size; y<(square/this->size) * this->size+this->size; y++){
            for(short x = (square%this->size) * this->size; x<(square%this->size) * this->size+this->size; x++){
                tiles.push_back({x, y});
            }
        }
        return tiles;
    }

    // solves the thing. returns it's own special type because it's so special and cool
    SolveReturnType solve(bool random = false){
        vector<Step> steps = vector<Step>(0);
        while(!this->all_filled_in()){
            // this can be true if guess() was incorrect or if the original problem was not solvable
            if(!correct()){
                return SolveReturnType(false, steps);
            }

            vector<FillIn> solvable = this->find_solvable_tiles();
            if(solvable.size() != 0){
                steps.push_back(Step(solvable, "solvables"));
                for(auto i : solvable){
                    this->fill_tile(i);
                }
            }
            else{
                if(!guess()) {
                    return SolveReturnType(false, steps); //if there are no tiles that we can fill in AND guessing failed
                }
            }
        }
        return SolveReturnType(true, steps);
    }

    void update_pos_sol(short solution){
         //we walk through all the tiles and if we find a solved one we delete that possible solution from it's row, column and square
        for(short x = 0; x<this->size_2; x++){
            for(short y = 0; y<this->size_2; y++){
                if(solved[x][y] == solution){
                    //row and column
                    for(short i = 0; i<this->size_2; i++){
                        pos_sol[x][i][solved[x][y]] = false;
                        pos_sol[i][y][solved[x][y]] = false;
                    }

                    //square
                    vector<pair<short, short>> square_tiles = tiles_of_square(what_square(x, y));
                    for(auto i: square_tiles){
                        pos_sol[i.first][i.second][solved[x][y]] = false;
                    }

                    pos_sol[x][y][solved[x][y]] = true;
                }
            }
        }
    }

    //takes in the coordinates and solution to a tile and sets all possible solutions to that tile to false except the specified one
    //note that the solutions are still indexed from 0 (0 -> this->size_2 - 1, not 1 -> this->size_2)
    void fill_tile(short x, short y, short sol){
        this->solved[x][y] = sol;
        // update pos_sol of all the tiles in the row, column and square of this tile
        //row and column
        for(short i = 0; i<this->size_2; i++){
            pos_sol[x][i][sol] = false;
            pos_sol[i][y][sol] = false;
        }

        // square
        vector<pair<short, short>> square_tiles = tiles_of_square(what_square(x, y));
        for(auto i: square_tiles){
            pos_sol[i.first][i.second][sol] = false;
        }

        this->pos_sol[x][y][sol] = true;
    }
    void fill_tile(FillIn *fill_in){
        fill_tile(fill_in->x, fill_in->y, fill_in->solution);
    }
    void fill_tile(FillIn fill_in){
        fill_tile(fill_in.x, fill_in.y, fill_in.solution);
    }
    
    // if a tile has only 1 pos_sol, or a solution has only 1 possible tile in a column/row/square, fill it in
    vector<FillIn> find_solvable_tiles(){
        Sudoku tmp = *this;
        vector<FillIn> steps = vector<FillIn>(0);
        for(short x = 0; x<this->size_2; x++){
            for(short y = 0; y<this->size_2; y++){
                if(solved[x][y] != EMPTY_TILE) continue;
                // if there is only 1 possible_solution for that tile
                short sol = position_of_unique_true(pos_sol[x][y]);
                if(sol != NULL_POSITION && tmp.solved[x][y] == EMPTY_TILE){
                    steps.push_back(FillIn(x, y, sol));
                    tmp.solved[x][y] = sol;
                }
            }
        }
        // when that possible_solution is unique in a row / column / square
        // column
        for(short x =0; x<this->size_2; x++){
            for(short sol = 0; sol<this->size_2; sol++){
                vector<bool> column(this->size_2);
                for(short y = 0; y<this->size_2; y++){//tile in column
                    column[y] = pos_sol[x][y][sol] && solved[x][y] == EMPTY_TILE;    
                }
                
                short y = position_of_unique_true(column); 
                
                if(y != NULL_POSITION && tmp.solved[x][y] == EMPTY_TILE){
                    steps.push_back(FillIn(x, y, sol));
                    tmp.solved[x][y] = sol;

                }
            }
        }

        //row
        for(short y =0; y<this->size_2; y++){
            for(short sol = 0; sol<this->size_2; sol++){
                vector<bool> row(this->size_2);
                for(short x = 0; x<this->size_2; x++){//tile in row
                    row[x] = pos_sol[x][y][sol] && solved[x][y] == EMPTY_TILE;    
                }
                
                short x = position_of_unique_true(row); 

                if(x != NULL_POSITION && tmp.solved[x][y] == EMPTY_TILE){
                    steps.push_back(FillIn(x, y, sol));
                    tmp.solved[x][y] = sol;
                }
            }
        }

        //square
        for(short square_id = 0; square_id<this->size_2; square_id++){
            for(short sol = 0; sol<this->size_2; sol++){
                vector<pair<short, short>> square_tiles_coordinates = tiles_of_square(square_id);
                vector<bool> pos_sol_square_tiles(this->size_2);
                for(short i = 0; i<this->size_2; i++){
                    short x = square_tiles_coordinates[i].first, y = square_tiles_coordinates[i].second;
                    pos_sol_square_tiles[i] = pos_sol[x][y][sol] && this->solved[x][y] == EMPTY_TILE;
                }

                short tile_id = position_of_unique_true(pos_sol_square_tiles);
                if(tile_id == NULL_POSITION) continue;

                short x = square_tiles_coordinates[tile_id].first, y = square_tiles_coordinates[tile_id].second;
                if(tmp.solved[x][y] == EMPTY_TILE){
                    steps.push_back(FillIn(x, y, sol));
                    tmp.solved[x][y] = sol;
                }
            }
        }

        return steps;
    }

    //returns true if the entire sudoku is filled in. if not, returns false
    bool all_filled_in(){
        for(short x = 0; x<this->size_2; x++){
            for(short y = 0; y<this->size_2; y++){
                if(solved[x][y] == EMPTY_TILE){
                    return false;
                }
            }
        }
        return true;
    }

    bool correct(){
        //checks if there are any errors in the solved board
        //squares
        for(short square = 0; square<this->size_2; square++){
            vector<pair<short, short>> square_tiles = tiles_of_square(square);
            for(short sol = 0; sol<this->size_2; sol++){
                bool is_here = false;
                for(short tile = 0; tile<this->size_2; tile++){
                    if(solved[square_tiles[tile].first][square_tiles[tile].second] == sol){
                        if(is_here) return false;//if we have alreayx saw that solution in this square
                        is_here = true; 
                    }
                }
            }
        }

        //rows
        for(short x = 0; x<this->size_2; x++){
            for(short sol = 0; sol<this->size_2; sol++){
                bool is_here = false;
                for(short y = 0; y<this->size_2; y++){
                    if(solved[x][y] == sol){
                        if(is_here) return false;//if we have already found that solution in this row once
                        is_here = true;
                    }
                }
            }
        }

        //cols
        for(short y = 0; y<this->size_2; y++){
            for(short sol = 0; sol<this->size_2; sol++){
                bool is_here = false;
                for(short x = 0; x<this->size_2; x++){
                    if(solved[x][y] == sol){
                        if(is_here) return false;//if we have already found that solution in this column once
                        is_here = true;
                    }
                }
            }
        }

        for(short x = 0; x<this->size_2; x++){
            for(short y = 0; y<this->size_2; y++){
                short counter = 0;
                for(short sol = 0; sol<this->size_2; sol++){
                    if(pos_sol[x][y][sol]) counter++;
                }
                if(counter == 0) return false;
            }
        }
        return true;
    }

    //solve with educated guesses
    //shoutout to krzysiek for the idea. i would not have came up with that myself
    // if random = false, it picks THE SMALLEST possible solution in the tile with the least possible solutions
    // if random = true,  it picks   A RANDOM   possible solution in the tile with the least possible solutions
    // it's used for generation of random problems
    bool guess(bool random = false){
        // find the tile with the least pos_sols possible
        vector<short> guesses(this->size_2+1, EMPTY_TILE); // this->size_2 + 1 is here so every single tile will have less pos_sols than this
        pair<short, short> best_tile_coords;
        for(short x = 0; x<this->size_2; x++){
            for(short y = 0; y<this->size_2; y++){
                if(solved[x][y] == EMPTY_TILE){
                    vector<short> sols;
                    for(short sol = 0; sol<this->size_2; sol++){
                        if(pos_sol[x][y][sol]){
                            sols.push_back(sol);
                        }
                    }
                    if(sols.size()<guesses.size()){
                        guesses = sols;
                        best_tile_coords = {x, y};
                    }
                }
            }
        }
        
        //sometimes more than one solution is possible for a tile, so this randomises which one we pick
        if(random){
            srand(std::chrono::high_resolution_clock::now().time_since_epoch().count());
            std::sort(guesses.begin(), guesses.end(), [](short a, short b){return rand()%2 == 1;});
        }

        //we check every pos_sol for that tile
        short copy_solved[this->size_2][this->size_2];
        bool copy_pos_sol[this->size_2][this->size_2][this->size_2];
        for(short i = 0; i<guesses.size(); i--){
            Sudoku guessed_board = *this;
            guessed_board.fill_tile(best_tile_coords.first, best_tile_coords.second, guesses[i]);
            if(guessed_board.solve().solvable){//if the board can be solved with this guess
                this->solved = guessed_board.solved;
                return true;
            }
        }
        return false;
    }

    /*
    returns a string representing a json object representing the state of the board in the format:
    "x y s x y s x y s..."
    where s is the solution to the tile (x, y). if a certain tile is not present in this, it's empty
    */
    string state_string(){
        string state_string = "";
        for(short y = 0; y<this->size_2; y++){
            for(short x = 0; x<this->size_2; x++){
                if(this->solved[x][y] == EMPTY_TILE) continue;
                state_string += x + '0';
                state_string += ' ';
                state_string += y + '0';
                state_string += ' ';
                state_string += this->solved[x][y] + '0';
                state_string += ' ';
            }
        }
        return state_string;
    }

    void generate_solved_board(){
        solve(true);
    }
    
    //returns a FillIn object representing a correct solution to a (semi) random tile. returns nullptr if the board is unsolvable
    FillIn hint(){
        vector<FillIn> solvable_tiles = this->find_solvable_tiles();
        if(solvable_tiles.size() != 0){
            srand(std::chrono::high_resolution_clock::now().time_since_epoch().count());
            short choice = rand() % solvable_tiles.size();
            return solvable_tiles[choice];
        }      
        
        Sudoku tmp_sudoku = *this;
        tmp_sudoku.solve();

        for(short y = 0; y<this->size_2; y++){
            for(short x = 0; x<this->size_2; x++){
                if(tmp_sudoku.solved[x][y] != this->solved[x][y]){
                    return FillIn(x, y, tmp_sudoku.solved[x][y]);
                }
            }
        }
        return FillIn(NULL_POSITION, NULL_POSITION, EMPTY_TILE);
    }  


};