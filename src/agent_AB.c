/*********************************************************
 *  agent.c
 *  Nine-Board Tic-Tac-Toe Agent
 *  COMP3411/9414/9814 Artificial Intelligence
 *  Alan Blair, CSE, UNSW
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "common.h"
#include "agent.h"
#include "game.h"

#define MAX_MOVE 81

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) > (b)) ? (b) : (a))

int board[10][10];
int cpBoard[10][10]; //just an idea
int move[MAX_MOVE+1];
int player;
int m;
//int alpha;
//int beta;
//int max_depth;
/*********************************************************//*
   Print usage information and exit
*/
void usage( char argv0[] )
{
  printf("Usage: %s\n",argv0);
  printf("       [-p port]\n"); // tcp port
  printf("       [-h host]\n"); // tcp host
  exit(1);
}

/*********************************************************//*
   Parse command-line arguments
*/
void agent_parse_args( int argc, char *argv[] )
{
  int i=1;
  while( i < argc ) {
    if( strcmp( argv[i], "-p" ) == 0 ) {
      if( i+1 >= argc ) {
        usage( argv[0] );
      }
      port = atoi(argv[i+1]);
      i += 2;
    }
    else if( strcmp( argv[i], "-h" ) == 0 ) {
      if( i+1 >= argc ) {
        usage( argv[0] );
      }
      host = argv[i+1];
      i += 2;
    }
    else {
      usage( argv[0] );
    }
  }
}

/*********************************************************//*
   Called at the beginning of a series of games
*/
void agent_init()
{
  struct timeval tp;

  // generate a new random seed each time
  gettimeofday( &tp, NULL );
  srandom(( unsigned int )( tp.tv_usec ));
}

/*********************************************************//*
   Called at the beginning of each game
*/
void agent_start( int this_player )
{
  reset_board( board );
  m = 0;
  move[m] = 0;
  player = this_player;
}

/*********************************************************//*
   Choose second move and return it
   board_num: which board prev_move was made
   prev_move: which position the move was made within the board


*/
int agent_second_move( int board_num, int prev_move )
{
  int this_move;
  //printf("The coordinates I got at second move is %d,%d", prev_move, board_num);
  move[0] = board_num;
  move[1] = prev_move;
  board[board_num][prev_move] = !player;
  m = 2;
  do {
    this_move = 1 + random()% 9;
  } while( board[prev_move][this_move] != EMPTY );
  move[m] = this_move;
  board[prev_move][this_move] = player;
  return( this_move );
}

/*********************************************************//*
   Choose third move and return it
*/
int agent_third_move(
                     int board_num,
                     int first_move,
                     int prev_move
                    )
{
  int this_move;
  move[0] = board_num;
  move[1] = first_move;
  move[2] = prev_move;
  board[board_num][first_move] =  player;
  board[first_move][prev_move] = !player;
  m=3;
  do {
    this_move = 1 + random()% 9;
  } while( board[prev_move][this_move] != EMPTY );
  move[m] = this_move;
  board[move[m-1]][this_move] = player;
  return( this_move );
}

/*********************************************************//*
   Choose next move and return it
*/
int agent_next_move( int prev_move )
{
  //printf("Here is my next move");
  int this_move;
  //Updating the board given previous move.
  m++;
  move[m] = prev_move;
  board[move[m-1]][move[m]] = !player;
  //Updating move number for current move.
  m++;

  //This is just for initialization for value this_move
  //do {
  //  this_move = 1 + random()% 9;
  //} while( board[prev_move][this_move] != EMPTY );

  //Initializing alpha-beta for pruning
  int alpha = -10000;
  int beta = 10000;

  //if looking at all depth, search takes too long.
  int max_depth = 0;
  max_depth = (2*m) - (2 * (m/1.5));
  if (max_depth > 8)
  {
    max_depth = 7;
  }

  //Calling minmax_algorithm
  int maxScore = -10000;
  this_move = -1;
  printf("Previous move was made to %d\n", prev_move);
  printf("Current depth search is to %d\n", max_depth);
  int myValues [10];
  for(int p = 1; p < 10; p++)
  {
    if(board[prev_move][p] == EMPTY)
    {
      //printf("I will be trying at board=%d, position=%d\n", prev_move, p);
      //Make potential move
      board[prev_move][p] = player;
      //int moveScore = Minimax(board, p, 0, max_depth);
      int moveScore = AlphaBetaSearch(board, prev_move, p, 0, max_depth, alpha, beta);
      board[prev_move][p] = EMPTY;
      myValues[p] = moveScore;
      printf("My move score for board[%d][%d] is %d\n", prev_move, p, myValues[p]);

      //Need someway to say avoid -100 and choose from 0 if there is no 100.
      //Maybe intermediate heuristic is needed.
      //i.e. if there is two in row and empty spot, we return 50.
      if(maxScore < moveScore)
      {
        maxScore = moveScore;
        this_move = p;
        printf("Current max score is %d at position=%d \n", maxScore, this_move);
      }
    }
  }

  if(maxScore == 0)
  {
    int tempMax = 2;
    int tempAlpha = -10000;
    int tempBeta = 10000;
    for(int t = 1; t < 10; t++)
    {
      if(board[prev_move][t] == EMPTY)
      {
        board[prev_move][t] = player;
        int tempScore = intermediateScore(board, prev_move, t);
        printf("My move score for board[%d][%d] is %d\n", prev_move, t, tempScore);
        board[prev_move][t] = EMPTY;
        if(maxScore < tempScore && myValues[t] >= 0)
        {
          maxScore = tempScore;
          this_move = t;
        }
      }
    }
  }

  printf("Returning %d as my move!\n", this_move);
  move[m] = this_move;
  board[move[m-1]][this_move] = player;
  return( this_move );
}

