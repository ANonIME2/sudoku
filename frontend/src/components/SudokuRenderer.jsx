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

  const [stateMatrix, setStateMatrix] = React.useState([]);
  //stores all the tiles that have solutions colliding with other tiles (when there is the same solution in the same row or column or square) in the format [{x, y}, {x, y}...]
  const [errorTilesArray, setErrorsArray] = React.useState([])


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
    stateArray = props.state.trim().split(" ");
    stateArray = stateArray.slice(0, stateArray.length - stateArray.length % 3);

    let newStateMatrix = [];
    let newErrorsArray = []
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


    setStateMatrix(newStateMatrix)
    setErrorsArray(newErrorsArray)

  }, [props.state])

  function updateState(x, y, event) {
    if (isNaN(Number(event.target.value)) || '0' == event.target.value) {
      event.target.value = '';
      return;
    }


    let newState = '';
    const tiles = document.querySelectorAll(".tile");

    for (let i = 0; i < tiles.length; i++) {
      if (tiles[i].value != "") {
        newState += `${tiles[i].getAttribute('x')} ${tiles[i].getAttribute('y')} ${tiles[i].value - 1} `;
      }
    }

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
    const tiles = (
      stateMatrix.map((column, y) => {
        return column.map((row, x) => {
          let className = `tile${wideBorders(x, y)}`;
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

      <div className="sudoku_board">
        {
          tiles
        }
      </div>

    )
  }

  return render();
}