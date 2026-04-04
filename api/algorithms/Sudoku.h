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
    static constexpr int EMPTY_TILE = -1, NULL_POSITION = -1;    

    class FillIn{
    public:
        int x, y, solution;
        FillIn(int x, int y, int solution): x(x), y(y), solution(solution){}
    };

    size_t size, size_2;
    vector<vector<vector<bool>>> pos_sol;
    vector<vector<int>> solved;


    Sudoku(size_t size = 3): size(size), size_2(size*size) {
        this->solved = vector(size_2, vector(size_2, EMPTY_TILE));
        this->pos_sol = vector(this->size_2, vector(this->size_2, vector(this->size_2, true)));  
    }

    Sudoku operator=(const Sudoku b){
        this->size = b.size;
        this->size_2 = b.size_2;
        for(int y = 0; y<b.size_2; y++){
            for(int x = 0; x<b.size_2; x++){
                this->solved[x][y] = b.solved[x][y];
                for(int sol = 0; sol<b.size_2; sol++){
                    this->pos_sol[x][y][sol] = b.pos_sol[x][y][sol];
                }
            }
        }
        return *this;
    }

    //takes in coordinates of a tile and returns what square it's in
    int what_square(int x, int y){
        int square = 0;
        square += x / this->size;
        square += (y / this->size)*this->size;
        return square;
    }

    //takes in the id of a square and returns a vector of coordinates of the tiles in it
    vector<pair<int, int>> tiles_of_square(int square){
        vector<pair<int, int>> tiles;
        
        for(int y = (square/this->size) * this->size; y<(square/this->size) * this->size+this->size; y++){
            for(int x = (square%this->size) * this->size; x<(square%this->size) * this->size+this->size; x++){
                tiles.push_back({x, y});
            }
        }
        return tiles;
    }

    //returns true if the board is solvable and false otherwise
    bool solve(bool random = false, bool save_steps = true){
        while(!this->all_filled_in()){
            // this can be true if guess() was incorrect or if the original problem was impossible 
            if(!correct()){
                return false;
            }
            std::cout<<this->update_pos_sol()<<"\n";
            vector<FillIn*> solvable = this->find_solvable_tiles();
            if(solvable.size() != 0){
                for(auto i : solvable){
                    this->fill_tile(i);
                }
            }
            else{//if there are no tiles that we can fill in AND guessing failed
                if(!guess()) {
                    return false;
                }
            }
        }
        return true;
    }

    void output_solved(){
        cout<<"||=========================================================================||"<<endl;
        for(int x = 0; x<this->size_2; x++){
            cout<<"|";
            for(int y = 0; y<this->size_2; y++){
                cout<<"|       ";
                if(y%this->size == 2){
                    cout<<"|";}
            }
            cout<<"|";
            cout<<endl;
            cout<<"|";
            for(int y = 0; y<this->size_2; y++){
                cout<<"|   ";

                if(solved[x][y] != EMPTY_TILE){
                    cout<<solved[x][y]+1;
                }
                else{
                    cout<<" ";
                }
                
                cout<<"   ";
                if(y%this->size == 2){  
                    cout<<"|";}        
            }
            cout<<"|";
            cout<<endl;
            cout<<"|";
            for(int y = 0; y<this->size_2; y++){
                cout<<"|       ";
                if(y%this->size == 2){
                    cout<<"|";
                }
            }
            cout<<"|";
            cout<<endl;
            if(x%this->size == 2){
                cout<<"||=========================================================================||"<<endl;
            }
            else{
                cout<<"||-------------------------------------------------------------------------||"<<endl;
            }
        }
        cout<<endl;
    }

    void output_pos_sol(){
        ofstream out("pos_sol.tyt");
        out<<"---------------------------------------------------------------------------"<<endl;
        out<<"---------------------------------------------------------------------------"<<endl;
        for(int x = 0; x<this->size_2; x++){
            for(int y = 0; y<this->size_2; y++){
                out<<"| ";
                for(int z = 0; z<this->size; z++)
                    if(pos_sol[x][y][z]){
                        out<<z+1<<" ";
                    }else{
                        out<<"  ";
                    }
                if(y%this->size == 2)
                    out<<"|";
            }
            out<<endl;
            for(int y = 0; y<this->size_2; y++){
                out<<"| ";
                for(int z = this->size; z<this->size * 2; z++)
                    if(pos_sol[x][y][z]){
                        out<<z+1<<" ";
                    }else{
                        out<<"  ";
                    }
                if(y%this->size == 2)
                    out<<"|";        
            }
            out<<endl;
            for(int y = 0; y<this->size_2; y++){
                out<<"| ";
                for(int z = this->size * 2; z<this->size_2; z++)
                    if(pos_sol[x][y][z]){
                        out<<z+1<<" ";
                    }else{
                        out<<"  ";
                    }
                if(y%this->size == 2)
                    out<<"|";
            }
            out<<endl;
            out<<"---------------------------------------------------------------------------"<<endl;
            if(x%this->size == 2)
                out<<"---------------------------------------------------------------------------"<<endl;
        }
    }

    //return value = false, means that the board can not be solved
    bool update_pos_sol(){
        //we create a copy of pos_sol just to see if it changes at all.
        auto copy_pos_sol = this->pos_sol;
        for(int i =0 ;i<this->size_2; i++){
            for(int j = 0; j<this->size_2; j++){
                for(int k = 0; k<this->size_2; k++){
                    copy_pos_sol[i][j][k] = pos_sol[i][j][k];
                }
            }
        }
        //we walk through all the tiles and if we find a solved one we delete that possible solution from it's row, column and square
        for(int x = 0; x<this->size_2; x++){
            for(int y = 0; y<this->size_2; y++){
                if(solved[x][y] != EMPTY_TILE){
                    //row and column
                    for(int i = 0; i<this->size_2; i++){
                        pos_sol[x][i][solved[x][y]] = false;
                        pos_sol[i][y][solved[x][y]] = false;
                    }

                    //square
                    vector<pair<int, int>> square_tiles = tiles_of_square(what_square(x, y));
                    for(auto i: square_tiles){
                        pos_sol[i.first][i.second][solved[x][y]] = false;
                    }

                    pos_sol[x][y][solved[x][y]] = true;
                }
            }
        }
        
        //check if anything has changed
        for(int x =0 ;x<this->size_2; x++){
            for(int y = 0; y<this->size_2; y++){
                for(int sol = 0; sol<this->size_2; sol++){
                    if(copy_pos_sol[x][y][sol] != pos_sol[x][y][sol]){
                        return true;// IMPORTANT THIS MIGHT FUCK ME LATER SO JUST WATCH OUT
                    }
                }
            }
        }        
        return false;
    }

    //takes in the coordinates and solution to a tile and sets all possible solutions to that tile to false except the specified one
    //note that the solutions are still indexed from 0 (0 -> this->size_2 - 1, not 1 -> this->size_2)
    void fill_tile(int x, int y, int sol){
        this->solved[x][y] = sol;
        for(int i = 0; i<this->size_2; i++){
            this->pos_sol[x][y][i] = false;
        }
        
        // update pos_sol of all the tiles in the row, column and square of this tile
        //row and column
        for(int i = 0; i<this->size_2; i++){
            pos_sol[x][i][sol] = false;
            pos_sol[i][y][sol] = false;
        }

        // square
        vector<pair<int, int>> square_tiles = tiles_of_square(what_square(x, y));
        for(auto i: square_tiles){
            pos_sol[i.first][i.second][sol] = false;
        }

        this->pos_sol[x][y][sol] = true;
    }

    void fill_tile(FillIn *fill_in){
        fill_tile(fill_in->x, fill_in->y, fill_in->solution);
    }

    // if there is only 1 'true' value in the vector, return it's index, otherwise return NULL_POSITION
    int position_of_unique_true(vector<bool> v){
        bool accoured = false;
        int position = NULL_POSITION;
        for(int i = 0; i<v.size(); i++){
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
    
    // if a tile has only 1 pos_sol, or a solution has only 1 possible tile in a column/row/square, fill it in
    vector<FillIn*> find_solvable_tiles(){
        int sol_counter, tile_counter;
        vector<FillIn*> steps = vector<FillIn*>(0);
        for(int x = 0; x<this->size_2; x++){
            for(int y = 0; y<this->size_2; y++){
                if(solved[x][y] != EMPTY_TILE) continue;
                //if there is only 1 possible_solution for that tile
                int pos_of_unique_true = position_of_unique_true(pos_sol[x][y]); //position of the only 'true' value in the vector. if there is not exactly 1 'true' value, this is equal to NULL_POSITION
                if(pos_of_unique_true != NULL_POSITION){
                    steps.push_back(new FillIn(x, y, pos_of_unique_true));
                }
            }
        }
        //when that possible_solution is unique in a row / column / square
        // column
        for(int x =0; x<this->size_2; x++){
            for(int sol = 0; sol<this->size_2; sol++){
                vector<bool> column(this->size_2);
                for(int i = 0; i<this->size_2; i++){//tile in column
                    column[i] = pos_sol[x][i][sol];    
                }
                
                int tile_y = position_of_unique_true(column); //position of the only 'true' value in the vector. if there is not exactly 1 'true' value, this is equal to NULL_POSITION
                
                if(tile_y != NULL_POSITION && solved[x][tile_y] == EMPTY_TILE){
                    steps.push_back(new FillIn(x, tile_y, sol));
                }
            }
        }

        //row
        for(int y =0; y<this->size_2; y++){
            for(int sol = 0; sol<this->size_2; sol++){
                vector<bool> row(this->size_2);
                for(int i = 0; i<this->size_2; i++){//tile in row
                    row[i] = pos_sol[i][y][sol];    
                }
                
                int tile_x = position_of_unique_true(row); //position of the only 'true' value in the vector. if there is not exactly 1 'true' value, this is equal to NULL_POSITION

                if(tile_x != NULL_POSITION && solved[tile_x][y] == EMPTY_TILE){
                    steps.push_back(new FillIn(tile_x, y, sol));
                }
            }
        }

        //square
        for(int square_id = 0; square_id<this->size_2; square_id++){
            for(int sol = 0; sol<this->size_2; sol++){
                vector<pair<int, int>> square_tiles_coordinates = tiles_of_square(square_id);
                vector<bool> pos_sol_square_tiles(this->size_2);
                for(int i = 0; i<this->size_2; i++){
                    pos_sol_square_tiles[i] = pos_sol[square_tiles_coordinates[i].first][square_tiles_coordinates[i].second][sol];
                }

                int tile_id = position_of_unique_true(pos_sol_square_tiles); //position of the only 'true' value in the vector. if there is not exactly 1 'true' value, this is equal to NULL_POSITION

                if(tile_id != NULL_POSITION && solved[square_tiles_coordinates[tile_id].first][square_tiles_coordinates[tile_id].second] == EMPTY_TILE){
                    steps.push_back(new FillIn(square_tiles_coordinates[tile_id].first, square_tiles_coordinates[tile_id].second, sol));
                }
            }
        }

        return steps;
    }

    bool all_filled_in(){
        //returns true if the entire sudoku is filled in. if not, returns false
        for(int x = 0; x<this->size_2; x++){
            for(int y = 0; y<this->size_2; y++){
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
        for(int square = 0; square<this->size_2; square++){
            vector<pair<int, int>> square_tiles = tiles_of_square(square);
            for(int sol = 0; sol<this->size_2; sol++){
                bool is_here = false;
                for(int tile = 0; tile<this->size_2; tile++){
                    if(solved[square_tiles[tile].first][square_tiles[tile].second] == sol){
                        if(is_here) return false;//if we have alreayx saw that solution in this square
                        is_here = true; 
                    }
                }
            }
        }

        //rows
        for(int x = 0; x<this->size_2; x++){
            for(int sol = 0; sol<this->size_2; sol++){
                bool is_here = false;
                for(int y = 0; y<this->size_2; y++){
                    if(solved[x][y] == sol){
                        if(is_here) return false;//if we have already found that solution in this row once
                        is_here = true;
                    }
                }
            }
        }

        //cols
        for(int y = 0; y<this->size_2; y++){
            for(int sol = 0; sol<this->size_2; sol++){
                bool is_here = false;
                for(int x = 0; x<this->size_2; x++){
                    if(solved[x][y] == sol){
                        if(is_here) return false;//if we have already found that solution in this column once
                        is_here = true;
                    }
                }
            }
        }

        for(int x = 0; x<this->size_2; x++){
            for(int y = 0; y<this->size_2; y++){
                int counter = 0;
                for(int sol = 0; sol<this->size_2; sol++){
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
        vector<int> least_sols(this->size_2+1, EMPTY_TILE); // this->size_2 + 1 is here so every tile's pos_sol will be shorter than this
        pair<int, int> best_tile_coords;
        for(int x = 0; x<this->size_2; x++){
            for(int y = 0; y<this->size_2; y++){
                if(solved[x][y] == EMPTY_TILE){
                    vector<int> sols;
                    for(int sol = 0; sol<this->size_2; sol++){
                        if(pos_sol[x][y][sol]){
                            sols.push_back(sol);
                        }
                    }
                    if(sols.size()<least_sols.size()){
                        least_sols = sols;
                        best_tile_coords = {x, y};
                    }
                }
            }
        }
        
        if(random){
            sort(least_sols.begin(), least_sols.end());
        }

        //we check every pos_sol for that tile
        int copy_solved[this->size_2][this->size_2];
        bool copy_pos_sol[this->size_2][this->size_2][this->size_2];
        while(!least_sols.empty()){
            for(int i = least_sols.size()-1; i>=0; i--){
                //make copies
                for(int j =0 ;j<this->size_2; j++){
                    for(int k = 0; k<this->size_2; k++){
                        copy_solved[j][k] = solved[j][k];                       
                    }
                }
                for(int l =0 ;l<this->size_2; l++){
                    for(int j = 0; j<this->size_2; j++){
                        for(int k = 0; k<this->size_2; k++){
                            copy_pos_sol[l][j][k] = pos_sol[l][j][k];
                        }
                    }
                }
                
                //guess
                solved[best_tile_coords.first][best_tile_coords.second] = least_sols[i];
                if(solve()){//if the board can be solved 
                    return true;
                }else{
                    //go back to before we made any guesses
                    for(int j =0 ;j<this->size_2; j++){
                        for(int k = 0; k<this->size_2; k++){
                            solved[j][k] = copy_solved[j][k];                       
                        }
                    }
                    for(int l =0 ;l<this->size_2; l++){
                        for(int j = 0; j<this->size_2; j++){
                            for(int k = 0; k<this->size_2; k++){
                                pos_sol[l][j][k] = copy_pos_sol[l][j][k];
                            }
                        }
                    }
                    least_sols.pop_back();
                }
            }
        }
        return false;
    }

    //returns a string representing a json object representing the state of the board in the format:
    /*
    {
        "state":"x y s x y s",
        "solvable":[bool]
    }
    where s is the solution to the tile (x, y). s = '/' is an empty tile.
    */
   
    string to_json(){
        string state_json = "";
        state_json += "{\"state\":\"";
        for(int y = 0; y<this->size_2; y++){
            for(int x = 0; x<this->size_2; x++){
                if(this->solved[x][y] == EMPTY_TILE) continue;
                state_json += x + '0';
                state_json += ' ';
                state_json += y + '0';
                state_json += ' ';
                state_json += this->solved[x][y] + '0';
                state_json += ' ';
            }
        }
        state_json += "\",\"solvable\":";
        Sudoku tmp = *this;
        if(tmp.solve()){
            state_json += "true";
        }else{
            state_json += "false";
        }
        state_json += "}\n";
        return state_json;
    }

    void generate_solved_board(){
        solve(true);
    }
    
    //returns a FillIn object representing a correct solution to a (semi) random tile. returns nullptr if the board is unsolvable
    FillIn hint(){
        std::cout<<this->update_pos_sol()<<"\n";
        vector<FillIn*> solvable_tiles = this->find_solvable_tiles();
        if(solvable_tiles.size() != 0){
            srand(std::chrono::high_resolution_clock::now().time_since_epoch().count());
            int choice = rand() % solvable_tiles.size();
            return *solvable_tiles[choice];
        }      
        
        Sudoku tmp_sudoku = *this;
        tmp_sudoku.solve();

        for(int y = 0; y<this->size_2; y++){
            for(int x = 0; x<this->size_2; x++){
                if(tmp_sudoku.solved[x][y] != this->solved[x][y]){
                    return FillIn(x, y, tmp_sudoku.solved[x][y]);
                }
            }
        }
        return FillIn(NULL_POSITION, NULL_POSITION, EMPTY_TILE);
    }  
};