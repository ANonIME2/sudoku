export const BOARD_SIZE = 3;
export const BOARD_SIZE_2 = BOARD_SIZE * BOARD_SIZE;

export function Sudoku(propos){
    let state = propos.state;
    const setState = propos.setState;
    let stateArray = [];
    
    const [board, setBoard] = React.useState([]);
    
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
                                    value={board[x][y]}
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
export function Main(propos){
    const [state, setState] = React.useState("")
    const [errorMsg, setErrorMsg] = React.useState("");
    const solve = ()=>{
        setErrorMsg("");
        fetch(`/solve/${state}`)
        .then(x=>x.text().then(value=>{
            value = JSON.parse(value)
            
            if(!value["solvable"]){
                setErrorMsg("This sudoku cannot be solved!")
            }

            setState(value["state"].replaceAll('"',''));
        }
        )
        );
    }

    function render(){
        return (
            <>
                {errorMsg == ""? (<></>) : (<p className="sudoku_error_msg">{errorMsg}</p>)}
                
                <div className="sudoku_main">
                    <Sudoku state={state} setState={setState}/>
                    {/* <textarea 
                        className="state_input"
                        autoFocus 
                        type="text"
                        value={state}
                        onInput={(e)=>{
                            setState(e.target.value);
                        }}
                    /> */}
                    {
                        propos.type=="solver"? (<button className="solve_button" onClick={solve}>SOLVE</button>) : <></>
                    }
                </div>
            </>
        )
    }
    return render();
}