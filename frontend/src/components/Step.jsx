import Sudoku from "./SudokuRenderer";
import React from "react";
import "../styles/Step.css"
import { ReactFlow, Handle, Position} from "@xyflow/react";


export default function SudokuStep({data}){
  let state = data.state
  function render(){
    return (
      <div className="sudokuStep" data-id={data.id}>
        <Handle
          type="source"
          position={Position.Right}
          id={`${data.id}-source`}
        />
        <Handle
          type="target"
          position={Position.Left}
          id={`${data.id}-target`}
        />
        <p className="sudokuStepTitle">{data.title}</p>
        <Sudoku state={state} setState={()=>{}} allDisabled={true} highlights={data.fillIns} style={{width:"400px", height:"400px", fontSize:"1.2rem"}}/>
      </div>
    )
  }

  return render();
}