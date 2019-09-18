/*********************************************************
 *  agent.h
 *  Nine-Board Tic-Tac-Toe Agent
 *  COMP9414 Artificial Intelligence
 *  Vincent Woo Kim
 */
extern int   port;
extern char *host;

 //  parse command-line arguments
void agent_parse_args( int argc, char *argv[] );

 //  called at the beginning of a series of games
void agent_init();

 //  called at the beginning of each game
void agent_start( int this_player );

int  agent_second_move(int board_num, int prev_move );

int  agent_third_move(int board_num,int first_move,int prev_move);

int  agent_next_move( int prev_move );

void agent_last_move( int prev_move );
//  Alpha beta prunning search algorithm
int  alpha_beta_search(int myBoard[10][10], int currentBoard, int nextBoard, int myTurn, int depth, int alpha, int beta);
//  Check if the board is full
int  no_more_move_board(int myBoard[10][10], int prev_move);
//  Heuristic function of the terminal state of the game.
int  board_score(int myBoard[10][10], int currentBoard);
 //  called at the end of each game
void agent_gameover( int result, int cause );
 //  called at the end of the series of games
void agent_cleanup();
