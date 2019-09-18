/*********************************************************
 *  agent.c
 *  Nine-Board Tic-Tac-Toe Agent
 *  COMP3411/9414/9814 Artificial Intelligence
 *  Vincent Woo Kim
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
int move[MAX_MOVE+1];
int player;
int m;
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
    if (this_move == board_num)
    {
      this_move = (this_move + 1) % 9;
    }
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
    if (this_move == board_num)
    {
      this_move = (this_move + 1) % 9;
    }
  } while( board[prev_move][this_move] != EMPTY );
  move[m] = this_move;
  board[move[m-1]][this_move] = player;
  return( this_move );
}

/*********************************************************//*
   Apply alpha-beta search algorithm to choose best available move and return it
*/
int agent_next_move( int prev_move )
{
  int this_move;
  m++;
  move[m] = prev_move;
  board[move[m-1]][move[m]] = !player;
  m++;

  int alpha = -100000;
  int beta = 100000;

  int max_depth = 1;
  if(m < 25)
    max_depth = 7;
  else if (m < 60)
    max_depth = 9;
  else
    max_depth = 11;

  int maxScore = -100000;
  for(int p = 1; p < 10; p++)
  {
    if(board[prev_move][p] == EMPTY)
    {
      board[prev_move][p] = player;
      int moveScore = alpha_beta_search(board, prev_move, p, 0, max_depth, alpha, beta);
      board[prev_move][p] = EMPTY;
      if(maxScore < moveScore)
      {
        maxScore = moveScore;
        this_move = p;
      }
    }
  }
  move[m] = this_move;
  board[move[m-1]][this_move] = player;
  return( this_move );
}

/*********************************************************//*
    Alpha beta search algorithm to recursively loop through potential moves until maximum depth is reached or terminal state is reached.
*/
int alpha_beta_search(int myBoard[10][10], int currentBoard, int nextBoard, int myTurn, int depth, int alpha, int beta)
{
  int score = 0;
  for(int i = 1; i < 10; i++)
  {
    int value = board_score(myBoard, i);
    if (value == 10000 || value == -10000)
      return value;

    score += value;
  }

  if(no_more_move_board(myBoard, currentBoard) == 1)
    return 0;
  if(depth == 0)
    return score;

  if(myTurn == 1)
  {
      for(int p = 1; p < 10; p++)
      {
        if(myBoard[nextBoard][p] == EMPTY)
        {
          myBoard[nextBoard][p] = player;
          alpha = max(alpha, alpha_beta_search(myBoard, nextBoard, p, 0, depth-1, alpha, beta));
          myBoard[nextBoard][p] = EMPTY;

          if(alpha >= beta)
            return alpha;
        }
      }
      return alpha;
  }
  else
  {
    for(int p = 1; p < 10; p++)
    {
      if(myBoard[nextBoard][p] == EMPTY)
      {
        myBoard[nextBoard][p] = !player;
        beta = min(beta, alpha_beta_search(myBoard, nextBoard, p, 1, depth-1, alpha, beta));
        myBoard[nextBoard][p] = EMPTY;

        if(alpha >= beta)
          return beta;
      }
    }
    return beta;
  }
}

/*********************************************************//*
    Check if board is full by going over the board checking if there is avail spot.
*/
int no_more_move_board(int myBoard[10][10], int numBoard)
{
  for(int p = 1; p < 10; p++)
  {
    if(myBoard[numBoard][p] == EMPTY)
      return 0;
  }
  return 1;
}

