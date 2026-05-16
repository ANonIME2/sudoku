#include <chrono>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include <stack>
#include <fstream>
#include <random>
using namespace std;

class Sudoku{
public:
    static constexpr short EMPTY_TILE = -1, NULL_POSITION = -1;    
    size_t size, size_2;
    vector<vector<vector<bool>>> pos_sol;
    vector<vector<short>> solved;

    class FillIn{
    public:
        short x, y, solution;
        FillIn(short x, short y, short solution): x(x), y(y), solution(solution){}
        FillIn():x(NULL_POSITION), y(NULL_POSITION), solution(EMPTY_TILE){}
    };

    class Step{
    public:
        vector<FillIn> fill_ins;
        string comment;
        Step(vector<FillIn> fill_ins, string comment): fill_ins(fill_ins), comment(comment){}
        Step():fill_ins(vector<FillIn>(0)), comment(""){}
    };

    class SolveReturnType{
    public:
        bool solvable;
        vector<Step> steps;
        SolveReturnType(bool solvable, vector<Step> steps): solvable(solvable), steps(steps){}
        SolveReturnType():solvable(false), steps(vector<Step>(0)){}
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

    short how_many_empty_tiles(){
        short counter = 0;
        for(short x = 0; x<size_2; x++){
            for(short y = 0; y<size_2; y++){
                if(solved[x][y] == EMPTY_TILE) counter++;
            }
        }
        return counter;
    }

    // solves the thing. returns it's own special type because it's so special and cool
    SolveReturnType solve(vector<Step> steps = vector<Step>(0), bool random = false, short how_many_tiles_leave_empty = 0){
        while(how_many_empty_tiles() > how_many_tiles_leave_empty){
            if(!correct()){
                return SolveReturnType(false, steps);
            }

            vector<FillIn> solvable = this->find_solvable_tiles();
            if(solvable.size() != 0){
                steps.push_back(Step(solvable, "solvables"));
                for(auto i : solvable){
                    this->fill_tile(i);
                }
                //sometimes, find_solvable_tiles() can reveal that the board is not solvable
                if(!correct()){
                    return SolveReturnType(false, steps);
                }                
            }
            else{
                auto guess_msg = guess(steps, random, how_many_tiles_leave_empty);
                steps = guess_msg.second;
                if(!guess_msg.first) {
                    return SolveReturnType(false, steps); //if there are no tiles that we can fill in AND guessing failed
                }
            }
        }
        return SolveReturnType(true, steps);
    }

    //solve with educated guesses
    //shoutout to krzysiek for the idea. i would not have came up with that myself
    // if random = false, it picks THE SMALLEST possible solution in the tile with the least possible solutions
    // if random = true,  it picks   A RANDOM   possible solution in the tile with the least possible solutions
    // it's used for generation of random problems
    pair<bool, vector<Step>> guess(vector<Step> steps = vector<Step>(0), bool random = false, short how_many_tiles_leave_empty = 0){
        if(random)
            srand(std::chrono::high_resolution_clock::now().time_since_epoch().count());
        // find the tiles with the least pos_sols possible and pick one
        short best_guesses_length = this->size_2 + 1;
        vector<pair<pair<short, short>, vector<short>>> best_tiles_candidates;

        for(short x = 0; x<this->size_2; x++){
            for(short y = 0; y<this->size_2; y++){
                if(solved[x][y] == EMPTY_TILE){
                    vector<short> solutions;
                    for(short sol = 0; sol<this->size_2; sol++){
                        if(pos_sol[x][y][sol]){
                            solutions.push_back(sol);
                        }
                    }

                    if(solutions.size()<best_guesses_length){
                        best_tiles_candidates = {{{x, y}, solutions}};
                        best_guesses_length = solutions.size();
                    }else if(solutions.size() == best_guesses_length){
                        best_tiles_candidates.push_back({{x, y}, solutions});
                    }
                }
            }
        }

        pair<short, short> picked_tile_coords;
        vector<short> picked_tile_guesses;

        //sometimes more than one solution is possible for a tile, so this randomises which one we pick
        if(random){
            std::mt19937 rng(std::random_device{}());
            
            short tmp = std::rand()%best_tiles_candidates.size();
            picked_tile_coords = best_tiles_candidates[tmp].first;
            picked_tile_guesses = best_tiles_candidates[tmp].second;
            
            std::shuffle(picked_tile_guesses.begin(), picked_tile_guesses.end(), rng);
        }else{
            picked_tile_coords = best_tiles_candidates[0].first;
            picked_tile_guesses = best_tiles_candidates[0].second;
        }

        //we check every pos_sol for that tile
        for(short i = 0; i<picked_tile_guesses.size(); i++){
            FillIn fill_in = FillIn(picked_tile_coords.first, picked_tile_coords.second, picked_tile_guesses[i]);
            Step new_step = Step(vector<FillIn>(1, fill_in), "guess");
            steps.push_back(new_step);

            Sudoku guessed_board = *this;
            guessed_board.fill_tile(fill_in);
                SolveReturnType solve_msg = guessed_board.solve(steps, random, how_many_tiles_leave_empty);
            steps = solve_msg.steps;
            
            if(solve_msg.solvable){//if the board can be solved with this guess
                this->solved = guessed_board.solved;
                return make_pair(true, steps);
            }else{
                steps.push_back(Step(std::vector<FillIn>(0), "cancel guess"));
            }

        }
        return make_pair(false, steps);
    }

