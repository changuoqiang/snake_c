#include <ncurses.h>
#include <stdlib.h> // srand, rand 
#include <time.h>   // time
#include <string.h> // memset

const int DELAY = 200;
const char FOOD = '#';
const char SNAKE = 'o';
int max_y = 0, max_x = 0;
int next_y = 0, next_x = 0;
bool game_over = false;
int score = 0;

// move direction
typedef enum {
    LEFT,
    RIGHT,
    UP,
    DOWN
} direction;
direction cur_dir = RIGHT;

//postion
typedef struct {
    int x;
    int y;
} point;
point food = {0, 0};

// random create a food
void create_food(void){

    food.x = (rand()%(max_x-20)) + 10; //(10, max_x -1 + 10)
    food.y = (rand()%(max_y-10)) + 5;
}

void draw_ch(point pt, char ch){
    mvaddch(pt.y, pt.x, ch);
}

// snake node, bidirectional linked list
typedef struct node {
    struct node* prev;
    point pt;
    struct node* next;
} node;
node *head=NULL, *tail=NULL;

void curse_init(void){
    initscr();
    noecho();
    keypad(stdscr, true);
    cbreak();
    curs_set(false);

    getmaxyx(stdscr, max_y, max_x);
}
void curse_deinit(){
    endwin();
}

void game_init(void){
    srand(time(NULL));
    cur_dir = RIGHT;
    game_over = false;
    score = 0;
    next_x = 0;
    next_y = 0;

    clear(); // clear the screen
    //init the snake
    head = (node*)malloc(sizeof(node));
    memset(head, 0, sizeof(node));

    node* list = head;
    for(int i=1; i<3; i++){
        node* body = (node*)malloc(sizeof(node)); 
        memset(body, 0, sizeof(node));

        list->next = body;
        body->prev = list;
        list = list->next;
    }
    tail = list;

    list = head;
    int i = 0;
    while(list){
        list->pt.x = max_x/2 + i--;
        list->pt.y = max_y/2;
        list = list->next;
    }

    create_food();
    refresh();
}

void game_deinit(void){
    // clean the snake 
    node *list = head, *temp;

    while(list){
        temp = list->next;
        free(list);
        list = temp;
    }
}

void game_quit(void){
    game_deinit();
    curse_deinit();
    exit(0);
}

void draw_screen(void){
    clear();
    if(game_over)
        mvprintw(max_y/2, (max_x - 54)/2, "You Loser! click space or enter to continue, q to quit");

    if(!game_over){
        //draw snake
        node* list = head;
        while(list){
            draw_ch(list->pt, SNAKE);
            list = list->next;
        }
        //draw the current food
        draw_ch(food, FOOD);
    }

    //draw score
    mvprintw(0, 0, "Score: %i", score);
    mvprintw(0, (max_x-15)/2, "press q to quit");

    refresh();
}

int main(int argc, char** argv){
    curse_init();
    game_init();

    int ch = 0;
    while(true){
        draw_screen();
        getmaxyx(stdscr, max_y, max_x);

        timeout(DELAY);
        if(game_over){
            ch = getch();
            if(ch=='q') game_quit();
            if(ch==ERR || (ch!=' ' && ch!='\n')) continue;

            game_deinit();
            game_init();
        }

        ch = getch();
        if((ch=='d' || ch==KEY_RIGHT) && cur_dir!=LEFT){
            cur_dir = RIGHT;
        }else if((ch=='a' || ch==KEY_LEFT) && cur_dir!=RIGHT){
            cur_dir = LEFT;
        }else if((ch=='w' || ch==KEY_UP) && cur_dir!=DOWN){
            cur_dir = UP;
        }else if((ch=='s' || ch==KEY_DOWN) && cur_dir!=UP){
            cur_dir = DOWN;
        }else if(ch=='q'){
            game_quit();
        }

        next_x = head->pt.x;
        next_y = head->pt.y;
        if(cur_dir == RIGHT) next_x++;
        else if(cur_dir == LEFT) next_x--;
        else if(cur_dir == UP) next_y--;
        else if(cur_dir == DOWN) next_y++;
        
        if(next_x == food.x && next_y == food.y){
            //eat the food
            node* new_head = (node*)malloc(sizeof(node));

            new_head->prev = NULL;
            new_head->next = head;
            new_head->pt.x = food.x;
            new_head->pt.y = food.y;

            head->prev = new_head;
            head = new_head;
            
            //create new food
            create_food();
            // 
            score += 1;
        }else if((next_x >= max_x || next_x < 0) || (next_y >= max_y || next_y < 0)){
            // hit the edge
            game_over = true;
        }else{
            //collision detect with self body
            node* list = head;
            int i = 0 ;
            while(list){
                if(list->pt.x == next_x && list->pt.y == next_y){
                    game_over = true;
                    break;
                }
                list = list->next;
            }
            if(!game_over){
                //movement
                node* new_head = (node*)malloc(sizeof(node));

                new_head->prev = NULL;
                new_head->next = head;
                new_head->pt.x = next_x;
                new_head->pt.y = next_y;

                head->prev = new_head;
                head = new_head;
                
                node* new_tail = tail->prev;
                new_tail->next = NULL;
                free(tail);
                tail = new_tail;
            }
        }
    }
    game_deinit();
    curse_deinit();
    return 0;
}
