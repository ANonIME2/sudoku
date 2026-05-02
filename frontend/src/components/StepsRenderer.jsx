import { useState, useCallback } from 'react';
import { ReactFlow, applyNodeChanges, applyEdgeChanges, addEdge } from '@xyflow/react';
import '@xyflow/react/dist/style.css';
import "../styles/stepsDisplay.css"
import testSteps from './testSteps';
import Stack from './Stack';
import testPreSolvedState from './testPreSolvedState';
import SudokuStep from './Step';
 
// const initialNodes = [
//   { id: 'n1', position: { x: 0, y: 0 }, data: { label: 'Node 1' } },
//   { id: 'n2', position: { x: 0, y: 100 }, data: { label: 'Node 2' } },
// ];

// const initialEdges = [{ id: 'n1-n2', source: 'n1', target: 'n2' }];
const initialEdges = [];

class Step{
  constructor(step, state, parent, children){
    this.step = step;
    this.state = state;
    this.parent = parent
    this.children = children
  }
}

export default function StepsDisplay(props) {
  // const steps = props.steps;
  // const preSolvedState = props.preSolvedState;
  const preSolvedState = testPreSolvedState;
  const steps = testSteps;
  let graph = [];

  //identificators of the parents of "guess" nodes
  let goBacks = new Stack();

  for(let i = 0; i<steps.length; i++){
    let parent = i - 1 < 0 ? 0 : i - 1;
    
    if(steps[i].msg == "guess"){
      goBacks.push(i-1);
    }else if(steps[i].msg == "cancel guess"){
      parent = goBacks.pop();
    }
    
    let state = i - 1 < 0 ? preSolvedState : graph[parent].state + steps[i].fillIns;

    let newNode = new Step(steps[i], state, parent, []); 
    graph.push(newNode);
    graph[parent].children.push(i);
  }

  let initialNodes = [];
  let guessDepth = 0;
  graph.forEach((ele, i) => {
    if(ele.step.msg == "guess"){
      guessDepth += 1;
    }else if(ele.step.msg == "cancel guess"){
      guessDepth -= 1; 
    }

    initialNodes.push({
      id: String(i),
      type:'sudokuStep',
      position: {x: 700 * i, y: 700 * guessDepth},
      data: {
        title: ele.step.msg + ele.step.fillIns,
        state: ele.state
      }
    })

    ele.children.forEach(child => {
      initialEdges.push({
        id: String(i)+"-"+String(child), 
        source:String(i), 
        target: String(child)
      })
    });
    
  });

  const [nodes, setNodes] = useState(initialNodes);
  const [edges, setEdges] = useState(initialEdges);
  const nodeTypes = {
    sudokuStep: SudokuStep
  }

  return (
    <div className='stepsDisplay'>
      <ReactFlow
        nodes={nodes}
        edges={edges}
        nodeTypes={nodeTypes}
        fitView
      />
    </div>
  );
}