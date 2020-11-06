#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL_image.h>


#define Grid 3
#define cellW (DisplayWidth / Grid)
#define cellH (DisplayHeight / Grid)

#define EMPTY 0
#define PLAYERX 1
#define PLAYERO 2

#define RUNNINGSTATE 0
#define PLAYERXWON 1
#define PLAYEROWON 2
#define TIESTATE 3
#define QUITESTATE 4

typedef struct {
    int board[Grid * Grid];
    int player;
    int state;
}game_t;

const SDL_Color GRID_COLOR = { .r = 255, .g = 255, .b = 255 };
const SDL_Color PLAYER_X_COLOR = { .r = 255, .g = 50, .b = 50 };
const SDL_Color PLAYER_O_COLOR = { .r = 50, .g = 100, .b = 255 };
const SDL_Color TIE_COLOR = { .r = 100, .g = 100, .b = 100 };
const int DisplayWidth = 610.0;
const int DisplayHeight = 610.0;


void NextTurn(game_t *game)
{
    if (game->player == PLAYERX)
    {
        game->player = PLAYERO;
    } else if (game->player == PLAYERO)
    {
        game->player = PLAYERX;
    }
}

int checkIfWon(game_t *game, int player)
{
    int rowCounter = 0;
    int columnCounter = 0;
    int diagonal1 = 0;
    int diagonal2 = 0;

    for (int i = 0; i < Grid; ++i)
    {
        for (int j = 0; j < Grid; ++j)
        {
            if (game->board[i * Grid + j] == player){
                rowCounter++;
            }
            if (game->board[j * Grid + i] == player){
                columnCounter++;
            }
        }

        if (rowCounter >= Grid || columnCounter >= Grid)
        {
            return 1;
        }
        rowCounter = 0;
        columnCounter = 0;

        if (game->board[i * Grid + i] == player)
        {
            diagonal1++;
        }
        if (game->board[i * Grid + Grid - i - 1] == player)
        {
            diagonal2++;
        }
    }

    return diagonal1 >= Grid || diagonal2 >= Grid;
}

int CountMoves(const int *board, int cell)
{
    int counter = 0;
    for (int i = 0; i < Grid * Grid; ++i)
    {
        if (board[i] == cell)
        {
            counter++;
        }
    }

    return counter;
}

void GameOver(game_t *game)
{
    if (checkIfWon(game, PLAYERX))
    {
        game->state = PLAYERXWON;
    } else if (checkIfWon(game, PLAYERO))
    {
        game->state = PLAYEROWON;
    } else if (CountMoves(game->board, EMPTY) == 0)
    {
        game->state = TIESTATE;
    }
}

void PlayerTurn(game_t *game, int row, int column)
{
    if (game->board[row * Grid + column] == EMPTY){
        game->board[row * Grid + column] = game->player;
        NextTurn(game);
        GameOver(game);
    }
}


void resetGame(game_t *game){
    game->player = PLAYERX;
    game->state = RUNNINGSTATE;
    for (int i = 0; i < Grid * Grid; ++i){
        game->board[i] = EMPTY;
    }
}

void click(game_t *game, int row, int column)
{
    if (game->state == RUNNINGSTATE){
        PlayerTurn(game, row, column);
    } else{
        resetGame(game);
    }
}

void renderGameOverState(SDL_Renderer *Renderer, const game_t *game, const SDL_Color *color)
{
    RenderGrid(Renderer, color);
    RenderBoard(Renderer, game->board, color, color);
}


void renderGame(SDL_Renderer *Renderer, const game_t *game)
{
    switch (game->state){
    case RUNNINGSTATE:
        renderRunning(Renderer, game);
        break;

    case PLAYERXWON:
        renderGameOverState(Renderer, game, &PLAYER_X_COLOR);
        break;

    case PLAYEROWON:
        renderGameOverState(Renderer, game, &PLAYER_O_COLOR);
        break;

    case TIESTATE:
        renderGameOverState(Renderer, game, &TIE_COLOR);

    default:{}

    }
}

void renderRunning(SDL_Renderer *Renderer, const game_t *game)
{
    RenderGrid(Renderer, &GRID_COLOR);
    RenderBoard(Renderer, game->board, &PLAYER_X_COLOR, &PLAYER_O_COLOR);

}

