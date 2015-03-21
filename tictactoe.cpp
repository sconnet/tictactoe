/*
 * tictactoe (c) 2011 Steve Connet
 *
 * computer vs. computer deathmatch
 *
 */

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define VER_MAJ 1
#define VER_MIN 1
#define BANNER  "TIC-TAC-TOE (c) 2011 Steve Connet"
#define USAGE   "USAGE: tictactoe.exe num_games"

#define DESC    "\
The computer will play against itself for the number of games specified in the\n\
command line argument 'num_games'. After the program has played the specified\n\
number of games, it will print the winner statistics."

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

/*-----------------------------------------------------------------------------
 *  Class Row
 *
 *  Contains 3 pointers that point to three cells in the board
 *-----------------------------------------------------------------------------
 */
class Row
{
public:
    void assign(uint8_t *c1, uint8_t *c2, uint8_t *c3);

    bool check(uint16_t rowSum) const
    {
        return rowSum == (*c[0] + *c[1] + *c[2]);
    }

    void mark(uint8_t glyph);
protected:
    uint8_t *c[3];
};

void Row::assign(uint8_t *c1, uint8_t *c2, uint8_t *c3)
{
    c[0] = c1;  // assign pointers
    c[1] = c2;
    c[2] = c3;
}

void Row::mark(uint8_t glyph)
{
    // find open cell in random order
    uint8_t start_cell = rand() % 2;
    for(int n = 0; n < 3; ++n)
    {
        uint8_t cell = start_cell % 3;
        if(*c[cell] == ' ')
        {
            *c[cell] = glyph;
            break;
        }
        ++start_cell;
    }
}

/*-----------------------------------------------------------------------------
 *  Class Board
 *
 *      R3  R4  R5
 *    +-----------+
 * R0 | 0 | 1 | 2 |   3 horizontal rows
 *    |---+---+---|   3 vertical rows
 * R1 | 3 | 4 | 5 |   2 diagonal rows
 *    |---+---+---|
 * R2 | 6 | 7 | 8 |
 *    +-----------+
 *   R7           R6
 *-----------------------------------------------------------------------------
 */
class Board
{
public:
    Board();
    void print();
    bool check(uint16_t rowSum) const;
    bool move(uint8_t glyph, uint16_t rowSum);
    void clear();

protected:
    static const int MAX_ROWS  = 8; // 3 horizontal, 3 vertical, 2 diagonal
    static const int MAX_CELLS = 9; // 3x3
    Row row[MAX_ROWS];
    uint8_t cell[MAX_CELLS];
};

Board::Board()
{
    clear();

    // assign cells to horizontal rows
    row[0].assign(&cell[0], &cell[1], &cell[2]);
    row[1].assign(&cell[3], &cell[4], &cell[5]);
    row[2].assign(&cell[6], &cell[7], &cell[8]);

    // assign cells to vertical rows
    row[3].assign(&cell[0], &cell[3], &cell[6]);
    row[4].assign(&cell[1], &cell[4], &cell[7]);
    row[5].assign(&cell[2], &cell[5], &cell[8]);

    // assign cells to diagnoal rows
    row[6].assign(&cell[0], &cell[4], &cell[8]);
    row[7].assign(&cell[2], &cell[4], &cell[6]);
}
void Board::clear()
{
    memset(cell, ' ', MAX_CELLS); // clear cells
}

void Board::print()
{
    printf("+-----------+\n");
    printf("| %c | %c | %c |\n", cell[0], cell[1], cell[2]);
    printf("|---+---+---|\n");
    printf("| %c | %c | %c |\n", cell[3], cell[4], cell[5]);
    printf("|---+---+---|\n");
    printf("| %c | %c | %c |\n", cell[6], cell[7], cell[8]);
    printf("+-----------+\n");
}

bool Board::check(uint16_t rowSum) const
{
    for(int n = 0; n < MAX_ROWS; ++n)
    {
        if(row[n].check(rowSum))
        {
            return true;
        }
    }
    return false;
}

