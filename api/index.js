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


app.post('/generate', (req, res)=>{
  console.log(req.body.difficoulty);
  const result = execSync(`.\\${ALGORITHMS_FOLDER}\\generator.exe ${req.body.difficoulty}`).toString();
  return res.json(result);
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
