import express from 'express'
import fs from 'node:fs'
import { engine } from 'express-handlebars'
import {execSync} from 'child_process'

const app = express()
const port = 3000
const STATE_FILE = "state.txt"

app.use(express.urlencoded())
app.use(express.static('static'))
app.engine('handlebars', engine());
app.set('view engine', 'handlebars');
app.set('views', './views')

app.get('/', (req, res) => {
  res.render('home')
})

app.get('/solve/:state', (req, res)=>{
  let state = req.params.state;
  let solvable = true;
  
  fs.writeFileSync(STATE_FILE, state)
  execSync("solver.exe");
  state = JSON.parse(fs.readFileSync(STATE_FILE, {encoding:"utf-8", flag:'r'}));
  return res.json(state);
})

app.get('/generator', (req, res)=>{
  res.render('generator');
})

app.get('/generate-problem', (req, res)=>{
  execSync('generator.exe'); 
  const   state = JSON.parse(fs.readFileSync(STATE_FILE, {encoding:"utf-8", flag:'r'}));
  return res.json(state);
})

app.listen(port, () => {
  console.log(`App listening on port ${port}`)
})