/*********************************************************//*
    Heuristic function of the algorithm. Searches for terminal state and intermediate scores of the board.
    Win/Loss/Draw = 10000/-10000/0
    Attack Two on row, col, diag each gets +-10 (i.e. XX*, X*X, *XX)
    Attack One on row, col, diag each gets +-2 (i.e. X**, *X*, **X)
    Block Two on row, col, diag each gets +-5 (i.e. XOO, OXO, OOX)
    Zero sum are made to variable score.
*/
int board_score(int myBoard[10][10], int currentBoard)
{
  for(int iter = 1; iter < 8; iter += 3)
  {
    if(myBoard[currentBoard][iter] == myBoard[currentBoard][iter+1] && myBoard[currentBoard][iter+1] == myBoard[currentBoard][iter+2])
    {
      if(myBoard[currentBoard][iter] == player)
        return 10000;
      else if (myBoard[currentBoard][iter] == !player)
        return -10000;
    }
  }
  for(int iter = 1; iter < 4; iter++)
  {
    if(myBoard[currentBoard][iter] == myBoard[currentBoard][iter+3] && myBoard[currentBoard][iter+3] == myBoard[currentBoard][iter+6])
    {
      if(myBoard[currentBoard][iter] == player)
        return 10000;
      else if(myBoard[currentBoard][iter] == !player)
        return -10000;
    }
  }
  if(myBoard[currentBoard][1] == myBoard[currentBoard][5] && myBoard[currentBoard][5] == myBoard[currentBoard][9])
  {
    if(myBoard[currentBoard][1] == player)
      return 10000;
    else if(myBoard[currentBoard][1] == !player)
      return -10000;
  }
  if(myBoard[currentBoard][3] == myBoard[currentBoard][5] && myBoard[currentBoard][5] == myBoard[currentBoard][7])
  {
    if(myBoard[currentBoard][3] == player)
      return 10000;
    else if(myBoard[currentBoard][3] == !player)
      return -10000;
  }

  int attackTwo = 10;
  int attackOne = 2;
  int blockTwo = 5;
  int score = 0;
  for(int i = 1; i < 9; i += 3)
  {
    if(myBoard[currentBoard][i] != EMPTY && myBoard[currentBoard][i+1] != EMPTY && myBoard[currentBoard][i+2] != EMPTY)
    {
      if(myBoard[currentBoard][i] == myBoard[currentBoard][i+1] && myBoard[currentBoard][i+1] != myBoard[currentBoard][i+2])
      {
        if(myBoard[currentBoard][i + 2] == player)
          score += blockTwo;
        else if (myBoard[currentBoard][i + 2] == !player)
          score -= blockTwo;
      }
      else if (myBoard[currentBoard][i] != myBoard[currentBoard][i+1] && myBoard[currentBoard][i+1] == myBoard[currentBoard][i+2])
      {
        if(myBoard[currentBoard][i] == player)
          score += blockTwo;
        else if (myBoard[currentBoard][i] == !player)
          score -= blockTwo;
      }
      else if (myBoard[currentBoard][i] == myBoard[currentBoard][i+2] && myBoard[currentBoard][i+2] != myBoard[currentBoard][i+1])
      {
        if(myBoard[currentBoard][i + 1] == player)
          score += blockTwo;
        else if (myBoard[currentBoard][i + 1] == !player)
          score -= blockTwo;
      }
    }
    else if(myBoard[currentBoard][i] != EMPTY && myBoard[currentBoard][i+1] != EMPTY && myBoard[currentBoard][i+2] == EMPTY)
    {
      if(myBoard[currentBoard][i] == myBoard[currentBoard][i+1])
      {
        if(myBoard[currentBoard][i] == player)
          score += attackTwo;
        else if (myBoard[currentBoard][i] == !player)
          score -= attackTwo;
      }
    }
    else if(myBoard[currentBoard][i] != EMPTY && myBoard[currentBoard][i+1] == EMPTY && myBoard[currentBoard][i+2] != EMPTY)
    {
      if(myBoard[currentBoard][i] == myBoard[currentBoard][i+2])
      {
        if(myBoard[currentBoard][i] == player)
          score += attackTwo;
        else if (myBoard[currentBoard][i] == !player)
          score -= attackTwo;
      }
    }
    else if(myBoard[currentBoard][i] == EMPTY && myBoard[currentBoard][i+1] != EMPTY && myBoard[currentBoard][i+2] != EMPTY)
    {
      if(myBoard[currentBoard][i+1] == myBoard[currentBoard][i+2])
      {
        if(myBoard[currentBoard][i+1] == player)
          score += attackTwo;
        else if (myBoard[currentBoard][i+1] == !player)
          score -= attackTwo;
      }
    }
    else if(myBoard[currentBoard][i] != EMPTY && myBoard[currentBoard][i+1] == EMPTY && myBoard[currentBoard][i+2] == EMPTY)
    {
      if(myBoard[currentBoard][i] == player)
        score += attackOne;
      else if (myBoard[currentBoard][i] == !player)
        score -= attackOne;
    }
    else if(myBoard[currentBoard][i] == EMPTY && myBoard[currentBoard][i+1] != EMPTY && myBoard[currentBoard][i+2] == EMPTY)
    {
      if(myBoard[currentBoard][i+1] == player)
        score += attackOne;
      else if (myBoard[currentBoard][i+1] == !player)
        score -= attackOne;
    }
    else if(myBoard[currentBoard][i] == EMPTY && myBoard[currentBoard][i+1] == EMPTY && myBoard[currentBoard][i+2] != EMPTY)
    {
      if(myBoard[currentBoard][i+2] == player)
        score += attackOne;
      else if (myBoard[currentBoard][i+2] == !player)
        score -= attackOne;
    }
  }
  for(int i = 1; i < 4; i++)
  {
    if(myBoard[currentBoard][i] != EMPTY && myBoard[currentBoard][i+3] != EMPTY && myBoard[currentBoard][i+6] != EMPTY)
    {
      if(myBoard[currentBoard][i] == myBoard[currentBoard][i+3] && myBoard[currentBoard][i+3] != myBoard[currentBoard][i+6])
      {
        if(myBoard[currentBoard][i + 6] == player)
          score += blockTwo;
        else if (myBoard[currentBoard][i + 6] == !player)
          score -= blockTwo;
      }
      else if (myBoard[currentBoard][i] != myBoard[currentBoard][i+3] && myBoard[currentBoard][i+3] == myBoard[currentBoard][i+6])
      {
        if(myBoard[currentBoard][i] == player)
          score += blockTwo;
        else if (myBoard[currentBoard][i] == !player)
          score -= blockTwo;
      }
      else if (myBoard[currentBoard][i] == myBoard[currentBoard][i+6] && myBoard[currentBoard][i+6] != myBoard[currentBoard][i+3])
      {
        if(myBoard[currentBoard][i + 3] == player)
          score += blockTwo;
        else if (myBoard[currentBoard][i + 3] == !player)
          score -= blockTwo;
      }
    }
    else if(myBoard[currentBoard][i] != EMPTY && myBoard[currentBoard][i+3] != EMPTY && myBoard[currentBoard][i+6] == EMPTY)
    {
      if(myBoard[currentBoard][i] == myBoard[currentBoard][i+3])
      {
        if(myBoard[currentBoard][i] == player)
          score += attackTwo;
        else if (myBoard[currentBoard][i] == !player)
          score -= attackTwo;
      }
    }
    else if(myBoard[currentBoard][i] != EMPTY && myBoard[currentBoard][i+3] == EMPTY && myBoard[currentBoard][i+6] != EMPTY)
    {
      if(myBoard[currentBoard][i] == myBoard[currentBoard][i+6])
      {
        if(myBoard[currentBoard][i] == player)
          score += attackTwo;
        else if (myBoard[currentBoard][i] == !player)
          score -= attackTwo;
      }
    }
    else if(myBoard[currentBoard][i] == EMPTY && myBoard[currentBoard][i+3] != EMPTY && myBoard[currentBoard][i+6] != EMPTY)
    {
      if(myBoard[currentBoard][i+3] == myBoard[currentBoard][i+6])
      {
        if(myBoard[currentBoard][i+3] == player)
          score += attackTwo;
        else if (myBoard[currentBoard][i+3] == !player)
          score -= attackTwo;
      }
    }
    else if(myBoard[currentBoard][i] != EMPTY && myBoard[currentBoard][i+3] == EMPTY && myBoard[currentBoard][i+6] == EMPTY)
    {
      if(myBoard[currentBoard][i] == player)
        score += attackOne;
      else if (myBoard[currentBoard][i] == !player)
        score -= attackOne;
    }
    else if(myBoard[currentBoard][i] == EMPTY && myBoard[currentBoard][i+3] != EMPTY && myBoard[currentBoard][i+6] == EMPTY)
    {
      if(myBoard[currentBoard][i+3] == player)
        score += attackOne;
      else if (myBoard[currentBoard][i+3] == !player)
        score -= attackOne;
    }
    else if(myBoard[currentBoard][i] == EMPTY && myBoard[currentBoard][i+3] == EMPTY && myBoard[currentBoard][i+6] != EMPTY)
    {
      if(myBoard[currentBoard][i+6] == player)
        score += attackOne;
      else if (myBoard[currentBoard][i+6] == !player)
        score -= attackOne;
    }
  }
  if(myBoard[currentBoard][1] != EMPTY && myBoard[currentBoard][5] != EMPTY && myBoard[currentBoard][9] != EMPTY)
  {
    if(myBoard[currentBoard][1] == myBoard[currentBoard][5] && myBoard[currentBoard][5] != myBoard[currentBoard][9])
    {
      if(myBoard[currentBoard][9] == player)
        score += blockTwo;
      else if (myBoard[currentBoard][9] == !player)
        score -= blockTwo;
    }
    else if (myBoard[currentBoard][1] != myBoard[currentBoard][5] && myBoard[currentBoard][5] == myBoard[currentBoard][9])
    {
      if(myBoard[currentBoard][1] == player)
        score += blockTwo;
      else if (myBoard[currentBoard][1] == !player)
        score -= blockTwo;
    }
    else if (myBoard[currentBoard][1] == myBoard[currentBoard][9] && myBoard[currentBoard][9] != myBoard[currentBoard][5])
    {
      if(myBoard[currentBoard][5] == player)
        score += blockTwo;
      else if (myBoard[currentBoard][5] == !player)
        score -= blockTwo;
    }
  }
  else if(myBoard[currentBoard][1] != EMPTY && myBoard[currentBoard][5] != EMPTY && myBoard[currentBoard][9] == EMPTY)
  {
    if(myBoard[currentBoard][1] == myBoard[currentBoard][5])
    {
      if(myBoard[currentBoard][1] == player)
        score += attackTwo;
      else if (myBoard[currentBoard][1] == !player)
        score -= attackTwo;
    }
  }
  else if(myBoard[currentBoard][1] != EMPTY && myBoard[currentBoard][5] == EMPTY && myBoard[currentBoard][9] != EMPTY)
  {
    if(myBoard[currentBoard][1] == myBoard[currentBoard][9])
    {
      if(myBoard[currentBoard][1] == player)
        score += attackTwo;
      else if (myBoard[currentBoard][1] == !player)
        score -= attackTwo;
    }
  }
  else if(myBoard[currentBoard][1] == EMPTY && myBoard[currentBoard][5] != EMPTY && myBoard[currentBoard][9] != EMPTY)
  {
    if(myBoard[currentBoard][5] == myBoard[currentBoard][9])
    {
      if(myBoard[currentBoard][5] == player)
        score += attackTwo;
      else if (myBoard[currentBoard][5] == !player)
        score -= attackTwo;
    }
  }
  else if(myBoard[currentBoard][1] != EMPTY && myBoard[currentBoard][5] == EMPTY && myBoard[currentBoard][9] == EMPTY)
  {
    if(myBoard[currentBoard][1] == player)
      score += attackOne;
    else if (myBoard[currentBoard][1] == !player)
      score -= attackOne;
  }
  else if(myBoard[currentBoard][1] == EMPTY && myBoard[currentBoard][5] != EMPTY && myBoard[currentBoard][9] == EMPTY)
  {
    if(myBoard[currentBoard][5] == player)
      score += attackOne;
    else if (myBoard[currentBoard][5] == !player)
      score -= attackOne;
  }
  else if(myBoard[currentBoard][1] == EMPTY && myBoard[currentBoard][5] == EMPTY && myBoard[currentBoard][9] != EMPTY)
  {
    if(myBoard[currentBoard][9] == player)
      score += attackOne;
    else if (myBoard[currentBoard][9] == !player)
      score -= attackOne;
  }

  if(myBoard[currentBoard][3] != EMPTY && myBoard[currentBoard][5] != EMPTY && myBoard[currentBoard][7] != EMPTY)
  {
    if(myBoard[currentBoard][3] == myBoard[currentBoard][5] && myBoard[currentBoard][5] != myBoard[currentBoard][7])
    {
      if(myBoard[currentBoard][7] == player)
        score += blockTwo;
      else if (myBoard[currentBoard][7] == !player)
        score -= blockTwo;
    }
    else if (myBoard[currentBoard][3] != myBoard[currentBoard][5] && myBoard[currentBoard][5] == myBoard[currentBoard][7])
    {
      if(myBoard[currentBoard][3] == player)
        score += blockTwo;
      else if (myBoard[currentBoard][3] == !player)
        score -= blockTwo;
    }
    else if (myBoard[currentBoard][3] == myBoard[currentBoard][7] && myBoard[currentBoard][7] != myBoard[currentBoard][5])
    {
      if(myBoard[currentBoard][5] == player)
        score += blockTwo;
      else if (myBoard[currentBoard][5] == !player)
        score -= blockTwo;
    }
  }
  else if(myBoard[currentBoard][3] != EMPTY && myBoard[currentBoard][5] != EMPTY && myBoard[currentBoard][7] == EMPTY)
  {
    if(myBoard[currentBoard][3] == myBoard[currentBoard][5])
    {
      if(myBoard[currentBoard][3] == player)
        score += attackTwo;
      else if (myBoard[currentBoard][3] == !player)
        score -= attackTwo;
    }
  }
  else if(myBoard[currentBoard][3] != EMPTY && myBoard[currentBoard][5] == EMPTY && myBoard[currentBoard][7] != EMPTY)
  {
    if(myBoard[currentBoard][3] == myBoard[currentBoard][7])
    {
      if(myBoard[currentBoard][3] == player)
        score += attackTwo;
      else if (myBoard[currentBoard][3] == !player)
        score -= attackTwo;
    }
  }
  else if(myBoard[currentBoard][3] == EMPTY && myBoard[currentBoard][5] != EMPTY && myBoard[currentBoard][7] != EMPTY)
  {
    if(myBoard[currentBoard][5] == myBoard[currentBoard][8])
    {
      if(myBoard[currentBoard][5] == player)
        score += attackTwo;
      else if (myBoard[currentBoard][5] == !player)
        score -= attackTwo;
    }
  }
  else if(myBoard[currentBoard][3] != EMPTY && myBoard[currentBoard][5] == EMPTY && myBoard[currentBoard][7] == EMPTY)
  {
    if(myBoard[currentBoard][3] == player)
      score += attackOne;
    else if (myBoard[currentBoard][3] == !player)
      score -= attackOne;
  }
  else if(myBoard[currentBoard][3] == EMPTY && myBoard[currentBoard][5] != EMPTY && myBoard[currentBoard][7] == EMPTY)
  {
    if(myBoard[currentBoard][5] == player)
      score += attackOne;
    else if (myBoard[currentBoard][5] == !player)
      score -= attackOne;
  }
  else if(myBoard[currentBoard][3] == EMPTY && myBoard[currentBoard][5] == EMPTY && myBoard[currentBoard][7] != EMPTY)
  {
    if(myBoard[currentBoard][7] == player)
      score += attackOne;
    else if (myBoard[currentBoard][7] == !player)
      score -= attackOne;
  }

  return score;
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
