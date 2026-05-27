# Sudoku solver and generator
That's about it.

# Wroks on
- windows 11 and prolly 10

# How to install and run
1. download the repo
2. run `npm install` in `/api` and then do the same in `/frontend`
3. compile the c++ scripts. in /api/algorithms run:
  - g++ solver.cpp -o solver.exe
  - g++ generator.cpp -o generator.exe
  - g++ hint.cpp -o hint.exe
  if you have some other compiler than g++, then figure the compiling out on your own. i don't actually know if it's going to work but it prolly will :D
4. to run the project, run `node .` in `/api` and in a seperate terminal, run `npm run dev` in `/frontend
