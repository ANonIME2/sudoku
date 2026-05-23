import "../styles/Sandbox.css"
import StepsRenderer from "../components/StepsRenderer.jsx";
import settings from "../../../settings.js"
import React from "react";
import Sudoku from "../components/SudokuRenderer";

export default function Main(propos){
    const [state, setState] = React.useState("")
    const [errorMsg, setErrorMsg] = React.useState("");
    const [solveResponse, setSolveResponse] = React.useState({});
    const [pickedMode, setPickedMode] = React.useState(undefined);
    const difficoultyLevels = [
        {name: "blank", colour:"gray", filledIn:0}, 
        {name:"easy", colour:"green", filledIn:50}, 
        {name:"medium", colour:"yellow", filledIn:40}, 
        {name:"hard", colour:"red", filledIn:30}]

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
            setState(value);
        }));
    }

    const generateProblem = (difficoulty) =>{

        fetch(`http://localhost:${settings.apiPort}/generate`,{
            method:"POST",
            headers:{"Content-Type":"application/json"},
            body:JSON.stringify({
                difficoulty:difficoultyLevels[difficoulty].filledIn
            })
        })
        .then(x=>x.text().then(value=>{
            value = JSON.parse(value)
            setState(value);
        }));
    }

    function render(){
        return (
            <>
                {pickedMode == undefined ? 
                    <div className="difficoulty-pick-container">
                        <h2>Pick a difficoulty</h2>
                        <div className="mode-btn-container">
                            {
                                difficoultyLevels.map((ele, i) => {
                                    return (
                                        <button
                                            style={{backgroundColor:ele.colour}}
                                            className="mode-btn"
                                            onClick={()=>{
                                                setPickedMode(i);
                                                generateProblem(i);
                                            }}
                                        >
                                            {ele.name}
                                        </button>
                                    )
                                })
                            }
                        </div>
                    </div> : 
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

                        <StepsRendererContainer solveResponse={solveResponse}/>
                    </div>
                    </>
                } 
            </>
        )
    }
    return render();
}

function StepsRendererContainer(props){    
    const solveResponse = props.solveResponse
    let steps = props.solveResponse.steps ? props.solveResponse.steps : [];
    const [fullscreen, setFullscreen] = React.useState(false);

    
    const toggleStepsFullscreen = ()=>{
        setFullscreen(!fullscreen)
    }

    function render(){
        return (
            <div className={`steps-renderer-container ${fullscreen ? "fullscreen" : ""}`}>
                <button onClick={toggleStepsFullscreen}>{fullscreen ? "minimize" : "fullscreen"}</button>
                <StepsRenderer preSolvedState={solveResponse.preSolvedState} steps={steps}/>
            </div>
        );
    }
    return render();
}