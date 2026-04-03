import React from "react";
import settings from "../../../settings";
import Sudoku from "../components/SudokuInput";

export default function Main(){
    const [state, setState] = React.useState("")
    function render(){
        return (
            <>
                <Sudoku state={state} setState={setState}/>
            </>
        )
    }
    return render();
}