void RenderGrid(SDL_Renderer *Renderer, const SDL_Color * color)
{
    SDL_SetRenderDrawColor(Renderer, color->r, color->g, color ->b, 255);

    for (int i = 1; i <Grid; ++i){
        SDL_RenderDrawLine(Renderer, i * cellW, 0, i * cellH, DisplayHeight);
        SDL_RenderDrawLine(Renderer, 0, i * cellW, DisplayWidth, i * cellH);
    }
}

void renderX(SDL_Renderer *Renderer, int row, int column, const SDL_Color *color)
{
    const float halfBox = fmax(cellW, cellH) * 0.1;
    const float middleX = cellW * 0.6 + column * cellW;
    const float middleY = cellH * 0.3 + row * cellH;

    SDL_SetRenderDrawColor(Renderer,color->r, color->g, color->b, 255);
    SDL_Rect line1 = {middleX - halfBox, middleY - halfBox, 10, 100};
    SDL_RenderFillRect(Renderer, &line1);




}
void renderO(SDL_Renderer *Renderer, int row, int column, const SDL_Color *color)
{
    const float halfBox = fmax(cellW, cellH) * 0.1;
    const float middleX = cellW * 0.6 + column * cellW;
    const float middleY = cellH * 0.3 + row * cellH;

    SDL_SetRenderDrawColor(Renderer, color->r, color->g, color->b, 255);
    SDL_Rect line2 = {middleX - halfBox * 3, middleY + halfBox, 100, 10};
    SDL_RenderFillRect(Renderer, &line2);


}




void RenderBoard(SDL_Renderer *Renderer, const int *board, const SDL_Color * PLAYER_X_COLOR, const SDL_Color *PLAYER_O_COLOR)
{
    for (int i = 0; i < Grid; ++i){
        for (int j = 0; j < Grid; ++j){
            switch(board[i * Grid + j]){
            case PLAYERX:
                renderX(Renderer, i, j, PLAYER_X_COLOR);
                break;

            case PLAYERO:
                renderO(Renderer, i, j, PLAYER_O_COLOR);
                break;

            default:{}
            }
        }
    }
}


int main(int argc, char* argv[])
{
    if(SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    SDL_Window *GameDisplay = SDL_CreateWindow("Tic Tac Toe",
                                               100, 100,
                                               DisplayWidth, DisplayHeight,
                                               SDL_WINDOW_SHOWN);

    if (GameDisplay == NULL)
    {
        fprintf("SDL_createWindow ERROR: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Renderer *Renderer = SDL_CreateRenderer(GameDisplay, -1,
                                                SDL_RENDERER_ACCELERATED  | SDL_RENDERER_PRESENTVSYNC);

    if (Renderer == NULL)
    {
        SDL_DestroyWindow(GameDisplay);
        fprintf("SDL_CreateRenderer ERROR: %s\n", SDL_GetError());
        return EXIT_FAILURE;

    }

    game_t game = {
        .board = {EMPTY, EMPTY, EMPTY,
                  EMPTY, EMPTY, EMPTY,
                  EMPTY, EMPTY, EMPTY},
        .player = PLAYERX,
        .state = RUNNINGSTATE


    };


   // const float cellH = DisplayWidth / Grid;
   // const float cellW = DisplayHeight / Grid;

    SDL_Event event;
    while (game.state != QUITESTATE)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                game.state = QUITESTATE;
                break;



            case SDL_MOUSEBUTTONDOWN:
                click(&game,
                      event.button.y / cellH,
                      event.button.x / cellW);
                break;

            default:{}
            }
        }


        SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
        SDL_RenderClear(Renderer);

     //   SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
       // SDL_Rect line1 = {200, 0, 5, 600};
       // SDL_RenderFillRect(Renderer, &line1);

      //  SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
     //   SDL_Rect line2 = {400, 0, 5, 600};
     //   SDL_RenderFillRect(Renderer, &line2);

      //  SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
      //  SDL_Rect line3 = {0, 200, 600, 5};
      //  SDL_RenderFillRect(Renderer, &line3);

      //  SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
      //  SDL_Rect line4 = {0, 400, 600, 5};
      //  SDL_RenderFillRect(Renderer, &line4);


        renderGame(Renderer, &game);
        SDL_RenderPresent(Renderer);

    }

    SDL_DestroyWindow(GameDisplay);
    SDL_Quit;

    return EXIT_SUCCESS;
}
