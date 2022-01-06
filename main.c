//Using SDL and standard IO
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>

//Screen dimension constants
const int FPS = 60;
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int CELL_NUM = 8;

// cell per second
const float SNAKE_SPEED = 1.0f;

typedef struct SNAKEPOINT {
    int x, y;
    struct SNAKEPOINT *next, *previous;
} SNAKE_POINT;

int mod(int a, int b) {
    return ((a % b) + b) % b;
}

int min(int a, int b) {
    return a < b ? a : b;
}

int max(int a, int b) {
    return a > b ? a : b;
}

int rangedRandom(int min, int upperBound) {
    return min + rand() % (upperBound - min);
}

bool inBetween(int number, int min, int max) {
    return number >= min && number <= max;
}

void drawBox(SDL_Renderer *sdlRenderer, int x, int y, Uint32 color) {
    Sint16 width1 = SCREEN_WIDTH * y / CELL_NUM;
    Sint16 height1 = SCREEN_HEIGHT * x / CELL_NUM;
    boxColor(sdlRenderer, width1, height1, width1 + SCREEN_WIDTH / CELL_NUM, height1 + SCREEN_HEIGHT / CELL_NUM, color);
}

void drawCircle(SDL_Renderer *sdlRenderer, int x, int y, Uint32 color) {
    Sint16 width1 = SCREEN_WIDTH * y / CELL_NUM;
    Sint16 height1 = SCREEN_HEIGHT * x / CELL_NUM;
    Sint16 ry = SCREEN_WIDTH / (2 * CELL_NUM), rx = SCREEN_HEIGHT / (2 * CELL_NUM);
    filledEllipseColor(sdlRenderer, width1 + ry, height1 + rx, ry, rx, color);
}

void insertPoint(SNAKE_POINT *point) {
    SNAKE_POINT *newPoint = malloc(sizeof *newPoint);
    newPoint->previous = point->previous;
    newPoint->next = point;
    newPoint->x = point->x;
    newPoint->y = point->y;
    point->previous->next = newPoint;
    point->previous = newPoint;
}

void removePoint(SNAKE_POINT *point) {
    point->previous->next = point->next;
    point->next->previous = point->previous;
    free(point);
}

typedef struct POSITION {
    int x;
    int y;
} POSITION;

bool checkConflict(SNAKE_POINT *head, int x, int y) {
    while (head->previous != NULL) {
        if (head->x == head->previous->x) {
            if (x == head->x) {
                int a = head->y, b = head->previous->y;
                int theMin = min(a, b), theMax = max(a, b);
                if (inBetween(y, theMin, theMax)) {
                    return true;
                }
            }
        } else {
            if (y == head->y) {
                int a = head->x, b = head->previous->x;
                int theMin = min(a, b), theMax = max(a, b);
                if (inBetween(x, theMin, theMax)) {
                    return true;
                }
            }
        }

        head = head->previous;
    }
    return false;
}

POSITION randomApple(SNAKE_POINT *head) {
    POSITION applePosition = {.x=rangedRandom(0, CELL_NUM), .y=rangedRandom(0, CELL_NUM)};
    while (checkConflict(head, applePosition.x, applePosition.y)) {
        applePosition.x = rangedRandom(0, CELL_NUM);
        applePosition.y = rangedRandom(0, CELL_NUM);
    }
    return applePosition;
}

int snakeLength(SNAKE_POINT *head) {
    int length = 1;
    while (head->previous != NULL) {
        length += (head->x - head->previous->x) + (head->y - head->previous->y);
        head = head->previous;
    }
    return length;
}

