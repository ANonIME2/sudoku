import React from "react";
import settings from "../../../settings";
import "../styles/SudokuRenderer.css"

//this takes 2 mandatory props. state and setState. the intended way of using them is to make yourself a setState in the parent component
// and this component just lets you edit them with a nice UI
export default function Sudoku(props) {
  const BOARD_SIZE = 3;
  const BOARD_SIZE_2 = BOARD_SIZE * BOARD_SIZE;
  const setState = props.setState;
  const disabled = props.disabled == undefined ? false : props.disabled;
  let stateArray = [];
  let state = props.state;
  const highlights = props.highlights ? props.highlights.trim().split(" ") : [];
  if(state[state.length-1] != ' '){
    state += " ";
  }

  const [stateMatrix, setStateMatrix] = React.useState([]);
  //stores all the tiles that have solutions colliding with other tiles (when there is the same solution in the same row or column or square) in the format [{x, y}, {x, y}...]
  const [errorTilesArray, setErrorsArray] = React.useState([])

  function what_square(x, y){
    let square = 0;
    square += Math.floor(x / BOARD_SIZE);
    square += Math.floor((y / BOARD_SIZE))*BOARD_SIZE;
    return square;
  }
  
  function tiles_of_square(square) {
    let tiles = [];
    for (let y = Math.floor(square / BOARD_SIZE) * BOARD_SIZE; y < Math.floor(square / BOARD_SIZE) * BOARD_SIZE + BOARD_SIZE; y++) {
      for (let x = (square % BOARD_SIZE) * BOARD_SIZE; x < (square % BOARD_SIZE) * BOARD_SIZE + BOARD_SIZE; x++) {
        tiles.push({ x: x, y: y });
      }
    }
    return tiles;
  }

  React.useEffect(() => { //arrows controlls     
    document.addEventListener("keydown", (e) => {
      if (!document.activeElement.classList.contains("tile")) return;
      function changeFocus(diff) {
        const focused = document.activeElement;
        const tiles = document.querySelectorAll(".tile");
        let new_focused_id = null;
        tiles.forEach((e, i) => {
          if (e == focused) new_focused_id = i + diff;
          return;
        });
        if (new_focused_id < 0 || new_focused_id >= BOARD_SIZE_2 * BOARD_SIZE_2) return;
        tiles[new_focused_id].focus();
      }

      if (e.key == "ArrowUp") {
        changeFocus(-BOARD_SIZE_2)
      } else if (e.key == "ArrowDown") {
        changeFocus(BOARD_SIZE_2)
      } else if (e.key == "ArrowLeft") {
        changeFocus(-1)
      } else if (e.key == "ArrowRight") {
        changeFocus(1)
      }
    })
  }, [])

  React.useEffect(() => {
    let stateSet = new Set();
    stateArray = state.trim().split(" ");
    stateArray = stateArray.slice(0, stateArray.length - stateArray.length % 3);
    let newStateMatrix = [];
    let newErrorsArray = [];

    for(let i = 0; i<stateArray.length; i+=3){
      stateSet.add([stateArray[i], stateArray[i+1], stateArray[i+2]]);
    }
    stateArray = [];
    
    stateSet.forEach(ele => {
      stateArray = stateArray.concat(ele);
    });

    setState(stateArray.join(" "))

    for (let y = 0; y < BOARD_SIZE_2; y++) {
      let tmp = [];
      for (let x = 0; x < BOARD_SIZE_2; x++) {
        tmp.push("")
      }
      newStateMatrix.push(tmp);
    }

    for (let i = 0; i < stateArray.length; i += 3) {
      newStateMatrix[stateArray[i]][stateArray[i + 1]] = Number(stateArray[i + 2]) + 1;
    }

    for (let y = 0; y < BOARD_SIZE_2; y++) {
      //a 2-dimentional array storing the info on what tiles have a certain solution. solutionAccurances[solution] = an array with objects {x, y} that represent the tiles where the solution is. if there's more than 1 item in that array, there is a collision and the sudoku in incorrect. got it? good. fuck you, future me. good luck understanding this shit in 2 weeks :P
      let solutionAccurances = [];
      for (let i = 0; i < BOARD_SIZE_2; i++) {
        solutionAccurances.push([]);
      }

      for (let x = 0; x < BOARD_SIZE_2; x++) {
        if (newStateMatrix[x][y] != '') {//if the tile is filled in
          solutionAccurances[newStateMatrix[x][y] - 1].push({ x: x, y: y });
        }
      }

      for (let solution = 0; solution < BOARD_SIZE_2; solution++) {
        if (solutionAccurances[solution].length > 1) {//is there a collision?
          newErrorsArray = newErrorsArray.concat(solutionAccurances[solution])
        }
      }
    }

    for (let x = 0; x < BOARD_SIZE_2; x++) {
      //a 2-dimentional array storing the info on what tiles have a certain solution. solutionAccurances[solution] = an array with objects {x, y} that represent the tiles where the solution is. if there's more than 1 item in that array, there is a collision and the sudoku in incorrect. got it? good. fuck you, future me. good luck understanding this shit in 2 weeks :P
      let solutionAccurances = [];
      for (let i = 0; i < BOARD_SIZE_2; i++) {
        solutionAccurances.push([]);
      }

      for (let y = 0; y < BOARD_SIZE_2; y++) {
        if (newStateMatrix[x][y] != '') {//if the tile is filled in
          solutionAccurances[newStateMatrix[x][y] - 1].push({ x: x, y: y });
        }
      }

      for (let solution = 0; solution < BOARD_SIZE_2; solution++) {
        if (solutionAccurances[solution].length > 1) {//is there a collision?
          newErrorsArray = newErrorsArray.concat(solutionAccurances[solution])
        }
      }
    }


    for (let square = 0; square < BOARD_SIZE_2; square++) {
      let tiles = tiles_of_square(square);
      //a 2-dimentional array storing the info on what tiles have a certain solution. solutionAccurances[solution] = an array with objects {x, y} that represent the tiles where the solution is. if there's more than 1 item in that array, there is a collision and the sudoku in incorrect. got it? good. fuck you, future me. good luck understanding this shit in 2 weeks :P
      let solutionAccurances = [];
      for (let i = 0; i < BOARD_SIZE_2; i++) {
        solutionAccurances.push([]);
      }

      tiles.forEach(ele => {
        if (newStateMatrix[ele.x][ele.y] != '') {//if the tile is filled in
          solutionAccurances[newStateMatrix[ele.x][ele.y] - 1].push({ x: ele.x, y: ele.y });
        }
      });



      for (let solution = 0; solution < BOARD_SIZE_2; solution++) {
        if (solutionAccurances[solution].length > 1) {//is there a collision?
          newErrorsArray = newErrorsArray.concat(solutionAccurances[solution])
        }
      }
    }

    //all possible solutions false
    let posSolutions = [];
    for(let x = 0; x<BOARD_SIZE_2; x++){
      let newColumn = [];
      for(let y = 0; y<BOARD_SIZE_2; y++){
        let newTile = [];
        for(let solution = 0; solution< BOARD_SIZE_2; solution++){
          newTile.push(true);
        }
        newColumn.push(newTile)
      }
      posSolutions.push(newColumn)
    }

    for(let x = 0; x<BOARD_SIZE_2; x++){
      for(let y = 0; y<BOARD_SIZE_2; y++){
        const solution = newStateMatrix[x][y] - 1;
        if(solution == undefined || solution == -1) continue
        //row and column
        for(let i = 0; i<BOARD_SIZE_2; i++){
          posSolutions[x][i][solution] = false;
          posSolutions[i][y][solution] = false;
        }
        //square
        const squareTiles = tiles_of_square(what_square(x, y));
        squareTiles.forEach(ele => {
          posSolutions[ele.x][ele.y][solution] = false;
        });
      }
    }
    
    for(let x = 0; x<BOARD_SIZE_2; x++){
      for(let y = 0; y<BOARD_SIZE_2; y++){
        if(newStateMatrix[x][y]){continue}
        let isCorrect = false;
        for(let sol = 0; sol<BOARD_SIZE_2; sol++){
          isCorrect = isCorrect || posSolutions[x][y][sol]; 
        }

        if(!isCorrect) newErrorsArray.push({x: x, y:y});
      }
    }
    console.log(newStateMatrix);
    
    setStateMatrix(newStateMatrix)
    setErrorsArray(newErrorsArray)
  }, [state])

  function updateState(x, y, event) {
    const newSolution = event.target.value;
    
    if (isNaN(Number(newSolution)) || '0' == newSolution) {
      event.target.value = '';
      return;
    }
    
    let newStateArray = state.trim().split(" ");
    let filledIn = false;
    for(let i = 0; i<newStateArray.length; i+= 3){
      const xi = newStateArray[i], yi=newStateArray[i+1];
      console.log(newSolution, xi, yi);
      if(xi == x && yi == y){
        filledIn = true;
        if(newSolution == ""){
          newStateArray = newStateArray.slice(0, i).concat(newStateArray.slice(i+3, newStateArray.length));
        }else{
          newStateArray[i+2] = newSolution;
        }
      }
    }
    console.log(filledIn);
    
    if(!filledIn){
      newStateArray = newStateArray.concat([x, y, newSolution]);
    }
    console.log(newStateArray);
    

    const newState = newStateArray.join(" ");
    console.log(newStateArray);
    
    setState(newState)
  }

  //returns a string representing the class names needed for a specified tile. 
  function wideBorders(x, y) {
    let result = '';
    if (y % BOARD_SIZE == BOARD_SIZE - 1) {
      result += " medium_bottom_border"
    }
    else if (y % BOARD_SIZE == 0) {
      result += " medium_top_border"
    }
    if (x % BOARD_SIZE == BOARD_SIZE - 1) {
      result += " medium_right_border"
    }
    else if (x % BOARD_SIZE == 0) {
      result += " medium_left_border"
    }

    if (x == 0) {
      result += " wide_left_border";
    }
    else if (x == BOARD_SIZE_2 - 1) {
      result += " wide_right_border";
    }
    if (y == 0) {
      result += " wide_top_border";
    }
    else if (y == BOARD_SIZE_2 - 1) {
      result += " wide_bottom_border";
    }

    return result;
  }

  function render() {
    let highlightsMatrix = [];

    for(let i = 0; i<BOARD_SIZE_2; i++){
      let newColumn = []
      for(let j = 0; j<BOARD_SIZE_2; j++){
        newColumn.push(false);
      }
      highlightsMatrix.push(newColumn);
    }

    for(let i = 0; i<highlights.length; i+=3){
      const x = highlights[i], y = highlights[i+1];
      highlightsMatrix[x][y]= true;
    }

    const tiles = (
      stateMatrix.map((column, y) => {
        return column.map((row, x) => {
          let className = `tile${wideBorders(x, y)}`;
          if(highlightsMatrix[x][y]){
            className += " highlighted_tile"
          }
          if (errorTilesArray.some((ele) => ele.x == x && ele.y == y)) {
            className += " error_tile";
          }

          return (           
            <input
              onChange={(e) => { updateState(x, y, e) }}
              maxLength={1}
              value={isNaN(stateMatrix[x][y]) ? "" : stateMatrix[x][y]}
              className={className}
              x={x}
              y={y}
              key={String(x) + String(y)}
              disabled={disabled}
            />
          )
        })
      })
    )

    return (

      <div className="sudoku_board" style={props.style}>
        {
          tiles
        }
      </div>

    )
  }

  return render();
}