#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include <stack>
using namespace std;

class Sudoku{
public:
    size_t size, size_2;
    vector<vector<vector<bool>>> pos_sol;
    vector<vector<int>> solved;
    Sudoku(size_t size = 3): size(size), size_2(size*size) {
        this->solved = vector(size_2, vector(size_2, -1));
        this->pos_sol = vector(this->size_2, vector(this->size_2, vector(this->size_2, true)));  
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

    bool solve(bool random = false){
        while(!this->all_filled_in()){
            if(!this->update_pos_sol()){
                return false;
            }
            this->fill_solved_tiles();
            if(!correct()){
                return false;
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

                if(solved[x][y] != -1){
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
        //we create a copy of pos_sol just to see if it changes at all. if it doesn't, we need to guess();
        bool copy_pos_sol[this->size_2][this->size_2][this->size_2];
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
                if(solved[x][y] != -1){
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
        for(int i =0 ;i<this->size_2; i++){
            for(int j = 0; j<this->size_2; j++){
                for(int k = 0; k<this->size_2; k++){
                    if(copy_pos_sol[i][j][k] != pos_sol[i][j][k]){
                        return true;// IMPORTANT THIS MIGHT FUCK ME LATER SO JUST WATCH OUT
                    }
                }
            }
        }        
        return guess();
    }

    //takes in the coordinates and solution to a tile and sets all possible solutions to that tile to false except the specified one
    //note that the solutions are still indexed from 0 (0 -> this->size_2 - 1, not 1 -> this->size_2)
    void fill_tile(int x, int y, int sol){
        this->solved[x][y] = sol;
        for(int i = 0; i<this->size_2; i++){
            this->pos_sol[x][y][i] = false;
        }
        this->pos_sol[x][y][sol] = true;
    }

    // if there is only 1 'true' value in the vector, return it's index, otherwise return -1
    int position_of_unique_true(vector<bool> v){
        bool accoured = false;
        int position = -1;
        for(int i = 0; i<v.size(); i++){
            if(v[i]){
                if(accoured){
                    return -1;
                }
                accoured = true;
                position = i;
            }
        }
        return position;
    }

    //if a tile has only 1 pos_sol, or a solution has only 1 possible tile in a column/row/square, fill it in
    void fill_solved_tiles(){
        int sol_counter, tile_counter;
        for(int x = 0; x<this->size_2; x++){
            for(int y = 0; y<this->size_2; y++){
                //if there is only 1 possible_solution for that tile

                int pos_of_unique_true = position_of_unique_true(pos_sol[x][y]); //position of the only 'true' value in the vector. if there is not exactly 1 'true' value, this is equal to -1
                if(pos_of_unique_true != -1){
                    fill_tile(x, y, pos_of_unique_true);
                }
            }
        }
        //when that possible_solution is unique in a row / column / square
        //row
        for(int x =0; x<this->size_2; x++){
            for(int sol = 0; sol<this->size_2; sol++){
                
                vector<bool> row(this->size_2);
                for(int i = 0; i<this->size_2; i++){//tile in row
                    row[i] = pos_sol[x][i][sol];    
                }
                
                int tile_y = position_of_unique_true(row); //position of the only 'true' value in the vector. if there is not exactly 1 'true' value, this is equal to -1

                if(tile_y != -1){
                    fill_tile(x, tile_y, sol);
                }
            }
        }
        //column
        for(int y =0; y<this->size_2; y++){
            for(int sol = 0; sol<this->size_2; sol++){
                vector<bool> column(this->size_2);
                for(int i = 0; i<this->size_2; i++){//tile in column
                    column[i] = pos_sol[i][y][sol];    
                }
                
                int tile_x = position_of_unique_true(column); //position of the only 'true' value in the vector. if there is not exactly 1 'true' value, this is equal to -1

                if(tile_x != -1){
                    fill_tile(tile_x, y, sol);
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

                int tile_id = position_of_unique_true(pos_sol_square_tiles); //position of the only 'true' value in the vector. if there is not exactly 1 'true' value, this is equal to -1

                if(tile_id != -1){
                    fill_tile(square_tiles_coordinates[tile_id].first,square_tiles_coordinates[tile_id].second, sol);
                }
            }
        }
    }

    bool all_filled_in(){
        //returns true if the entire sudoku is filled in. if not, returns false
        for(int x = 0; x<this->size_2; x++){
            for(int y = 0; y<this->size_2; y++){
                if(solved[x][y] == -1){
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
    //shoutout krzysiek for the idea. i would not have came up with that myself
    // if random = false, it picks THE SMALLEST possible solution in the tile with the least possible solutions
    // if random = true,  it picks   A RANDOM   possible solution in the tile with the least possible solutions
    // it's used for generation of random problems
    bool guess(bool random = false){
        // we find the tile with the least pos_sols possible
        vector<int> least_sols(this->size_2+1, -1);
        pair<int, int> best_tile_coords;
        for(int x = 0; x<this->size_2; x++){
            for(int y = 0; y<this->size_2; y++){
                if(solved[x][y] == -1){
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

    void print_solution_to_file(string file = "state.txt"){
        string return_value = "";
        return_value += "{\"state\":\"";
        for(int y = 0; y<this->size_2; y++){
            for(int x = 0; x<this->size_2; x++){
                return_value += x + '0';
                return_value += ' ';
                return_value += y + '0';
                return_value += ' ';
                return_value += this->solved[x][y] + '0';
                return_value += ' ';
            }
        }
        return_value += "\",\"solvable\":";
        if(this->correct()){
            return_value += "true";
        }else{
            return_value += "false";
        }
        return_value += "}\n";
        ofstream out(file);
        out<<return_value;
    
    }



    void generate_solved_board(){
        solve(true);
    }
};