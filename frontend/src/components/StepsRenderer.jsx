import { useState, useCallback } from 'react';
import { ReactFlow, applyNodeChanges, applyEdgeChanges, addEdge, MarkerType } from '@xyflow/react';
import '@xyflow/react/dist/style.css';
import "../styles/stepsDisplay.css"
import testSteps from './testSteps';
import Stack from './Stack';
import testPreSolvedState from './testPreSolvedState';
import SudokuStep from './Step';
import { useEffect } from 'react';


class Step{
  constructor(step, state, parent, children){
    this.step = step;
    this.state = state;
    this.parent = parent
    this.children = children;
    this.XDepth = 0;
    this.YDepth = 0;
  }
}

export default function StepsRenderer(props) {
  const preSolvedState = props.preSolvedState == undefined ? "" : props.preSolvedState
  const steps = props.steps == undefined ? [] : props.steps;
  const [nodes, setNodes] = useState(undefined);
  const [edges, setEdges] = useState(undefined);
  

  let graph = [];
  let initialEdges = [];

  //identificators of the parents of "guess" nodes
  let goBacks = new Stack();

  for(let i = 0; i<steps.length; i++){
    let parent = i - 1 < 0 ? 0 : i - 1;
    
    if(steps[i].msg == "guess"){
      goBacks.push(i-1);
    }else if(steps[i].msg == "cancel guess"){
      parent = goBacks.pop();
    }
    
    let state = i - 1 < 0 ? preSolvedState + steps[i].fillIns: graph[parent].state + steps[i].fillIns;

    let newNode = new Step(steps[i], state, parent, []); 
    graph.push(newNode);
    graph[parent].children.push(i);
  }

  for(let i = 0; i<graph.length; i++){
    if(graph[i].step.msg == "cancel guess"){
      for(let child = 0; child<graph[i].children.length; child++){
        graph[graph[i].parent].children.push(graph[i].children[child]);
      }
    }
  }



  let initialNodes = [];
  let currentYDepth = 0;
  let currentXDepth = 0;
  let xDepths = []

  graph.forEach((ele, i) => {
    currentXDepth += 1;
    if(ele.step.msg == "guess" && graph[i-1].step.msg == "cancel guess"){
      currentYDepth += 1;
    }else if(ele.step.msg == "cancel guess"){
      
      currentXDepth = graph[ele.parent].XDepth + 1;
      // yDepth += 1; 
      return;
    }

    ele.XDepth = currentXDepth;
    ele.YDepth = currentYDepth;

    initialNodes.push({
      id: String(i),
      type:'sudokuStep',
      position: {x: 450 * currentXDepth, y: 550 * currentYDepth},
      data: {
        title: ele.step.msg,
        state: ele.state,
        fillIns: ele.step.fillIns
      }
    })

    ele.children.forEach(child => {
      if(child != 0){//stops the first node from being it's own child
        initialEdges.push({
          id: String(i)+"-"+String(child), 
          source:String(i), 
          target: String(child),
          MarkerType: MarkerType.Arrow
        })
      }
    });
    
  });
  
  const nodeTypes = {
    sudokuStep: SudokuStep
  }

  useEffect(()=>{
    setEdges(initialEdges);
    setNodes(initialNodes);    
  }, [props.preSolvedState, props.steps])

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