int main() {
    srand(time(NULL));   // Initialization, should only be called once.

    // 1-> right-left, 0-> top-down
    int direction = 1;
    // 1-> to the positive, -1-> reverse
    int toThePositive = 1;

    float betweenMovementTemp = 0.0f;

    SNAKE_POINT head = {.x=0, .y=2, .next=NULL, .previous=NULL};
    SNAKE_POINT tail = {.x=0, .y=0, .next=&head, .previous=NULL};
    head.previous = &tail;

    POSITION applePosition = randomApple(&head);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }
    SDL_Window *sdlWindow = SDL_CreateWindow("Snake_Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH,
                                             SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
    SDL_Renderer *sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

    SDL_bool shallExit = SDL_FALSE;
    while (shallExit == SDL_FALSE) {
        SDL_RenderClear(sdlRenderer);

        // draw table
        for (int i = 0; i < CELL_NUM; ++i) {
            for (int j = 0; j < CELL_NUM; ++j) {
                drawBox(sdlRenderer, i, j, ((mod(i + j, 2) == 0) ? 0xff00ff00 : 0xff008800));
            }
        }

        // listen for key events
        SDL_Event sdlEvent;
        while (SDL_PollEvent(&sdlEvent)) {
            switch (sdlEvent.type) {
                case SDL_QUIT:
                    shallExit = SDL_TRUE;
                    break;
                case SDL_KEYUP:
                    switch (sdlEvent.key.keysym.scancode) {
                        case SDL_SCANCODE_UP:
                            if (!(head.y == head.previous->y && head.x > head.previous->x)) {
                                direction = 0;
                                toThePositive = -1;
                            }
                            break;
                        case SDL_SCANCODE_DOWN:
                            if (!(head.y == head.previous->y && head.x < head.previous->x)) {
                                direction = 0;
                                toThePositive = 1;
                            }
                            break;
                        case SDL_SCANCODE_RIGHT:
                            if (!(head.x == head.previous->x && head.y < head.previous->y)) {
                                direction = 1;
                                toThePositive = 1;
                            }
                            break;
                        case SDL_SCANCODE_LEFT:
                            if (!(head.x == head.previous->x && head.y > head.previous->y)) {
                                direction = 1;
                                toThePositive = -1;
                            }
                            break;
                    }
            }
        }

        // Move
        betweenMovementTemp += SNAKE_SPEED / FPS;
        if (betweenMovementTemp > 1.0f) {
            betweenMovementTemp -= 1.0f;

            // right-left
            if (direction == 1) {
                if (head.y == head.previous->y) {
                    insertPoint(&head);
                }
                head.y += (toThePositive == 1) ? 1 : -1;
            } else {
                if (head.x == head.previous->x) {
                    insertPoint(&head);
                }
                head.x += (toThePositive == 1) ? 1 : -1;
            }

            // If snake ate the apple, stop the tail, else, make it move
            if (head.x == applePosition.x && head.y == applePosition.y) {
                applePosition = randomApple(&head);
            } else {
                if (tail.x != tail.next->x) {
                    tail.x += ((tail.next->x) - tail.x) > 0 ? 1 : -1;
                    if (tail.x == tail.next->x) {
                        removePoint(tail.next);
                    }
                } else {
                    tail.y += ((tail.next->y) - tail.y) > 0 ? 1 : -1;
                    if (tail.y == tail.next->y) {
                        removePoint(tail.next);
                    }
                }
            }

            if (!inBetween(head.y, 0, CELL_NUM) || !inBetween(head.x, 0, CELL_NUM) ||
                checkConflict(head.previous, head.x, head.y)) {
                shallExit = SDL_TRUE;
            }
            if (snakeLength(&head) == CELL_NUM * CELL_NUM) {
                shallExit = SDL_TRUE;
            }
        }

        SNAKE_POINT *drawingPoint = &head;
        Uint32 color = 0xffff0000;

        while (drawingPoint->previous != NULL) {
            // draw till the tail
            if (drawingPoint->x != drawingPoint->previous->x) {
                int a = drawingPoint->previous->x, b = drawingPoint->x;
                int theMin = min(a, b), theMax = max(a, b);
                for (int i = theMin + 1; i < theMax; ++i) {
                    drawBox(sdlRenderer, i, drawingPoint->y, color);
                }
            } else {
                int a = drawingPoint->previous->y, b = drawingPoint->y;
                int theMin = min(a, b), theMax = max(a, b);
                for (int i = theMin + 1; i < theMax; ++i) {
                    drawBox(sdlRenderer, drawingPoint->x, i, color);
                }
            }

            drawingPoint = drawingPoint->previous;
            drawBox(sdlRenderer, drawingPoint->x, drawingPoint->y, color);
        }
        drawCircle(sdlRenderer, head.x, head.y, color);

        Uint32 appleColor = 0xff0000ff;
        drawCircle(sdlRenderer, applePosition.x, applePosition.y, appleColor);

        SDL_RenderPresent(sdlRenderer);
        SDL_Delay(1000 / FPS);
    }

    // Exit
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();
    return 0;
}
