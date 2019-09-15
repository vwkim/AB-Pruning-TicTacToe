/*********************************************************
 *  agent.c
 *  Nine-Board Tic-Tac-Toe Agent
 *  COMP3411/9414/9814 Artificial Intelligence
 *  Alan Blair, CSE, UNSW
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

int **GetBOI(int myBoard[10][10], int board_num);
int Minimax(int myBoard[10][10], int numBoard, int myTurn, int depth);
int AlphaBetaSearch(int myBoard[10][10], int currentBoard, int nextBoard, int myTurn, int depth, int alpha, int beta);
int no_more_move_board(int myBoard[10][10], int prev_move);
int boardscore(int myBoard[10][10], int currentBoard);
int intermediateScore(int myBoard[10][10], int currentBoard, int move, int myTurn);
 //  called at the end of each game
void agent_gameover( int result, int cause );

 //  called at the end of the series of games
void agent_cleanup();
