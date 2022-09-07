/*  TETRIS:
    Within the grid on the screen, blocks of different shapes & colours (called tetrominos) will fall from the top of the grid to the bottom of grid.
    Once the tetromino has a block directly underneath it, it will stop moving and a new user-controlled tetromino will spawn at the top of the grid
    The player can move the tetrominos left, right & down-faster.
    The player must get a full row of tetromino blocks, and then this row will be deleted and everything above it shifted downwards.
    The player tries to play for as long as possible, before the stack of blocks is too high and overflowing the top of grid

    Author: Nicholas Malloura
    Date written: 28/04/2022 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <stdbool.h>
#include <conio.h>

#define FIELD_WIDTH 12
#define FIELD_HEIGHT 18
#define SCREEN_WIDTH 35
#define SCREEN_HEIGHT 22
#define FONT_SIZE 34

#define CLOCK_SPEED 500



typedef struct{
    int currentPiece;               // Index of user-controlled tetromino
    int nextPiece;
    int currentRotation;            // Rotation of this tetromino (0=0deg, 1=45deg, 2=90deg, 3=270deg)
    int currentX;                   // x & y co-ord of current tetromino
    int currentY;
}Player;



//Initialises variables for the player.
void player_init(Player* myplayer){
    myplayer->currentPiece = myplayer->nextPiece;   // Sets the current-piece as the next-piece
    myplayer->nextPiece = rand() % 7;               // Sets the new next-piece as a random tetromino out of the 7 tetrominos
    myplayer->currentRotation = 0;                  // Rotation of current tetromino
    myplayer->currentX = FIELD_WIDTH / 2;           // Starting pos: vertically at top & horizontally in middle
    myplayer->currentY = 0;
}

// Initialises the field (playable grid)
void field_init(unsigned char* pField){
    for(int x=0; x < FIELD_WIDTH; x++){                 // Sets playing field to '0', unless it is on the left, right or bottom boundary then set to '9'
        for(int y=0; y < FIELD_HEIGHT; y++){            // '0' = empty space that a tetromino can fill, '9' is a wall that the tetromino can't move into.

            if(x==0 || x==(FIELD_WIDTH-1) || y==(FIELD_HEIGHT-1)){
                pField[y*FIELD_WIDTH + x] = 9;
            }else{
                pField[y*FIELD_WIDTH + x] = 0;
    }}}
}

// For rotating the player's tetromino. Given a coord, returns the resulting coord after the rotation
int rotate(int p_x, int p_y, int r){

    switch(r % 4){
        case 0: return p_y * 4 + p_x;           // 0 degrees
        case 1: return 12 + p_y - (p_x * 4);    // 90 degrees
        case 2: return 15 - (p_y * 4) - p_x;    // 180 degrees
        case 3: return 3 - p_y + (p_x * 4);     // 270 degrees
        default: return 0;                      // Error, should not happen
}}

// Check for collision (before rotating and if block below or left/right of tetromino are free to move into)
bool collision_chec(char tetrominos[7][17], unsigned char pField[], int indxTetromino, int rotation, int pos_x, int pos_y){

    for(int px=0; px < 4; px++){                    // Each tetromino is a 4x4 grid
        for(int py=0; py < 4; py++){                // Iterating through each block in tetromino

            int pi = rotate(px, py, rotation);                      // Get index of where for-loop is in tetromino
            int fi = (pos_y + py) * FIELD_WIDTH + (pos_x + px);     // Get index of where tetromino is in Field

            if( pos_x + px >= 0 && pos_x + px < FIELD_WIDTH && pos_y + py >= 0 && pos_y + py < FIELD_HEIGHT && tetrominos[indxTetromino][pi] == 'X' && pField[fi] != 0 ){
                return false;   // Collision
            }
    }}

    return true;    // No collision
}

// Changes colour of text (to represent blocks & border)
void printf_colour(char txt, int colour, HANDLE hConsole){

    SetConsoleTextAttribute(hConsole, colour);      // Sets text colour to colour-argument
    printf("%c", txt);
    SetConsoleTextAttribute(hConsole, 15);          // Sets text colour back to white (default)
}

// Copy the field/grid[] (inc the in-place tetrominos) onto the screen[]
void draw_field(char screen[], unsigned char pField[], char character_field[]){
    for(int x=0; x < FIELD_WIDTH; x++){
        for(int y=0; y < FIELD_HEIGHT; y++){
            screen[ (y+2)*SCREEN_WIDTH +x +2] = character_field[pField[ y*FIELD_WIDTH +x ]];
}}}

// Copy the player's tetromino onto the screen[]
void draw_player(char tetrominos[7][17], char screen[], Player player1){
    for(int px=0; px < 4; px++){
        for(int py=0; py < 4; py++){
            if( tetrominos[player1.currentPiece][rotate(px, py, player1.currentRotation)] != '.')
					screen[(player1.currentY + py + 2) * SCREEN_WIDTH + (player1.currentX + px + 2)] = player1.currentPiece + 65;

            // '+65' to convert to ACSCI (E.g. 0->A, 1->B)
}}}

// Copy the 'next piece display' on right side of screen, onto the screen[]
void draw_next_piece(char tetrominos[7][17], char screen[], int tetromino_indx){
    for(int px=0; px < 4; px++){
        for(int py=0; py < 4; py++){
            if( tetrominos[tetromino_indx][rotate(px, py, 0)] != '.'){           //'0' as no rotation
                screen[ (7+py)*SCREEN_WIDTH + (FIELD_WIDTH +6 +px)] = tetromino_indx + 65;
            }else{
                screen[ (7+py)*SCREEN_WIDTH + (FIELD_WIDTH +6 +px)] = ' ';
            }
}}}

// Start Screen (Displays instructions & waits for player to press any key before starting)
void start_screen(){

    system("cls");
    printf("\n\tTETRIS\n\nControls:\n'Z' - Rotate clockwise\n'Arrow keys' - Move left, right and down\n\n\nPress any key to start...");
    getch();
}

// Prints/draws the whole display
void display(HANDLE hConsole, char screen[], unsigned char pField[], char character_field[], char tetrominos[7][17], Player player1, int score){
    draw_field(screen, pField, character_field);
    draw_player(tetrominos, screen, player1);
    draw_next_piece(tetrominos, screen, player1.nextPiece);

    system("cls");                                                              // Clears console

    for(int i =0; i<SCREEN_HEIGHT*SCREEN_WIDTH; i++){                           // Prints 'screen' array to console

        if(i == 2*SCREEN_WIDTH + FIELD_WIDTH +6){
            printf("SCORE: %5d ", score);                                       // Prints SCORE
            i+=12;

        }else if(i == 5*SCREEN_WIDTH + FIELD_WIDTH +6){                         // Prints Next-piece text
            printf("NEXT BLOCK:");
            i+=10;

        }else{
            switch (screen[i]){
                case '#':
                    printf_colour('+', 208, hConsole);                          // Walls of playable grid
                    break;
                case '=':
                    printf_colour('=', 7, hConsole);                            // Completed row
                    break;
                case 'A':
                    printf_colour(' ', 16, hConsole);                           // 'A'-'G': for the different tetrominos
                    break;
                case 'B':
                    printf_colour(' ', 32, hConsole);
                    break;
                case 'C':
                    printf_colour(' ', 48, hConsole);
                    break;
                case 'D':
                    printf_colour(' ', 64, hConsole);
                    break;
                case 'E':
                    printf_colour(' ', 80, hConsole);
                    break;
                case 'F':
                    printf_colour(' ', 96, hConsole);
                    break;
                case 'G':
                    printf_colour(' ', 128, hConsole);
                    break;

                default:
                    printf_colour(screen[i], 7, hConsole);                      // Any text in default colour
                    break;
}}}}


int main(){

    HANDLE hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);

    COORD coord_screen = {SCREEN_WIDTH, SCREEN_HEIGHT};
    SMALL_RECT windowSize = {0 , 0 ,  SCREEN_WIDTH-1, SCREEN_HEIGHT-1};
    CONSOLE_CURSOR_INFO cursorInfo = {.dwSize=100, .bVisible=FALSE};

    COORD coord_font = {FONT_SIZE, FONT_SIZE};
    CONSOLE_FONT_INFOEX font = { sizeof(CONSOLE_FONT_INFOEX) };
    font.dwFontSize = coord_font;

    SetConsoleWindowInfo(hConsoleOut, TRUE, &windowSize);               // Sets console window size
    SetConsoleScreenBufferSize(hConsoleOut, coord_screen);              // Sets console buffer size
    SetConsoleCursorInfo(hConsoleOut, &cursorInfo);                     // Hides cursor in console
    SetCurrentConsoleFontEx(hConsoleOut, 0, &font);



    char tetrominos[7][17];                             // Stores all the different tetromino block options
    char character_field[] = " ABCDEFG=#";              // ' ' = Empty cell, 'A-G' = Tetromino blocks, '=' = Line for matching, '#' = Border wall of grid


    char screen[SCREEN_HEIGHT * SCREEN_WIDTH];          // Contains whole screen
    for(int i=0; i < SCREEN_HEIGHT*SCREEN_WIDTH; i++){screen[i] = ' ';}

    unsigned char pField[FIELD_HEIGHT * FIELD_WIDTH];

    strcpy(tetrominos[0], "..X...X...X...X.");          // Filling out tetrominos[] with the different tetrominos (these will be 4x4 grids, but stored as 1D)
    strcpy(tetrominos[1], "..X..XX...X.....");
	strcpy(tetrominos[2], ".....XX..XX.....");
	strcpy(tetrominos[3], "..X..XX..X......");
	strcpy(tetrominos[4], ".X...XX...X.....");
	strcpy(tetrominos[5], ".X...X...XX.....");
	strcpy(tetrominos[6], "..X...X..XX.....");


    /// VARIABLES
    int score;                                  // Player's score for current match
    int high_score;                             // Player's highest score for all matches
    int speedCounter;                           // How much faster speed is running at
    int keyboard_input;
    int deletedRows[FIELD_HEIGHT];              // Rows to be deleted this iteration. For each row: 1=delete, 0=keep row
    bool gameOver;
    bool fullRow;                               // If player gets a full row of blocks
    char retry_response;                        // After game over, response to playing another round
    Player player1;


    do{

        /// INITIALISING VARIABILES AT START OF EACH MATCH
        gameOver = false;
        score = 0;
        speedCounter = 0;

        player1.nextPiece = rand() % 7;          //Sets current piece (Becomes currentPiece in player_init()
        player_init(&player1);                   //Initialising player1:

        field_init(pField);
        for(int i=0; i<FIELD_HEIGHT; i++) {deletedRows[i]=0;}        // Set rows to delete to 0

        start_screen();



        /// GAME LOOP: ============================================================================================================

        while(!gameOver){


            // CLOCK TIMING: ======================================================================================================

            Sleep(CLOCK_SPEED - speedCounter*10);           // Game speed will increase over time

            // GAME LOGIC: ========================================================================================================

            if(collision_chec(tetrominos, pField, player1.currentPiece, player1.currentRotation, player1.currentX, player1.currentY+1)){
                player1.currentY++;             // If space below, move player down every tick

            }else{
                // Lock current piece in field, if there is no free space below player
                for(int px=0; px < 4; px++){
                    for(int py=0; py < 4; py++){
                        if( tetrominos[player1.currentPiece][rotate(px, py, player1.currentRotation)] != '.'){
                            pField[(player1.currentY + py) * FIELD_WIDTH + (player1.currentX + px)] = player1.currentPiece + 1;
                }}}

                // Increase game speed / difficulty
                if(speedCounter < 50){
                    speedCounter++;
                }


                // Check if any rows are fully filled (if so, remove that whole row of blocks)
                for(int py=0; py<FIELD_HEIGHT - player1.currentY; py++){                              //Only need to check the rows that the player was at, and below
                    if(player1.currentY + py < FIELD_HEIGHT - 1){

                        fullRow = true;
                        for(int px=1; px < FIELD_WIDTH-1; px++){
                            fullRow &= (pField[(player1.currentY + py) * FIELD_WIDTH + px]) != 0;       // If any blank spaces on the row, then fullRow=false
                        }

                        if(fullRow){                                     // Removes this row
                            deletedRows[player1.currentY + py] = 1;

                            for(int px=1; px < FIELD_WIDTH-1; px++){
                                pField[(player1.currentY + py) * FIELD_WIDTH + px] = 8;          // character_field[8] = '='
                        }}
                }}


                // Choose next piece and restart player back at top of grid
                player_init(&player1);

                // If restarted piece (at top) is in collision, then game over
                gameOver = !collision_chec(tetrominos, pField, player1.currentPiece, player1.currentRotation, player1.currentX, player1.currentY);
            }

            // USER INPUT: =========================================================================================================
            while(kbhit()){
                keyboard_input = getch();

                if(keyboard_input == 75 && collision_chec(tetrominos, pField, player1.currentPiece, player1.currentRotation, player1.currentX-1, player1.currentY)){
                    player1.currentX--;             // LEFT key pressed && No collision -> tetromino moves left
                }else if(keyboard_input == 77 && collision_chec(tetrominos, pField, player1.currentPiece, player1.currentRotation, player1.currentX+1, player1.currentY)){
                    player1.currentX++;             // RIGHT key pressed && No collision -> tetromino moves right
                }else if(keyboard_input == 80 && collision_chec(tetrominos, pField, player1.currentPiece, player1.currentRotation, player1.currentX, player1.currentY+1)){
                        player1.currentY++;         // DOWN key pressed && No collision -> speed
                }else if(keyboard_input == 122 && collision_chec(tetrominos, pField, player1.currentPiece, player1.currentRotation+1, player1.currentX, player1.currentY+1)){
                        player1.currentRotation++;   // Z key pressed && No collision -> rotation by 90deg clockwise
            }}

            FlushConsoleInputBuffer(hConsoleIn);        // Clears input buffer, otherwise if user presses the left key 5times in a row, then the tetromino will move right over 5 ticks


            // MAKE & DISPLAY OUTPUT: ================================================================================================

            display(hConsoleOut, screen, pField, character_field, tetrominos, player1, score);      // Prints display

            for(int i=0; i<FIELD_HEIGHT; i++){                          // Loop through each row in grid

                if(deletedRows[i] == 1){                                // Check if this row should be deleted
                    score += 25;                                        // Increased score for each line removed by player

                    for(int px=1; px < FIELD_WIDTH -1; px++){
                        for(int py=i; py>0; py--){
                            pField[py*FIELD_WIDTH + px] = pField[(py-1) * FIELD_WIDTH +px];     // Remove this row & shift all rows above downwards
                        }
                        pField[px] = 0;
                    }

                    display(hConsoleOut, screen, pField, character_field, tetrominos, player1, score);      // Display these changed
                    Sleep(500);
            }}
            for(int i=0; i<FIELD_HEIGHT; i++){deletedRows[i] = 0;}      // Reset deletedRows[] to all 0's
        }


        /// GAME OVER: ================================================================================================================
        system("cls");
        if(score > high_score){high_score = score;}
        printf("GAME OVER!!\nFINAL SCORE: %d\nHIGHEST SCORE: %d\n", score, high_score);

        do{
            fflush(stdin);
            printf("Would you like to play again (y/n)\n");
            scanf("%c", &retry_response);
        }while(retry_response != 'y' && retry_response != 'n');

    }while(retry_response == 'y');

    system("cls");
    printf("HIGHEST SCORE: %d\n\n\n", high_score);

	return 0;
}
