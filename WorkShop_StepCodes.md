#کارگاه پروژه‌ی FoP Fall2021

[comment]: <> (![Drag Racing]&#40;hama.png&#41;)

[comment]: <> (_They threw me in prison to rot, along with my brothers and sisters! They deserve the same! You must carry on my work._)


## طریقه‌ی نصب SDL2
* Ubuntu:
```
sudo apt install libsdl2-dev libsdl2-gfx-dev
```

## راه‌اندازی کتابخانه
* CMake
```cmake
include_directories("/usr/include/SDL2")
target_link_libraries(${PROJECT_NAME} m SDL2 SDL2_gfx)
```

* Including Libs
```c
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
```

* First Init
```c
#include <stdio.h>
#include <stdbool.h>

int main() {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    return 0;
  }
  
  printf("Hello World\n");
  SDL_Quit();
  return 0;
}
```

## Window
```c
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

///////////// main
SDL_Window *sdlWindow = SDL_CreateWindow("Test_Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH,
SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
// error handling

SDL_Delay(1000);
SDL_DestroyWindow(sdlWindow);
```

## Renderer
```c
const int FPS = 60;

//// Main
SDL_Renderer *sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

while (true) {
    SDL_SetRenderDrawColor(sdlRenderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderClear(sdlRenderer);
    
    // draw box using gfx_primitives
    Uint32 color = 0xffffff00;
    Sint16 x1 = 60, x2 = 280;
    Sint16 y1 = 160, y2 = 320;
    boxColor(sdlRenderer, x1, y1, x2, y2, color);
    
    // draw circle using gfx_primitives
    Uint32 color2 = 0xff0000ff;
    Sint16 x = 480, y = 240, r = 100;
    filledCircleColor(sdlRenderer, x, y, r, color2);
    
    SDL_RenderPresent(sdlRenderer);
    SDL_Delay(1000 / FPS);
}
```

### Let's add Movement
```c
////// Main
Sint16 x1 = 60, x2 = 280;
Sint16 y1 = 160, y2 = 320;

//////// While
x1++; x2++;
```

### Let's add Fade transition
```c
////// Main
Uint32 color2 = 0xff0000ff;

//////// While
color2 -= 0x01000000;
```

### Let's add Relative Rotation
```c
////// Main
float theta = 0.f;

//////// While
// draw circle using gfx_primitives
Sint16 x = 480, y = 240;
Sint16 rotation_r = 20, r = 30;
Sint16 rotation_x = x + rotation_r * sin(theta), rotation_y = y + rotation_r * cos(theta);
filledCircleColor(sdlRenderer, rotation_x, rotation_y, r, color2);
theta += 0.3f;
```

## Event Handling Intro
```c
//// Main
SDL_bool shallExit = SDL_FALSE;
while (shallExit == SDL_FALSE) {
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent)) {
        switch (sdlEvent.type) {
            case SDL_QUIT:
                shallExit = SDL_TRUE;
                break;
        }
    }
}
```

## Step #1
```c
const int CELL_NUM = 8;

int mod(int a, int b) {
return ((a % b) + b) % b;
}

void drawBox(SDL_Renderer *sdlRenderer, int x, int y, Uint32 color) {
    Sint16 width1 = SCREEN_WIDTH * y / CELL_NUM;
    Sint16 height1 = SCREEN_HEIGHT * x / CELL_NUM;
    boxColor(sdlRenderer, width1, height1, width1 + SCREEN_WIDTH / CELL_NUM, height1 + SCREEN_HEIGHT / CELL_NUM, color);
}

//// Main
// draw table
for (int i = 0; i < CELL_NUM; ++i) {
    for (int j = 0; j < CELL_NUM; ++j) {
        drawBox(sdlRenderer, i, j, ((mod(i + j, 2) == 0) ? 0xff00ff00 : 0xff008800));
    }
}
```

## Step #2
```c

typedef struct SNAKEPOINT {
    int x, y;
    struct SNAKEPOINT *next, *previous;
} SNAKE_POINT;

int min(int a, int b) {
return a < b ? a : b;
}

int max(int a, int b) {
return a > b ? a : b;
}

void drawCircle(SDL_Renderer *sdlRenderer, int x, int y, Uint32 color) {
    Sint16 width1 = SCREEN_WIDTH * y / CELL_NUM;
    Sint16 height1 = SCREEN_HEIGHT * x / CELL_NUM;
    Sint16 ry = SCREEN_WIDTH / (2 * CELL_NUM), rx = SCREEN_HEIGHT / (2 * CELL_NUM);
    filledEllipseColor(sdlRenderer, width1 + ry, height1 + rx, ry, rx, color);
}

///// Main

SNAKE_POINT head = {.x=0, .y=2, .next=NULL, .previous=NULL};
SNAKE_POINT tail = {.x=0, .y=0, .next=&head, .previous=NULL};
head.previous = &tail;

////// While

// draw
SNAKE_POINT *drawingPoint = &head;
Uint32 color = 0xffff0000;

// draw Snake
while (drawingPoint->previous != NULL) {
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
```

## Step 3
```c

bool inBetween(int number, int min, int max) {
return number >= min && number <= max;
}

// cell per second
const float SNAKE_SPEED = 1.0f;

///////// Main
// 1-> right-left, 0-> top-down
int direction = 1;
// 1-> to the positive, -1-> reverse
int toThePositive = 1;

float betweenMovementTemp = 0.0f;

//////////// While
// Move
betweenMovementTemp += SNAKE_SPEED / FPS;
if (betweenMovementTemp > 1.0f) {
    betweenMovementTemp -= 1.0f;
    
    // right-left
    if (direction == 1) {
        head.y += (toThePositive == 1) ? 1 : -1;
    } else {
        head.x += (toThePositive == 1) ? 1 : -1;
    }

    if (tail.x != tail.next->x) {
        tail.x += ((tail.next->x) - tail.x) > 0 ? 1 : -1;
    } else {
        tail.y += ((tail.next->y) - tail.y) > 0 ? 1 : -1;
    }
    
    if (!inBetween(head.y, 0, CELL_NUM) || !inBetween(head.x, 0, CELL_NUM)) {
        shallExit = SDL_TRUE;
    }
}
```

## Step 4
```c
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

/////// Main

/////// While
// Move
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
```

## Step 5 (Generate Apple)
```c
#include <time.h>

typedef struct POSITION {
    int x;
    int y;
} POSITION;

int rangedRandom(int min, int upperBound) {
    return min + rand() % (upperBound - min);
}

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

////// Main
srand(time(NULL));   // Initialization, should only be called once.

POSITION applePosition = randomApple(&head);

////// Move
// If snake ate the apple, stop the tail, else, make it move
if (head.x == applePosition.x && head.y == applePosition.y) {
    applePosition = randomApple(&head);
} 


/////// Draw
Uint32 appleColor = 0xff0000ff;
drawCircle(sdlRenderer, applePosition.x, applePosition.y, appleColor);

```

## Step 6
```c
int snakeLength(SNAKE_POINT *head) {
    int length = 1;
    while (head->previous != NULL) {
        length += (head->x - head->previous->x) + (head->y - head->previous->y);
        head = head->previous;
    }
    return length;
}

//// Main
////// Move
if (!inBetween(head.y, 0, CELL_NUM) || !inBetween(head.x, 0, CELL_NUM) ||
    checkConflict(head.previous, head.x, head.y)) {
    shallExit = SDL_TRUE;
}
if (snakeLength(&head) == CELL_NUM * CELL_NUM) {
    shallExit = SDL_TRUE;
}
```