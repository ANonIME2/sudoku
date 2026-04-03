import React from "react";
import settings from "../../../settings";
import "../sudokuInput.css"

//this takes 2 mandatory props. state and setState. the intended way of using them is to make yourself a setState in the parent component
// and this component just lets you edit them with a nice UI
export default function Sudoku(props){
    const BOARD_SIZE = 3;
    const BOARD_SIZE_2 = BOARD_SIZE * BOARD_SIZE;
    let state = props.state;
    const setState = props.setState;
   
    console.log(state);
    console.log(setState);
    
    
    let stateArray = [];
    
    const [board, setBoard] = React.useState([]);

    React.useEffect(()=>{
        document.addEventListener("keydown", (e)=>{
            if(!document.activeElement.classList.contains("tile")) return;
            function changeFocus(diff){
                const focused = document.activeElement;
                const tiles = document.querySelectorAll(".tile");
                let new_focused_id = null;
                tiles.forEach((e, i) => {
                    if(e == focused) new_focused_id = i + diff;
                    return;
                });
                if(new_focused_id < 0 || new_focused_id >= BOARD_SIZE_2 * BOARD_SIZE_2) return;
                tiles[new_focused_id].focus();
            }

            if(e.key == "ArrowUp"){
                changeFocus(-BOARD_SIZE_2)
            }else if(e.key == "ArrowDown"){
                changeFocus(BOARD_SIZE_2)
            }else if(e.key == "ArrowLeft"){
                changeFocus(-1)
            }else if(e.key == "ArrowRight"){
                changeFocus(1)
            }
        })
    }, [])
    
    React.useEffect(()=>{
        stateArray = state.split(" ");
        if(stateArray[stateArray.length-1] == ""){
            stateArray = stateArray.slice(0, -1)
        }
        stateArray = stateArray.slice(0, stateArray.length - stateArray.length%3);
        
    

        let newBoard = [];
        for(let y = 0; y<BOARD_SIZE_2; y++){
            let tmp = [];
            for(let x = 0; x<BOARD_SIZE_2; x++){
                tmp.push("")
            }
            newBoard.push(tmp);
        }
        for(let i = 0; i<stateArray.length; i+=3){
            newBoard[stateArray[i]][stateArray[i+1]] = Number(stateArray[i+2])+1;
        }
        setBoard(newBoard)
    }, [state])
 
    function updateState(x, y, event){        
        if(isNaN(Number(event.target.value))){
            event.target.value = '';
            return;
        }


        let newState ='';
        const tiles = document.querySelectorAll(".tile");
        
        for(let i = 0; i<tiles.length; i++){
            if(tiles[i].value != ""){
                newState += `${tiles[i].getAttribute('x')} ${tiles[i].getAttribute('y')} ${tiles[i].value-1} `;
            }
        }
        
        console.log("setState was used");
        
        setState(newState)

    }

    //returns a string representing the class names needed for a specified tile. 
    function wideBorders(x, y){
        let result = '';
        if(y%BOARD_SIZE == BOARD_SIZE-1){
            result += " medium_bottom_border"
        }
        else if(y%BOARD_SIZE == 0){
            result += " medium_top_border"
        }
        if (x%BOARD_SIZE == BOARD_SIZE-1){
            result += " medium_right_border"
        }
        else if(x%BOARD_SIZE == 0){
            result += " medium_left_border"
        }

        if(x == 0){
            result += " wide_left_border";
        }
        else if(x == BOARD_SIZE_2-1){
            result += " wide_right_border";
        }
        if(y == 0){
            result += " wide_top_border";
        }
        else if(y == BOARD_SIZE_2 - 1){
            result += " wide_bottom_border";
        }

        return result;
    }

    function render(){
        return (
            <div className="sudoku_board">
                {
                    board.map((column, y)=>{
                        return column.map((row, x)=>{
                            return (
                                <input
                                    onChange={(e)=>{updateState(x, y, e)}}
                                    maxLength={1}
                                    value={isNaN(board[x][y]) ? "" : board[x][y]}
                                    className={`tile${wideBorders(x, y)}`}
                                    x={x}
                                    y={y}
                                    key={String(x)+String(y)}
                                />
                            )
                        })
                    })
                }
            </div>
        )
    }

    return render();
}