bool Board::move(uint8_t glyph, uint16_t rowSum)
{
    for(int n = 0; n < MAX_ROWS; ++n)
    {
        if(row[n].check(rowSum))
        {
            row[n].mark(glyph);
            return true;
        }
    }
    return false;
}

/*-----------------------------------------------------------------------------
 *  Class Player
 *
 * Ascii value of X is 88
 * Ascii value of O is 79
 * Ascii value of _ is 32
 *
 * Values used to identify a row (glyphs in any order):
 *
 * 32 + 32 + 32 = 96    ___
 * 88 + 32 + 32 = 152   X__
 * 79 + 32 + 32 = 143   O__
 * 88 + 88 + 32 = 208   XX_
 * 79 + 79 + 32 = 190   OO_
 * 88 + 88 + 79 = 255   XXO
 * 79 + 79 + 88 = 246   OOX
 *
 *-----------------------------------------------------------------------------
 */
class Player
{
public:
    static const uint8_t X = 'X';
    static const uint8_t O = 'O';
    static const uint8_t B = ' '; // blank

    Player(uint8_t glyph, Board &board);
    bool move();
    bool isWinner() const
    {
        return board.check(win);
    }

protected:
    uint8_t glyph;
    Board &board;
    uint16_t G0;
    uint16_t G1_me;
    uint16_t G1_opp;
    uint16_t G2_me;
    uint16_t G2_opp;
    uint16_t win;
};

Player::Player(uint8_t glyph, Board &board)
    : glyph(glyph),
      board(board)
{
    uint8_t other = (glyph == X) ? O : X;
    G0      = B     + B     + B;
    G1_me   = glyph + B     + B;
    G1_opp  = other + B     + B;
    G2_me   = glyph + glyph + B;
    G2_opp  = other + other + B;
    win     = glyph + glyph + glyph;
}

bool Player::move()
{
    bool ok = false;

    // try to make winning move
    ok = ok || board.move(glyph, G2_me);

    // if cannot, try to block opponent from winning move
    ok = ok || board.move(glyph, G2_opp);

    // if cannot, try to move in row with my existing glyph
    ok = ok || board.move(glyph, G1_me);

    // if cannot, try to move in row with opponents existing glyph
    ok = ok || board.move(glyph, G1_opp);

    // if cannot, move in an open space
    ok = ok || board.move(glyph, G0);

    // board must be full
    return ok;
}

class Game
{
public:
    int play();
protected:
    Board board;
    Player *player1;
    Player *player2;
};

int Game::play()
{
    int winner = -1;

    player1 = new Player(Player::X, board);
    player2 = new Player(Player::O, board);

    while(true)
    {
        if(player1->move() == false)
        {
            winner = 0;
            break; // board full - draw
        }

        if(player1->isWinner() == true)
        {
            winner = 1;
            break;
        }

        if(player2->move() == false)
        {
            winner = 0;
            break; // board full - draw
        }

        if(player2->isWinner() == true)
        {
            winner = 2;
            break;
        }
    }

    delete player2;
    delete player1;
    return winner;
}

int main(int argc, char *argv[])
{
    printf("tictactoe v%d.%02d (c) 2011 Steve Connet\n\n", VER_MAJ, VER_MIN);

    if(argc != 2)
    {
        printf("%s\n\n", USAGE);
        printf("%s\n\n", DESC);
        return EXIT_FAILURE;
    }

    int iterations = atoi(argv[1]);

    srand(time(NULL));

    int results[3] = { 0, 0, 0 }; // draw, Player 1, Player 2
    enum { X = 0, O = 1, DRAW = 2 };

    for(int i = 0; i < iterations; ++i)
    {
        Game game;
        int winner = game.play();
        ++results[winner];
    }

    printf("After %d iterations of the game,\n", iterations);
    printf("Player %c won %d times\n", Player::X, results[1]);
    printf("Player %c won %d times\n", Player::O, results[2]);
    printf("There were %d draws\n", results[0]);

    return EXIT_SUCCESS;
}
