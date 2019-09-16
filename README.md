# Tic-Tac-Toe-game
Tic-tac-toe game strategy with Alpha-Beta Prunning (UNSW COMP9414 Project)

Game is played on a 3x3 array of 3x3 Tic-Tac-Toe boards. First move is chosen at random by X on a randomly chosen board.
Then two players take turns placing an O or X alternately into an empty cell of the board corresponding to the cell of the previous move.
i.e. If previous move was on upper right corner, then current player plays on upper right board.

### Code is tested on Ubuntu terminals

# How to Start
cd src  
make all

1. To play by yourself
./servt -x -o
then play by selecting one of 1~9 numbers for each move.

2. To play with computer
./servt -p 12345 -x (port number should be a 5-digit number)
then on different terminal,
./agent -p 12345 OR ./lookt -p 12345

3. To let two programs play each other
./servt -p 12345
./agent -p 12345
./lookt -p 12345

Note: for ./lookt -p 12345 -d 6 specifies maximum search depth (default is 9; reasonable range is 1 to 18).
