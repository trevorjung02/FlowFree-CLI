# Flow Free Command Line Solver
This program solves Flow Free puzzles. If you don't know what Flow Free is, screenshots from the game are shown below, with an unsolved board on the left, and a solved board on the right.
<p float="left">
<img src="https://res.cloudinary.com/emrys/image/upload/v1639993017/flowfree/jumbo14x14_1_empty_blwgcs.png" alt="unsolved board" width="400"/>
<img src="https://res.cloudinary.com/emrys/image/upload/v1639993140/flowfree/jumbo14x14_1_solved_madswu.png" alt="solved board" width="400"/>
</p>
  
## Instructions for Windows users:
Download and unzip the latest release. No need to install.
### Run without input file: 
Run flowfree-cli.exe directly. Input the board in the console that pops up.
### Run with input file: 
Run the file flowfree-cli.exe in the command line. The input file should be a .txt file with the board. Example shown below: \
`flowfree-cli.exe <path/to/input/board.txt> `

### Board Format: 
Use letters to represent the colors and periods to represent empty spaces. An example 14x14 board is shown below: \
....g......... \
.............. \
.......o...... \
..h..cfk..i... \
...cf......... \
........o..... \
e.b.....b..... \
...l....l..... \
.......g...... \
a............. \
...m..a.i.d... \
....n.d.k.h... \
..e.....m..... \
........nj...j 