    static string steps_json(vector<Step> steps){
        string prefix = ", \"steps\":[";
        string state_json = prefix;
        for(auto step : steps){
            //fill-ins
            state_json += "{\"fillIns\":\"";
                for(auto fill_in : step.fill_ins){
                    state_json += to_string(fill_in.x) + " ";
                    state_json += to_string(fill_in.y) + " ";
                    state_json += to_string(fill_in.solution) + " ";
                }
                state_json += "\"";
                //msg
                state_json += ",\"msg\":\"" + step.comment;  
            state_json += "\"},";
        }
        if(state_json != prefix){
            state_json.pop_back();
        }
        state_json += "]";
        return state_json;
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

    void update_pos_sol(){
        for(short solution = 0; solution < this->size_2; solution++){
            update_pos_sol(solution);
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
                        if(is_here) {
                            return false;//if we have already seen that solution in this square
                        }
                        is_here = true; 
                    }
                }
            }
        }

        //columns
        for(short x = 0; x<this->size_2; x++){
            for(short sol = 0; sol<this->size_2; sol++){
                bool is_here = false;
                for(short y = 0; y<this->size_2; y++){
                    if(solved[x][y] == sol){
                        if(is_here){
                            return false;//if we have already found that solution in this row once
                        }
                        is_here = true;
                    }
                }
            }
        }

        //rows
        for(short y = 0; y<this->size_2; y++){
            for(short sol = 0; sol<this->size_2; sol++){
                bool is_here = false;
                for(short x = 0; x<this->size_2; x++){
                    if(solved[x][y] == sol){
                        if(is_here){
                            return false;//if we have already found that solution in this column once
                        }
                        is_here = true;
                    }
                }
            }
        }

        // if there is a tile which has no pos_sols
        for(short x = 0; x<this->size_2; x++){
            for(short y = 0; y<this->size_2; y++){
                short counter = 0;
                for(short sol = 0; sol<this->size_2; sol++){
                    if(pos_sol[x][y][sol]) counter++;
                }
                if(counter == 0){
                    return false;
                }
            }
        }
        return true;
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

    vector<vector<short>> generate_problem_shit_algorithm(double procent_solved){
        Sudoku board;
        board.solve(vector<Step>(0), true);
        board.update_pos_sol();
        vector<pair<short, short>> tiles_to_clear = random_tiles(size_2 * size_2 * (1.0-procent_solved));
        for(auto i : tiles_to_clear){
            board.solved[i.first][i.second] = -1;
        }
        return board.solved;
    }

    vector<pair<short, short>> random_tiles(short n){
        if(n < 0 || n > size_2 * size_2)
            throw "n must be in range [0, amout_of_tiles]";
        //partial Fisher-Yates algorithm
        vector<pair<short, short>> all_tiles;

        for(short y = 0; y<size_2; y++){
            for(short x = 0; x<size_2; x++){
                all_tiles.push_back({x, y});
            }
        }
        
        std::mt19937 rng(std::random_device{}());
        std::shuffle(all_tiles.begin(), all_tiles.end(), rng);
        vector<pair<short, short>> selected_tiles(all_tiles.begin(), all_tiles.begin() + n);
        return selected_tiles;
    }

    vector<vector<short>> generate_problem_using_hints(double procent_solved){
        Sudoku tmp;
        auto solved_tiles = tmp.hint(true, size_2 * size_2 * procent_solved);
        for(auto i : solved_tiles){
            tmp.fill_tile(i);
        }
        return tmp.solved;
    }
    
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



    //returns a FillIn object representing a correct solution to a tile. returns nullptr if the board is unsolvable
    vector<FillIn> hint(bool random, int how_many_tiles_solve){
        if(random)
            srand(std::chrono::high_resolution_clock::now().time_since_epoch().count());
        
        //if there are no solvable tiles, we have to solve the entire sudoku first, to find a definitely correct solution
        Sudoku tmp = *this;
        vector<FillIn> return_value;
        SolveReturnType solution = tmp.solve(vector<Step>(0), random, size_2 * size_2 - how_many_tiles_solve);
        
        for(auto step : solution.steps){
            for(auto fill_in : step.fill_ins){
                return_value.push_back(fill_in);
                if(return_value.size() >= how_many_tiles_solve){
                    return return_value;
                }
            } 
        }
        return return_value;
    }
};