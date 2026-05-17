import "../styles/Sandbox.css"
import StepsRenderer from "../components/StepsRenderer.jsx";
import settings from "../../../settings.js"
import React from "react";
import Sudoku from "../components/SudokuRenderer";

export default function Main(propos){
    const [state, setState] = React.useState("")
    const [errorMsg, setErrorMsg] = React.useState("");
    const [solveResponse, setSolveResponse] = React.useState({});

    const solve = ()=>{
        setErrorMsg("");
        fetch(`http://localhost:${settings.apiPort}/solve`,{
            method:"POST",
            headers:{"Content-Type":"application/json"},
            body:JSON.stringify({
                state:state
            })
        })
        .then(x=>x.text().then(value=>{
            value = JSON.parse(value)
            setSolveResponse(value);

            if(!value["solvable"]){
                setErrorMsg("This sudoku cannot be solved!")
            }

            setState(value["state"].replaceAll('"',''));
        }));
    }

    const hint = ()=>{
        fetch(`http://localhost:${settings.apiPort}/hint`,{
            method:"POST",
            headers:{"Content-Type":"application/json"},
            body:JSON.stringify({
                state:state
            })
        })
        .then(x=>x.text().then(value=>{
            value = JSON.parse(value)
            
            if(!value["solvable"]){
                setErrorMsg("This sudoku cannot be solved!")
            }

            setState(value["state"].replaceAll('"',''));
        }));
    }

    function render(){        
        return (
            <>
                <p className="sudoku_error_msg" style={{opacity: errorMsg == ""? "0%": "100%"}}>{errorMsg}</p>
                <div className="sudoku_main">
                    <Sudoku state={state} setState={setState}/>
                    <textarea 
                        className="state_input"
                        autoFocus 
                        type="text"
                        value={state}
                        onInput={(e)=>{
                            setState(e.target.value);
                        }}
                        style={{"position":"absolute", "left":"10px"}}
                    />

                    <div style={{display:"flex", flexDirection:"column", gap:"10px"}}>
                        <button className="btn-big" onClick={solve}>SOLVE</button>
                        <button className="btn-big" onClick={hint}>HINT</button>
                    </div>

                    {/*<div className="steps-renderer-container">
                        <StepsRenderer preSolvedState={solveResponse.preSolvedState} steps={solveResponse.steps}/>
                    </div>*/}
                </div>
            </>
        )
    }
    return render();
}
