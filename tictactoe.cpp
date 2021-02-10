/*
 * tictactoe (c) 2011 Steve Connet
 * computer vs. computer deathmatch
 *
 * Compile:
 * gcc -O2 -o tictactoe tictactoe.cpp -lstdc++
 * or g++ -O2 -o tictactoe tictactoe.cpp
 *
 *  Alt-J    = ∆
 *  Sh-Alt-V = ◊
 *  U+29EB   = ⧫
 *
 *
 *                +------+
 *                | Game |
 *                +------+
 *                  ◊  ⧫
 *                  |  |
 *                  |  |
 *                  |  |
 *        +---------+  +---------+
 *        |                      |
 *        |                      |
 *      2 |                    1 |
 *    +--------+             +-------+
 *    | Player |-------------| Board |
 *    +--------+ 2:1         +-------+
 *                               ⧫
 *                               |
 *                               |
 *                             8 |
 *                            +-----+
 *                            | Row |
 *                            +-----+
 */

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static char const *BANNER = "TIC-TAC-TOE 1.2 (c) 2011-2021 Steve Connet";
static char const *USAGE  = "USAGE: tictactoe.exe num_games";

static char const *DESC   = "\
The computer will play against itself for the number of games specified in\n\
the command line argument 'num_games'. After the program has played the\n\
specified number of games, it will display the results.";

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
        void mark(uint8_t const glyph);

        inline bool check(uint16_t rowSum) const
        {
            return ( rowSum == (*c[0] + *c[1] + *c[2]) );
        }

        inline bool isMatch(uint16_t const rowSum) const
        {
            return ( rowSum == (*c[0] + *c[1] + *c[2]) );
        }

    protected:
        uint8_t *c[3];
};

void Row::assign(uint8_t *c1, uint8_t *c2, uint8_t *c3)
{
    c[0] = c1;
    c[1] = c2;
    c[2] = c3;
}