/*********************************************************//*
Get_BOI()
input: board[10][10], prev_move
output: board[3][3]

Get board of interest.

Would this be necessary?
If board and previous_move is passed in, we can loop through to extract
board of interest values.

*/
int **GetBOI(int myBoard[10][10], int board_num)
{
  int **boi = malloc(sizeof(int*)*3);
  for(int i =0; i < 3; i++)
  {
    boi[i] = malloc(sizeof(int*) * 3);
  }

  boi[0][0] = myBoard[board_num][1];
  boi[0][1] = myBoard[board_num][2];
  boi[0][2] = myBoard[board_num][3];
  boi[1][0] = myBoard[board_num][4];
  boi[1][1] = myBoard[board_num][5];
  boi[1][2] = myBoard[board_num][6];
  boi[2][0] = myBoard[board_num][7];
  boi[2][1] = myBoard[board_num][8];
  boi[2][2] = myBoard[board_num][9];

  return boi;

}

/*********************************************************//*
MinMax_Algorithm()
input: board, Max or Min, depth
output: next move?

make the move and recursively examine the possibilities following the move.
at each depth determine whether it is good or not.
From each depth, always return win/draw, lose/draw based on minmax.
When earliest win is found, make the move based on the information.

board will be for current playing board not the whole board

//With just minmax, the move takes too long, since there are too many depth.
Thus, need to implement alpha-beta pruning.

*/
int Minimax(int myBoard[10][10], int numBoard, int myTurn, int depth)
{

  //Base case for recursive -> terminal state
  int score = boardscore(myBoard, numBoard);
  if(score == 100 || score == -100)
      return score;

  //If true, then game is a draw.
  if(no_more_move_board(myBoard, numBoard) == 1)
    return 0;

  //we stop at this depth!
  if(depth == 0)
    return 0;

  //Now recursive step here.
  //Check if the state is for max or min.
  if(myTurn == 1)
  {
      //This is my move. I want maximum score
      int maxVal = -10000;
      for(int p = 1; p < 10; p++)
      {
        if(myBoard[numBoard][p] == EMPTY)
        {
          //we are exploring all possibilities (for now).
          //We try this move if empty
          myBoard[numBoard][p] = player;

          //Now official recursive step!
          maxVal = max(maxVal, Minimax(myBoard, p, 0, depth-1));

          //we do not want to make this move official
          myBoard[numBoard][p] = EMPTY;

        }
      }
      return maxVal;
  }
  else //if (myTurn == 0)
  {
    //This one is for opponent. I want minimum score
    int minVal = 10000;
    for(int p = 1; p < 10; p++)
    {
      if(myBoard[numBoard][p] == EMPTY)
      {
        //we are exploring all possibilities (for now).
        //We try this move if empty
        myBoard[numBoard][p] = !player;

        //Now official recursive step!
        minVal = min(minVal, Minimax(myBoard, p, 1, depth-1));

        //we do not want to make this move official
        myBoard[numBoard][p] = EMPTY;
      }
    }
    return minVal;
  }

}
/*********************************************************//*
AlphaBetaSearch()
input: board, Max or Min, depth, alpha, beta
output: heuristic value

same as minimax except alpha-beta pruning added.
*/
int AlphaBetaSearch(int myBoard[10][10], int currentBoard, int nextBoard, int myTurn, int depth, int alpha, int beta)
{

  //Base case for recursive //this is checking current board
  int score = boardscore(myBoard, currentBoard);
  if(score == 100 || score == -100)
  {
      //printf("For board[%d][%d], If I ever do score!: %d at depth=%d\n", currentBoard,nextBoard, score, depth);
      return score;
  }
  //If true, then game is a draw at this search.
  if(no_more_move_board(myBoard, currentBoard) == 1)
  {
    //printf("Am I evaluating if there are no more moves at board\n");
    //int what = intermediateScore(myBoard, currentBoard, nextBoard);
    return 0;
  }
  //this is one of our terminal state.
  if(depth == 0)
  {
    //boardscore function will not take intermediate scoring into account.
    int intrimScore = intermediateScore(myBoard, currentBoard, nextBoard);
      //printf("At turn == %d, I got %d\n", myTurn, what);
    return intrimScore;
  }
  //Now recursive step here.
  //Check if the state is for max or min.
  if(myTurn == 1)
  {
      for(int p = 1; p < 10; p++)
      {
        if(myBoard[nextBoard][p] == EMPTY)
        {
          //we are exploring all possibilities (for now).
          //We try this move if empty
          myBoard[nextBoard][p] = player;

          //Now official recursive step!
          alpha = max(alpha, AlphaBetaSearch(myBoard, nextBoard, p, 0, depth-1, alpha, beta));
          //printf("Alpha at depth: %d is %d", depth-1, alpha);
          //we do not want to make this move official
          myBoard[nextBoard][p] = EMPTY;

          if(alpha >= beta)
            return alpha;
          //{
            //printf("Alpha Beta Pruning working on my potential move!\n");
            //return alpha;
          //}
        }
      }
      return alpha;
  }
  else //if (myTurn == 0)
  {
    for(int p = 1; p < 10; p++)
    {
      if(myBoard[nextBoard][p] == EMPTY)
      {
        //we are exploring all possibilities (for now).
        //We try this move if empty
        myBoard[nextBoard][p] = !player;

        //Now official recursive step!
        beta = min(beta, AlphaBetaSearch(myBoard, nextBoard, p, 1, depth-1, alpha, beta));
        //printf("beta at depth: %d is %d", depth-1, beta);
        //we do not want to make this move official
        myBoard[nextBoard][p] = EMPTY;

        if(alpha >= beta)
          return beta;
        //{
          //printf("Alpha Beta Pruning working on opponent's move!\n");
          //return beta;
        //}
      }
    }
    return beta;
  }

}
/*********************************************************//*
full_board()
xinput: board[3][3] //current board
input: myBoard[10][10], prev_move
output: True or False

Check if board is full by going over the board checking if there is avail spot.

*/
int no_more_move_board(int myBoard[10][10], int numBoard)
{
  //if all are not empty then we say no more move!
  for(int p = 1; p < 10; p++)
  {
    if(myBoard[numBoard][p] == EMPTY)
      return 0;
  }
  return 1;
}

