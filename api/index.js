import express from 'express'
import cors from 'cors'
import fs, { writeFileSync } from 'node:fs'
import {execSync} from 'child_process'
import settings from "../settings.js"

const app = express()
const port = settings.apiPort;
const ALGORITHMS_FOLDER = 'algorithms'
const INPUT_STATE_FILE_NAME = `input_state.txt`
const __dirname = import.meta.dirname;
const INPUT_STATE_FILE_ABS_PATH = `${__dirname}\\${ALGORITHMS_FOLDER}\\${INPUT_STATE_FILE_NAME}`

app.use(express.json())
app.use(cors())

app.post('/solve', (req, res)=>{
  let state = req.body.state; 
  fs.writeFileSync(INPUT_STATE_FILE_ABS_PATH, state)
  state = JSON.parse(execSync(`.\\${ALGORITHMS_FOLDER}\\solver.exe ${INPUT_STATE_FILE_ABS_PATH}`).toString());
  return res.json(state)
})


app.get('/generate-problem', (req, res)=>{
  execSync(`.\\${ALGORITHMS_FOLDER}\\generator.exe`); 
  const state = JSON.parse(fs.readFileSync(`.\\${ALGORITHMS_FOLDER}\\${OUTPUT_STATE_FILE}`, {encoding:"utf-8", flag:'r'}));
  return res.json(state);
})

app.post('/hint', (req, res)=>{
  let state = req.body.state;
  fs.writeFileSync(INPUT_STATE_FILE_ABS_PATH, state);
  state = JSON.parse(execSync(`.\\${ALGORITHMS_FOLDER}\\hint.exe ${INPUT_STATE_FILE_ABS_PATH}`).toString());
  console.log(state);
  return res.json(state);
})

app.listen(port, () => {
  console.log(`App listening on port ${port}`)
})