void Row::mark(uint8_t const glyph)
{
    // find open cell in random order
    uint8_t start_cell = rand() % 2;

    for (size_t n = 0; n < 3; ++n)
    {
        uint8_t idx = start_cell % 3;

        if (' ' == *c[idx])
        {
            *c[idx] = glyph;
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

        inline void clear()
        {
            memset(cell, ' ', MAX_CELLS); // clear cells
        }

        void print() const;
        bool hasWinner(uint16_t const rowSum) const;
        bool findMove(uint8_t const glyph, uint16_t const rowSum);

    protected:
        static const size_t MAX_ROWS  = 8; // 3 horiz, 3 vert, 2 diag
        static const size_t MAX_CELLS = 9; // 3x3

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

void Board::print() const
{
    printf("+-----------+\n");
    printf("| %c | %c | %c |\n", cell[0], cell[1], cell[2]);
    printf("|---+---+---|\n");
    printf("| %c | %c | %c |\n", cell[3], cell[4], cell[5]);
    printf("|---+---+---|\n");
    printf("| %c | %c | %c |\n", cell[6], cell[7], cell[8]);
    printf("+-----------+\n");
}

bool Board::hasWinner(uint16_t const rowSum) const
{
    bool result = false;

    for (size_t n = 0; n < MAX_ROWS; ++n)
    {
        if (row[n].isMatch(rowSum))
        {
            result = true;
            break;
        }
    }

    return result;
}

bool Board::findMove(uint8_t const glyph, uint16_t const rowSum)
{
    bool result = false;

    // select random row to start
    uint8_t start_row = rand() % (MAX_ROWS-1);

    for (size_t n = 0; n < MAX_ROWS; ++n)
    {
        uint8_t idx = start_row % MAX_ROWS;

        if (row[idx].isMatch(rowSum))
        {
            row[idx].mark(glyph);
            result = true;
            break;
        }

        ++start_row;
    }

    return result;
}

/*-----------------------------------------------------------------------------
 *
 * Class Player
 *
 * Ascii value of X is 88
 * Ascii value of O is 79
 * Ascii value of _ is 32
 *
 * Values used to identify a row (glyphs in any order):
 *
 * 32 + 32 + 32 = 96    ___   G0
 * 79 + 32 + 32 = 143   O__   G1
 * 88 + 32 + 32 = 152   X__   G1
 * 79 + 79 + 32 = 190   OO_   G2
 * 88 + 88 + 32 = 208   XX_   G2
 * 79 + 79 + 79 = 237   OOO   win
 * 79 + 79 + 88 = 246   OOX   draw
 * 88 + 88 + 79 = 255   XXO   draw
 * 88 + 88 + 88 = 264   XXX   win
 *
 *-----------------------------------------------------------------------------
 */
class Player
{
    public:
        static const uint8_t X = 'X';
        static const uint8_t O = 'O';
        static const uint8_t B = ' ';

        Player(uint8_t const glyph, Board &board);

        bool move();

        inline bool isWinner() const
        {
            return board.hasWinner(win);
        }


    protected:
        Board &board;

        uint8_t const glyph;

        uint16_t const G2_me;     // 2 glyphs of mine in row
        uint16_t const G2_opp;    // 2 glyphs of them in row
        uint16_t const G1_me;     // 1 glyph of mine in row
        uint16_t const G1_opp;    // 1 glyph of them in row
        uint16_t const win;       // 3 glyphs in row means winner

        static uint16_t const G0   = B+B+B; // 0 glyphs in row (blank row)
        static uint16_t const G1_O = O+B+B; // 1 glyph O in row
        static uint16_t const G1_X = X+B+B; // 1 glyph X in row, no O glyphs
        static uint16_t const G2_O = O+O+B; // 2 glyphs X in row, no O glyphs
        static uint16_t const G2_X = X+X+B; // 2 glyphs O in row, no X glyphs
        static uint16_t const G3_O = O+O+O; // 3 glyphs X in row, winner
        static uint16_t const G3_X = X+X+X; // 3 glyphs O in row, winner
};

Player::Player(uint8_t const the_glyph, Board &the_board) :
    glyph(the_glyph),
    board(the_board),
    win(    (X == glyph) ? G3_X : G3_O ),
    G2_me(  (X == glyph) ? G2_X : G2_O ),
    G1_me(  (X == glyph) ? G1_X : G1_O ),
    G2_opp( (X == glyph) ? G2_O : G2_X ),
    G1_opp( (X == glyph) ? G1_O : G1_X )
{
}

bool Player::move()
{
    bool ok = false;

    // try to make winning move
    ok = ok || board.findMove(glyph, G2_me);

    // if cannot, try to block opponent from winning move
    ok = ok || board.findMove(glyph, G2_opp);

    // if cannot, try to move in row with my existing glyph
    ok = ok || board.findMove(glyph, G1_me);

    // if cannot, try to move in row with opponents existing glyph
    ok = ok || board.findMove(glyph, G1_opp);

    // if cannot, move in an open space
    ok = ok || board.findMove(glyph, G0);

    // board must be full
    return ok;
}


/*-----------------------------------------------------------------------------
 *
 * Class Game
 *
 *-----------------------------------------------------------------------------
 */
class Game
{
    public:
        Game();
        ~Game();

        uint8_t play();

        enum
        {
            X,
            O,
            DRAW,
            NONE
        };

    protected:
        static const size_t NUM_PLAYERS = 2;

        Board board;
        Player *player[NUM_PLAYERS];
        uint8_t last_winner;
};

Game::Game() :
    last_winner(NONE)
{
    assert (2 == NUM_PLAYERS);

    player[X] = new Player(Player::X, board);
    player[O] = new Player(Player::O, board);

    assert(player[X] != NULL);
    assert(player[O] != NULL);
}

Game::~Game()
{
    for (size_t i = 0; i < NUM_PLAYERS; ++i)
    {
        delete player[i];
        player[i] = NULL;
    }
}

uint8_t Game::play()
{
    uint8_t winner = NONE;

    // the winner of the last game goes first
    uint8_t player_to_move = (X == last_winner) ? X : O;

    // clear board from previous game
    board.clear();

    // keep making moves until there is a winner
    while (NONE == winner)
    {
        for (size_t i = 0; i < NUM_PLAYERS; ++i)
        {
            if (false == player[player_to_move]->move())
            {
                winner = DRAW;
                break; // board full - draw
            }

            if (true == player[player_to_move]->isWinner())
            {
                winner = player_to_move;
                break;
            }

            // next players move
            player_to_move = ++player_to_move % DRAW;
        }
    }

    last_winner = winner;
//    board.print();

    return winner;
}

/*-----------------------------------------------------------------------------
 *
 * main entry point
 *
 *-----------------------------------------------------------------------------
 */
int main(int argc, char *argv[])
{
    int result = EXIT_SUCCESS;

    if (argc != 2)
    {
        printf("%s\n\n", BANNER);
        printf("%s\n\n", USAGE);
        printf("%s\n", DESC);

        result = EXIT_FAILURE;
    }
    else
    {
        size_t iterations = atoi(argv[1]);
        size_t result[3] = { 0, 0, 0 }; // Player 1, Player 2, Draw
        size_t percent[3] = { 0, 0, 0 };

        srand(time(NULL));

        Game game;
        for (size_t i = 0; i < iterations; ++i)
        {
            ++result[game.play()]; // game.play() returns 0, 1, or 2
        }

        for (size_t i = 0; i < 3; ++i)
        {
            percent[i] = 100 * (result[i] / static_cast<float>(iterations));
        }

        printf(" Games: %lu\n", iterations);
        printf("X wins: %lu (%lu%%)\n", result[Game::X], percent[Game::X]);
        printf("O wins: %lu (%lu%%)\n", result[Game::O], percent[Game::O]);
        printf(" Draws: %lu (%lu%%)\n", result[Game::DRAW], percent[Game::DRAW]);
    }

    return result;
}