/*********************************************************//*
Score()
input: board, board number
output: score(100, 0, -100)

Given board, evaluate outcome.
Now would board be evaluated based on whole board?
Or only the board of interest? for now it will be board of interest

Scores are 100, -100 to avoid confusion between move and score from output of minmax algorithm
if this.agent won, score = 100
draw = 0
else score = -100

*/
int boardscore(int myBoard[10][10], int numBoard)
{
  //Check rows
  for(int iter = 1; iter < 8; iter += 3)
  {
    if(myBoard[numBoard][iter] == myBoard[numBoard][iter+1] && myBoard[numBoard][iter+1] == myBoard[numBoard][iter+2])
    {
      if(myBoard[numBoard][iter] == player)
        return 100;
      else if (myBoard[numBoard][iter] == !player)
        return -100;
    }
  }
  //Check cols
  for(int iter = 1; iter < 4; iter++)
  {
    if(myBoard[numBoard][iter] == myBoard[numBoard][iter+3] && myBoard[numBoard][iter+3] == myBoard[numBoard][iter+6])
    {
      if(myBoard[numBoard][iter] == player)
        return 100;
      else if(myBoard[numBoard][iter] == !player)
        return -100;
    }
  }
  //Check diags 1-5-9
  if(myBoard[numBoard][1] == myBoard[numBoard][5] && myBoard[numBoard][5] == myBoard[numBoard][9])
  {
    if(myBoard[numBoard][1] == player)
      return 100;
    else if(myBoard[numBoard][1] == !player)
      return -100;
  }
  //check diags 3-5-7
  if(myBoard[numBoard][3] == myBoard[numBoard][5] && myBoard[numBoard][5] == myBoard[numBoard][7])
  {
    if(myBoard[numBoard][3] == player)
      return 100;
    else if(myBoard[numBoard][3] == !player)
      return -100;
  }

  //if none the above, it is a draw or still playing.
  return 0;


}
/*********************************************************//*
Following is the intermediate step
Cases:
  1) Blocked opponents 2 in a row -75
  2) Making 2 in a row with third empty -50
  3) Favoring 5 > 1,3,7,9 > 2,4,6,8 on empty board -25 (Not Yet)

Defensive:
1) Blocked 2 in a row, col, diag
2) If corner occupied, occupy the other corner

Offensive:
1) Made 2 in a row
2) occupied {1,3,5,7,9}

heuristic will be based on addition of all these.
i.e. if player blocked a 2 in a row while occupying the corner which opposite corner is empty,
add up the points.

Q1: Should all offensive/defensive have the same point or some should be valued more?
  - for now its all uniform.

*/
int intermediateScore(int myBoard[10][10], int currentBoard, int move)
{
  //brute force method.
  //int score = 0;





  //For row ()
  /*
  for(int i = 1; i < 8; i += 3)
  {
    //If stopped an attack (2)
    if(myBoard[currentBoard][i] != EMPTY && myBoard[currentBoard][i+1] != EMPTY && myBoard[currentBoard][i+2] != EMPTY)
    {
      if(myBoard[currentBoard][i] == myBoard[currentBoard][i+1] && myBoard[currentBoard][i+1] != myBoard[currentBoard][i+2])
      {
        if(myBoard[currentBoard][i + 1] == player)
          return 75;
        else if (myBoard[currentBoard][i + 1] == !player)
          return -75;
      }
      else if (myBoard[currentBoard][i] != myBoard[currentBoard][i+1] && myBoard[currentBoard][i+1] == myBoard[currentBoard][i+2])
      {
        if(myBoard[currentBoard][i] == player)
          return 75;
        else if (myBoard[currentBoard][i] == !player)
          return -75;
      }
    }
    else
    {
      //if made an attack (2)
      if(myBoard[currentBoard][i] == myBoard[currentBoard][i+1] && myBoard[currentBoard][i+2] == EMPTY)
      {
        if(myBoard[currentBoard][i] == player)
          return 75;
        else if (myBoard[currentBoard][i] == !player)
          return -75;
      }
      else if (myBoard[currentBoard][i] == myBoard[currentBoard][i+2] && myBoard[currentBoard][i+1] == EMPTY)
      {
        if(myBoard[currentBoard][i] == player)
          return 75;
        else if (myBoard[currentBoard][i] == !player)
          return -75;
      }
      else if (myBoard[currentBoard][i+1] == myBoard[currentBoard][i+2] && myBoard[currentBoard][i] == EMPTY)
      {
        if(myBoard[currentBoard][i+1] == player)
          return 75;
        else if (myBoard[currentBoard][i+1] == !player)
          return -75;
      }
      //Preference to {1,3,5,7,9}
      else if (myBoard[currentBoard][i] != EMPTY && myBoard[currentBoard][i+1] == EMPTY && myBoard[currentBoard][i+2] == EMPTY)
      {
        if (i == 1 || i == 3 || i == 5 || i == 7 || i == 8)
        {
          if(myBoard[currentBoard][i] == player)
            return 75;
          else if (myBoard[currentBoard][i] == !player)
            return -75;
        }
      }
      else if (myBoard[currentBoard][i] == EMPTY && myBoard[currentBoard][i+1] != EMPTY && myBoard[currentBoard][i+2] == EMPTY)
      {
        if (i == 1 || i == 3 || i == 5 || i == 7 || i == 8)
        {
          if(myBoard[currentBoard][i+1] == player)
            return 75;
          else if (myBoard[currentBoard][i+1] == !player)
            return -75;
        }
      }
      else if (myBoard[currentBoard][i] == EMPTY && myBoard[currentBoard][i+1] == EMPTY && myBoard[currentBoard][i+2] != EMPTY)
      {
        if (i == 1 || i == 3 || i == 5 || i == 7 || i == 8)
        {
          if(myBoard[currentBoard][i+2] == player)
            return 75;
          else if (myBoard[currentBoard][i+2] == !player)
            return -75;
        }
      }
    }
  }
  */






  //printf("%d is player, %d is !player\n", player, !player);
  //printf("%d is current board at depth = 0\n", currentBoard);
  //printf("%d, %d, %d is row1 of board\n", myBoard[currentBoard][1], myBoard[currentBoard][2], myBoard[currentBoard][3]);
  //printf("%d, %d, %d is row2 of board\n", myBoard[currentBoard][4], myBoard[currentBoard][5], myBoard[currentBoard][6]);
  //printf("%d, %d, %d is row3 of board\n", myBoard[currentBoard][7], myBoard[currentBoard][8], myBoard[currentBoard][9]);

  if (move == 1)
  {
    //Case1:
    //row
    if(myBoard[currentBoard][1] != EMPTY && myBoard[currentBoard][2] != EMPTY && myBoard[currentBoard][3] != EMPTY){
      if(myBoard[currentBoard][1] != myBoard[currentBoard][2] && myBoard[currentBoard][2] == myBoard[currentBoard][3])
      {
        if(myBoard[currentBoard][move] == player)
          return 75;
        else if (myBoard[currentBoard][move] == !player)
          return -75;
      }
    }
    //col
    else if(myBoard[currentBoard][1] != EMPTY && myBoard[currentBoard][4] != EMPTY && myBoard[currentBoard][7] != EMPTY){
      if(myBoard[currentBoard][1] != myBoard[currentBoard][4] && myBoard[currentBoard][4] == myBoard[currentBoard][7])
      {
        if(myBoard[currentBoard][move] == player)
          return 75;
        else if (myBoard[currentBoard][move] == !player)
          return -75;
      }
    }
    //diag
    else if(myBoard[currentBoard][1] != EMPTY && myBoard[currentBoard][5] != EMPTY && myBoard[currentBoard][9] != EMPTY){
      if(myBoard[currentBoard][1] != myBoard[currentBoard][5] && myBoard[currentBoard][5] == myBoard[currentBoard][9])
      {
        if(myBoard[currentBoard][move] == player)
          return 75;
        else if (myBoard[currentBoard][move] == !player)
          return -75;
      }
    }
    //Case2:
    //row
    else if((myBoard[currentBoard][1] == myBoard[currentBoard][2] && myBoard[currentBoard][3] == EMPTY) ||
             (myBoard[currentBoard][1] == myBoard[currentBoard][3] && myBoard[currentBoard][2] == EMPTY))
    {
      if(myBoard[currentBoard][move] == player)
        return 50;
      else if (myBoard[currentBoard][move] == !player)
        return -50;
    }
    else if((myBoard[currentBoard][1] == myBoard[currentBoard][4] && myBoard[currentBoard][7] == EMPTY) ||
             (myBoard[currentBoard][1] == myBoard[currentBoard][7] && myBoard[currentBoard][4] == EMPTY))
    {
      if(myBoard[currentBoard][move] == player)
        return 50;
      else if (myBoard[currentBoard][move] == !player)
        return -50;
    }
    else if((myBoard[currentBoard][1] == myBoard[currentBoard][5] && myBoard[currentBoard][9] == EMPTY) ||
             (myBoard[currentBoard][1] == myBoard[currentBoard][9] && myBoard[currentBoard][5] == EMPTY))
    {
      if(myBoard[currentBoard][move] == player)
        return 50;
      else if (myBoard[currentBoard][move] == !player)
        return -50;
    }
  }
  else if (move == 2)
  {
    //Case1:
    //row
    if(myBoard[currentBoard][1] != EMPTY && myBoard[currentBoard][2] != EMPTY && myBoard[currentBoard][3] != EMPTY){
      if(myBoard[currentBoard][2] != myBoard[currentBoard][1] && myBoard[currentBoard][1] == myBoard[currentBoard][3])
      {
        if(myBoard[currentBoard][move] == player)
          return 75;
        else if (myBoard[currentBoard][move] == !player)
          return -75;
      }
    }
    //col
    else if(myBoard[currentBoard][2] != EMPTY && myBoard[currentBoard][5] != EMPTY && myBoard[currentBoard][8] != EMPTY){
      if(myBoard[currentBoard][2] != myBoard[currentBoard][5] && myBoard[currentBoard][5] == myBoard[currentBoard][8])
      {
        if(myBoard[currentBoard][move] == player)
          return 75;
        else if (myBoard[currentBoard][move] == !player)
          return -75;
      }
    }
    //Case2:
    //row
    else if((myBoard[currentBoard][1] == myBoard[currentBoard][2] && myBoard[currentBoard][3] == EMPTY) ||
             (myBoard[currentBoard][2] == myBoard[currentBoard][3] && myBoard[currentBoard][1] == EMPTY))
    {
      if(myBoard[currentBoard][move] == player)
        return 50;
      else if (myBoard[currentBoard][move] == !player)
        return -50;
    }
    else if((myBoard[currentBoard][2] == myBoard[currentBoard][5] && myBoard[currentBoard][8] == EMPTY) ||
             (myBoard[currentBoard][2] == myBoard[currentBoard][8] && myBoard[currentBoard][5] == EMPTY))
    {
      if(myBoard[currentBoard][move] == player)
        return 50;
      else if (myBoard[currentBoard][move] == !player)
        return -50;
    }
  }
  else if (move == 3)
  {
    //Case1:
    //row
    if(myBoard[currentBoard][1] != EMPTY && myBoard[currentBoard][2] != EMPTY && myBoard[currentBoard][3] != EMPTY){
      if(myBoard[currentBoard][3] != myBoard[currentBoard][1] && myBoard[currentBoard][1] == myBoard[currentBoard][2])
      {
        if(myBoard[currentBoard][move] == player)
          return 75;
        else if (myBoard[currentBoard][move] == !player)
          return -75;
      }
    }
    //col
    else if(myBoard[currentBoard][3] != EMPTY && myBoard[currentBoard][6] != EMPTY && myBoard[currentBoard][9] != EMPTY){
      if(myBoard[currentBoard][3] != myBoard[currentBoard][6] && myBoard[currentBoard][6] == myBoard[currentBoard][9])
      {
        if(myBoard[currentBoard][move] == player)
          return 75;
        else if (myBoard[currentBoard][move] == !player)
          return -75;
      }
    }
    //diag
    else if(myBoard[currentBoard][3] != EMPTY && myBoard[currentBoard][5] != EMPTY && myBoard[currentBoard][7] != EMPTY){
      if(myBoard[currentBoard][3] != myBoard[currentBoard][5] && myBoard[currentBoard][5] == myBoard[currentBoard][7])
      {
        if(myBoard[currentBoard][move] == player)
          return 75;
        else if (myBoard[currentBoard][move] == !player)
          return -75;
      }
    }
    //Case2:
    //row
    else if((myBoard[currentBoard][3] == myBoard[currentBoard][2] && myBoard[currentBoard][1] == EMPTY) ||
             (myBoard[currentBoard][3] == myBoard[currentBoard][1] && myBoard[currentBoard][2] == EMPTY))
    {
      if(myBoard[currentBoard][move] == player)
        return 50;
      else if (myBoard[currentBoard][move] == !player)
        return -50;
    }
    else if((myBoard[currentBoard][3] == myBoard[currentBoard][6] && myBoard[currentBoard][9] == EMPTY) ||
             (myBoard[currentBoard][3] == myBoard[currentBoard][9] && myBoard[currentBoard][6] == EMPTY))
    {
      if(myBoard[currentBoard][move] == player)
        return 50;
      else if (myBoard[currentBoard][move] == !player)
        return -50;
    }
    else if((myBoard[currentBoard][3] == myBoard[currentBoard][5] && myBoard[currentBoard][7] == EMPTY) ||
             (myBoard[currentBoard][3] == myBoard[currentBoard][7] && myBoard[currentBoard][5] == EMPTY))
    {
      if(myBoard[currentBoard][move] == player)
        return 50;
      else if (myBoard[currentBoard][move] == !player)
        return -50;
    }
  }
  else if (move == 4)
  {
    //Case1:
    //row
    if(myBoard[currentBoard][4] != EMPTY && myBoard[currentBoard][5] != EMPTY && myBoard[currentBoard][6] != EMPTY){
      if(myBoard[currentBoard][4] != myBoard[currentBoard][5] && myBoard[currentBoard][5] == myBoard[currentBoard][6])
      {
        if(myBoard[currentBoard][move] == player)
          return 75;
        else if (myBoard[currentBoard][move] == !player)
          return -75;
      }
    }
    //col
    else if(myBoard[currentBoard][4] != EMPTY && myBoard[currentBoard][1] != EMPTY && myBoard[currentBoard][7] != EMPTY){
      if(myBoard[currentBoard][4] != myBoard[currentBoard][1] && myBoard[currentBoard][1] == myBoard[currentBoard][7])
      {
        if(myBoard[currentBoard][move] == player)
          return 75;
        else if (myBoard[currentBoard][move] == !player)
          return -75;
      }
    }
    //Case2:
    //row
    else if((myBoard[currentBoard][4] == myBoard[currentBoard][5] && myBoard[currentBoard][6] == EMPTY) ||
             (myBoard[currentBoard][4] == myBoard[currentBoard][6] && myBoard[currentBoard][5] == EMPTY))
    {
      if(myBoard[currentBoard][move] == player)
        return 50;
      else if (myBoard[currentBoard][move] == !player)
        return -50;
    }
    else if((myBoard[currentBoard][4] == myBoard[currentBoard][1] && myBoard[currentBoard][7] == EMPTY) ||
             (myBoard[currentBoard][4] == myBoard[currentBoard][7] && myBoard[currentBoard][1] == EMPTY))
    {
      if(myBoard[currentBoard][move] == player)
        return 50;
      else if (myBoard[currentBoard][move] == !player)
        return -50;
    }
  }
  else if (move == 5)
  {
    //Case1:
    //row
    if(myBoard[currentBoard][4] != EMPTY && myBoard[currentBoard][5] != EMPTY && myBoard[currentBoard][6] != EMPTY){
      if(myBoard[currentBoard][5] != myBoard[currentBoard][4] && myBoard[currentBoard][4] == myBoard[currentBoard][6])
      {
        if(myBoard[currentBoard][move] == player)
          return 75;
        else if (myBoard[currentBoard][move] == !player)
          return -75;
      }
    }
    //col
    else if(myBoard[currentBoard][2] != EMPTY && myBoard[currentBoard][5] != EMPTY && myBoard[currentBoard][8] != EMPTY){
      if(myBoard[currentBoard][5] != myBoard[currentBoard][2] && myBoard[currentBoard][2] == myBoard[currentBoard][8])
      {
        if(myBoard[currentBoard][move] == player)
          return 75;
        else if (myBoard[currentBoard][move] == !player)
          return -75;
      }
    }
    //diag
    else if(myBoard[currentBoard][1] != EMPTY && myBoard[currentBoard][5] != EMPTY && myBoard[currentBoard][9] != EMPTY){
      if(myBoard[currentBoard][5] != myBoard[currentBoard][1] && myBoard[currentBoard][1] == myBoard[currentBoard][9])
      {
        if(myBoard[currentBoard][move] == player)
          return 75;
        else if (myBoard[currentBoard][move] == !player)
          return -75;
      }
    }
    else if(myBoard[currentBoard][3] != EMPTY && myBoard[currentBoard][5] != EMPTY && myBoard[currentBoard][7] != EMPTY){
      if(myBoard[currentBoard][5] != myBoard[currentBoard][3] && myBoard[currentBoard][3] == myBoard[currentBoard][7])
      {
        if(myBoard[currentBoard][move] == player)
          return 75;
        else if (myBoard[currentBoard][move] == !player)
          return -75;
      }
    }
    //Case2:
    //row
    else if((myBoard[currentBoard][5] == myBoard[currentBoard][4] && myBoard[currentBoard][6] == EMPTY) ||
             (myBoard[currentBoard][5] == myBoard[currentBoard][6] && myBoard[currentBoard][4] == EMPTY))
    {
      if(myBoard[currentBoard][move] == player)
        return 50;
      else if (myBoard[currentBoard][move] == !player)
        return -50;
    }
    else if((myBoard[currentBoard][5] == myBoard[currentBoard][2] && myBoard[currentBoard][8] == EMPTY) ||
             (myBoard[currentBoard][5] == myBoard[currentBoard][8] && myBoard[currentBoard][2] == EMPTY))
    {
      if(myBoard[currentBoard][move] == player)
        return 50;
      else if (myBoard[currentBoard][move] == !player)
        return -50;
    }
    else if((myBoard[currentBoard][5] == myBoard[currentBoard][1] && myBoard[currentBoard][9] == EMPTY) ||
             (myBoard[currentBoard][5] == myBoard[currentBoard][9] && myBoard[currentBoard][1] == EMPTY))
    {
      if(myBoard[currentBoard][move] == player)
        return 50;
      else if (myBoard[currentBoard][move] == !player)
        return -50;
    }
    else if((myBoard[currentBoard][5] == myBoard[currentBoard][3] && myBoard[currentBoard][7] == EMPTY) ||
             (myBoard[currentBoard][5] == myBoard[currentBoard][7] && myBoard[currentBoard][3] == EMPTY))
    {
      if(myBoard[currentBoard][move] == player)
        return 50;
      else if (myBoard[currentBoard][move] == !player)
        return -50;
    }
  }
  else if (move == 6)
  {
    //Case1:
    //row
    if(myBoard[currentBoard][4] != EMPTY && myBoard[currentBoard][5] != EMPTY && myBoard[currentBoard][6] != EMPTY){
      if(myBoard[currentBoard][6] != myBoard[currentBoard][5] && myBoard[currentBoard][5] == myBoard[currentBoard][4])
      {
        if(myBoard[currentBoard][move] == player)
          return 75;
        else if (myBoard[currentBoard][move] == !player)
          return -75;
      }
    }
    //col
    else if(myBoard[currentBoard][6] != EMPTY && myBoard[currentBoard][3] != EMPTY && myBoard[currentBoard][9] != EMPTY){
      if(myBoard[currentBoard][6] != myBoard[currentBoard][3] && myBoard[currentBoard][3] == myBoard[currentBoard][9])
      {
        if(myBoard[currentBoard][move] == player)
          return 75;
        else if (myBoard[currentBoard][move] == !player)
          return -75;
      }
    }
    //Case2:
    //row
    else if((myBoard[currentBoard][6] == myBoard[currentBoard][5] && myBoard[currentBoard][4] == EMPTY) ||
             (myBoard[currentBoard][6] == myBoard[currentBoard][4] && myBoard[currentBoard][5] == EMPTY))
    {
      if(myBoard[currentBoard][move] == player)
        return 50;
      else if (myBoard[currentBoard][move] == !player)
        return -50;
    }
    else if((myBoard[currentBoard][6] == myBoard[currentBoard][3] && myBoard[currentBoard][9] == EMPTY) ||
             (myBoard[currentBoard][6] == myBoard[currentBoard][9] && myBoard[currentBoard][3] == EMPTY))
    {
      if(myBoard[currentBoard][move] == player)
        return 50;
      else if (myBoard[currentBoard][move] == !player)
        return -50;
    }
  }
  else if (move == 7)
  {
    //Case1:
    //row
    if(myBoard[currentBoard][7] != EMPTY && myBoard[currentBoard][8] != EMPTY && myBoard[currentBoard][9] != EMPTY){
      if(myBoard[currentBoard][7] != myBoard[currentBoard][8] && myBoard[currentBoard][8] == myBoard[currentBoard][9])
      {
        if(myBoard[currentBoard][move] == player)
          return 75;
        else if (myBoard[currentBoard][move] == !player)
          return -75;
      }
    }
    //col
    else if(myBoard[currentBoard][7] != EMPTY && myBoard[currentBoard][4] != EMPTY && myBoard[currentBoard][1] != EMPTY){
      if(myBoard[currentBoard][7] != myBoard[currentBoard][4] && myBoard[currentBoard][4] == myBoard[currentBoard][1])
      {
        if(myBoard[currentBoard][move] == player)
          return 75;
        else if (myBoard[currentBoard][move] == !player)
          return -75;
      }
    }
    //diag
    else if(myBoard[currentBoard][7] != EMPTY && myBoard[currentBoard][5] != EMPTY && myBoard[currentBoard][3] != EMPTY){
      if(myBoard[currentBoard][7] != myBoard[currentBoard][5] && myBoard[currentBoard][5] == myBoard[currentBoard][3])
      {
        if(myBoard[currentBoard][move] == player)
          return 75;
        else if (myBoard[currentBoard][move] == !player)
          return -75;
      }
    }
    //Case2:
    //row
    else if((myBoard[currentBoard][7] == myBoard[currentBoard][8] && myBoard[currentBoard][9] == EMPTY) ||
             (myBoard[currentBoard][7] == myBoard[currentBoard][8] && myBoard[currentBoard][9] == EMPTY))
    {
      if(myBoard[currentBoard][move] == player)
        return 50;
      else if (myBoard[currentBoard][move] == !player)
        return -50;
    }
    else if((myBoard[currentBoard][7] == myBoard[currentBoard][4] && myBoard[currentBoard][1] == EMPTY) ||
             (myBoard[currentBoard][7] == myBoard[currentBoard][4] && myBoard[currentBoard][1] == EMPTY))
    {
      if(myBoard[currentBoard][move] == player)
        return 50;
      else if (myBoard[currentBoard][move] == !player)
        return -50;
    }
    else if((myBoard[currentBoard][7] == myBoard[currentBoard][5] && myBoard[currentBoard][3] == EMPTY) ||
             (myBoard[currentBoard][7] == myBoard[currentBoard][3] && myBoard[currentBoard][5] == EMPTY))
    {
      if(myBoard[currentBoard][move] == player)
        return 50;
      else if (myBoard[currentBoard][move] == !player)
        return -50;
    }
  }
  else if (move == 8)
  {
    //Case1:
    //row
    if(myBoard[currentBoard][7] != EMPTY && myBoard[currentBoard][8] != EMPTY && myBoard[currentBoard][9] != EMPTY){
      if(myBoard[currentBoard][8] != myBoard[currentBoard][7] && myBoard[currentBoard][7] == myBoard[currentBoard][9])
      {
        if(myBoard[currentBoard][move] == player)
          return 75;
        else if (myBoard[currentBoard][move] == !player)
          return -75;
      }
    }
    //col
    else if(myBoard[currentBoard][8] != EMPTY && myBoard[currentBoard][5] != EMPTY && myBoard[currentBoard][2] != EMPTY){
      if(myBoard[currentBoard][8] != myBoard[currentBoard][5] && myBoard[currentBoard][5] == myBoard[currentBoard][2])
      {
        if(myBoard[currentBoard][move] == player)
          return 75;
        else if (myBoard[currentBoard][move] == !player)
          return -75;
      }
    }
    //Case2:
    //row
    else if((myBoard[currentBoard][8] == myBoard[currentBoard][7] && myBoard[currentBoard][9] == EMPTY) ||
             (myBoard[currentBoard][8] == myBoard[currentBoard][9] && myBoard[currentBoard][7] == EMPTY))
    {
      if(myBoard[currentBoard][move] == player)
        return 50;
      else if (myBoard[currentBoard][move] == !player)
        return -50;
    }
    else if((myBoard[currentBoard][8] == myBoard[currentBoard][5] && myBoard[currentBoard][2] == EMPTY) ||
             (myBoard[currentBoard][8] == myBoard[currentBoard][2] && myBoard[currentBoard][5] == EMPTY))
    {
      if(myBoard[currentBoard][move] == player)
        return 50;
      else if (myBoard[currentBoard][move] == !player)
        return -50;
    }
  }
  else if (move == 9)
  {
    //Case1:
    //row
    if(myBoard[currentBoard][9] != EMPTY && myBoard[currentBoard][8] != EMPTY && myBoard[currentBoard][7] != EMPTY){
      if(myBoard[currentBoard][9] != myBoard[currentBoard][8] && myBoard[currentBoard][8] == myBoard[currentBoard][7])
      {
        if(myBoard[currentBoard][move] == player)
          return 75;
        else if (myBoard[currentBoard][move] == !player)
          return -75;
      }
    }
    //col
    else if(myBoard[currentBoard][9] != EMPTY && myBoard[currentBoard][6] != EMPTY && myBoard[currentBoard][3] != EMPTY){
      if(myBoard[currentBoard][9] != myBoard[currentBoard][6] && myBoard[currentBoard][6] == myBoard[currentBoard][3])
      {
        if(myBoard[currentBoard][move] == player)
          return 75;
        else if (myBoard[currentBoard][move] == !player)
          return -75;
      }
    }
    //diag
    else if(myBoard[currentBoard][9] != EMPTY && myBoard[currentBoard][5] != EMPTY && myBoard[currentBoard][1] != EMPTY){
      if(myBoard[currentBoard][9] != myBoard[currentBoard][5] && myBoard[currentBoard][5] == myBoard[currentBoard][1])
      {
        if(myBoard[currentBoard][move] == player)
          return 75;
        else if (myBoard[currentBoard][move] == !player)
          return -75;
      }
    }
    //Case2:
    //row
    else if((myBoard[currentBoard][9] == myBoard[currentBoard][8] && myBoard[currentBoard][7] == EMPTY) ||
             (myBoard[currentBoard][9] == myBoard[currentBoard][7] && myBoard[currentBoard][8] == EMPTY))
    {
      if(myBoard[currentBoard][move] == player)
        return 50;
      else if (myBoard[currentBoard][move] == !player)
        return -50;
    }
    else if((myBoard[currentBoard][9] == myBoard[currentBoard][6] && myBoard[currentBoard][3] == EMPTY) ||
             (myBoard[currentBoard][9] == myBoard[currentBoard][3] && myBoard[currentBoard][6] == EMPTY))
    {
      if(myBoard[currentBoard][move] == player)
        return 50;
      else if (myBoard[currentBoard][move] == !player)
        return -50;
    }
    else if((myBoard[currentBoard][9] == myBoard[currentBoard][5] && myBoard[currentBoard][1] == EMPTY) ||
             (myBoard[currentBoard][9] == myBoard[currentBoard][1] && myBoard[currentBoard][5] == EMPTY))
    {
      if(myBoard[currentBoard][move] == player)
        return 50;
      else if (myBoard[currentBoard][move] == !player)
        return -50;
    }
  }
  return 0;
}
/*********************************************************//*
   Receive last move and mark it on the board
*/
void agent_last_move( int prev_move )
{
  m++;
  move[m] = prev_move;
  board[move[m-1]][move[m]] = !player;
}

/*********************************************************//*
   Called after each game
*/
void agent_gameover(
                    int result,// WIN, LOSS or DRAW
                    int cause  // TRIPLE, ILLEGAL_MOVE, TIMEOUT or FULL_BOARD
                   )
{
  // nothing to do here
}

/*********************************************************//*
   Called after the series of games
*/
void agent_cleanup()
{
  // nothing to do here
}
