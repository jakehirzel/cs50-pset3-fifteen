
/**
 * fifteen.c
 *
 * Computer Science 50
 * Problem Set 3
 *
 * Implements Game of Fifteen (generalized to d x d).
 *
 * Usage: fifteen d
 *
 * whereby the board's dimensions are to be d x d,
 * where d must be in [DIM_MIN,DIM_MAX]
 *
 * Note that usleep is obsolete, but it offers more granularity than
 * sleep and is simpler to use than nanosleep; `man usleep` for more.
 */
 
#define _XOPEN_SOURCE 500

#include <cs50.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// constants
#define DIM_MIN 3
#define DIM_MAX 9

// board
int board[DIM_MAX][DIM_MAX];

// dimensions
int d;

// keep track of blank space
int blank_coords[2];

// prototypes
void clear(void);
void greet(void);
void init(void);
void draw(void);
bool move(int tile);
bool won(void);

int main(int argc, string argv[])
{
    // ensure proper usage
    if (argc != 2)
    {
        printf("Usage: fifteen d\n");
        return 1;
    }

    // ensure valid dimensions
    d = atoi(argv[1]);
    if (d < DIM_MIN || d > DIM_MAX)
    {
        printf("Board must be between %i x %i and %i x %i, inclusive.\n",
            DIM_MIN, DIM_MIN, DIM_MAX, DIM_MAX);
        return 2;
    }

    // open log
    FILE* file = fopen("log.txt", "w");
    if (file == NULL)
    {
        return 3;
    }

    // greet user with instructions
    greet();

    // initialize the board
    init();

    // set initial state of blank space
    blank_coords[0] = d - 1;
    blank_coords[1] = d - 1;

    // accept moves until game is won
    while (true)
    {
        // clear the screen
        clear();

        // draw the current state of the board
        draw();

        // log the current state of the board (for testing)
        for (int i = 0; i < d; i++)
        {
            for (int j = 0; j < d; j++)
            {
                fprintf(file, "%i", board[i][j]);
                if (j < d - 1)
                {
                    fprintf(file, "|");
                }
            }
            fprintf(file, "\n");
        }
        fflush(file);
        
        // check for win
        if (won())
        {
            printf("ftw!\n");
            break;
        }

        // prompt for move
        printf("Tile to move: ");
        int tile = GetInt();
        
        // quit if user inputs 0 (for testing)
        if (tile == 0)
        {
            break;
        }

        // log move (for testing)
        fprintf(file, "%i\n", tile);
        fflush(file);

        // move if possible, else report illegality
        if (!move(tile))
        {
            printf("\nIllegal move.\n");
            usleep(300000);
        }

        // sleep thread for animation's sake
        usleep(600000);
    }
    
    // close log
    fclose(file);

    // success
    return 0;
}

/**
 * Clears screen using ANSI escape sequences.
 */
void clear(void)
{
    printf("\033[2J");
    printf("\033[%d;%dH", 0, 0);
}

/**
 * Greets player.
 */
void greet(void)
{
    clear();
    printf("WELCOME TO GAME OF FIFTEEN\n");
    usleep(900000);
}

/**
 * Initializes the game's board with tiles numbered 1 through d*d - 1
 * (i.e., fills 2D array with values but does not actually print them).  
 */
void init()
{
    // Fill the board by rows (i) and columns (j) with d^2 numbers (k)
    for (int i = 0, j = 0, k = (d * d - 1); i < d; i++)
    {
        
        // Reset j to index 0 for each new row
        j = 0;
        
        // Fill descending values of k into the j level of the array
        while (j < d && k > 0)
        {

            // Swap 1 & 2 when odd number of tiles (i.e. d is even)
            if (d % 2 == 0 && k == 2)
            {
                board[i][j] = 1;
                board[i][j + 1] = 2;
                j = j + 2;
                k = 0;
            }
            
            // Fill the board in normal fashion
            else
            {
                board[i][j] = k;
                j++;
                k--;
            }
        }
        
        // Handle the special case of k == 0 to allow for eventual underscores
        if (k == 0)
        {
            board[i][j] = 0;
        }
    }
}

/**
 * Prints the board in its current state.
 */
void draw(void)
{
    for (int i = 0; i < d; i++)
    {
        for (int j = 0; j < d; j++)
        {
            // Replace 95 with underscores
            if (board[i][j] == 0)
            {
                printf(" __ ");
            }
            
            // Otherwise print the init() values
            else
            {
                printf(" %2d ", board[i][j]);
            }
        }
        printf("\n\n");
    }
}

/**
 * If tile borders empty space, moves tile and returns true, else
 * returns false. 
 */
bool move(int tile)
{
    for (int i = 0; i < d; i++)
    {
        // Find board coordinates of value of tile
        for (int j = 0; j < d; j++)
        {
            if (board[i][j] == tile)
            {
                // tile and blank in same row?
                if (i == blank_coords[0])
                {
                    // is tile one to the left or right of blank?
                    if (j == blank_coords[1] + 1 || j == blank_coords[1] - 1)
                    {
                        // swap tiles, update blank_coords & return true
                        board[blank_coords[0]][blank_coords[1]] = tile;
                        board[i][j] = 0;
                        blank_coords[0] = i;
                        blank_coords[1] = j;
                        return true;
                    }
                }
                // tile and blank in same column?
                if (j == blank_coords[1])
                {
                    // is the tile one above or below blank?
                    if (i == blank_coords[0] + 1 || i == blank_coords[0] - 1)
                    {
                        board[blank_coords[0]][blank_coords[1]] = tile;
                        board[i][j] = 0;
                        blank_coords[0] = i;
                        blank_coords[1] = j;
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

/**
 * Returns true if game is won (i.e., board is in winning configuration), 
 * else false.
 */
bool won(void)
{

    // Don't bother running this if 1 isn't in the right spot
    if (board[0][0] == 1)
    {
        // Declare counter k, initialize to 1
        int k = 1;
        
        // Iterate up and check for sequential order 
        if (k < d * d - 1)
        {
            for (int i = 0; i < d; i++)
            {
                for (int j = 0; j < d; j++)
                {
                    if (board[i][j] == k)
                    {
                        k++;
                    }
                }
            }
        }
        
        // If the above makes it through all the numbers then:
        if (k == d * d)
        {
            if (board[d - 1][d - 1] == 0)
            {
                return true;
            }
        }
    }

    // Return to the game if nothing above results in true
    return false;
}