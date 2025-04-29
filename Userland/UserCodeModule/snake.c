#include <snake.h>
#include <libc.h>
#include <stdint.h>
#include <sounds.h>
#include <libasm.h>
#include <random.h>

#define DIM_X 1024
#define DIM_Y 768
#define COLOR1 0x2474FF
#define COLOR2 0x44B6FF
#define TRANSPARENT 0x000000
#define APPLE_COLOR 0xFF0000
#define LEAF__COLOR 0x006000
#define COLOR_SNAKE_1 0x00FF00
#define COLOR_SNAKE_2 0x9F009F
#define MENU_BACKGROUND_COLOR 0xD0B000
#define FONT_COLOR 0xFF0000
#define EDGE_COLOR 0x505050
#define GAPPLE_COLOR 0xFFFF00

#define PIXEL_PER_BLOCK 32
#define BLOCKS_DIM 20
#define DIM_LEFT_MARGIN (((DIM_X)-(PIXEL_PER_BLOCK*BLOCKS_DIM))/2)
#define DIM_RIGHT_MARGIN DIM_X-DIM_LEFT_MARGIN
#define DIM_TOP_MARGIN (((DIM_Y)-(PIXEL_PER_BLOCK*BLOCKS_DIM))/2)
#define DIM_BOTTOM_MARGIN DIM_Y-DIM_TOP_MARGIN
#define MENU_LEFT_MARGIN DIM_LEFT_MARGIN/8
#define MENU_RIGHT_MARGIN MENU_LEFT_MARGIN*7
#define MENU_TOP_MARGIN DIM_TOP_MARGIN
#define MENU_BOTTOM_MARGIN DIM_BOTTOM_MARGIN
#define APPLE_RED_STATUS 2
#define APPLE_GOLD_STATUS 3

#define ONE_PLAYER 1
#define TWO_PLAYERS 2
#define GAPPLE 3
#define EXIT 4

#define TOTAL_OF_BLOCKS (BLOCKS_DIM*BLOCKS_DIM)

typedef struct {
    int x;
    int y;
}Block;

static int option=1;
static int lastOption=0;
static char noColisionsP1=1;
static char noColisionsP2=1;
static char noWinner=1;
static char actualApple=2;
static char map[TOTAL_OF_BLOCKS];
static char goldAppleInit=0;
static char gappleMode=0;

static uint32_t appleSpray[PIXEL_PER_BLOCK][PIXEL_PER_BLOCK] = {
    TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT     ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT    ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT    ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT    ,LEAF__COLOR,TRANSPARENT,TRANSPARENT,TRANSPARENT    ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT   ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT,//1
    TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT     ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT    ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT    ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT    ,LEAF__COLOR,LEAF__COLOR,LEAF__COLOR,LEAF__COLOR    ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT   ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT,//2
    TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT     ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT    ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT    ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT    ,LEAF__COLOR,LEAF__COLOR,LEAF__COLOR,LEAF__COLOR    ,LEAF__COLOR,LEAF__COLOR,LEAF__COLOR,TRANSPARENT ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT   ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT,//3
    TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT     ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT    ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT    ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT    ,LEAF__COLOR,LEAF__COLOR,LEAF__COLOR,LEAF__COLOR    ,LEAF__COLOR,LEAF__COLOR,LEAF__COLOR,LEAF__COLOR ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT   ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT,//4

    TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT     ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT    ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT    ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT    ,LEAF__COLOR,LEAF__COLOR,LEAF__COLOR,LEAF__COLOR    ,LEAF__COLOR,LEAF__COLOR,LEAF__COLOR,LEAF__COLOR ,LEAF__COLOR,TRANSPARENT,TRANSPARENT,TRANSPARENT   ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT,//5
    TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT     ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT    ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT    ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT    ,LEAF__COLOR,LEAF__COLOR,LEAF__COLOR,LEAF__COLOR    ,LEAF__COLOR,LEAF__COLOR,LEAF__COLOR,LEAF__COLOR ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT   ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT,//6
    TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT     ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT    ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT    ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT    ,LEAF__COLOR,LEAF__COLOR,LEAF__COLOR,LEAF__COLOR    ,LEAF__COLOR,LEAF__COLOR,LEAF__COLOR,TRANSPARENT ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT   ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT,//7
    TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT     ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT    ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT    ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT    ,LEAF__COLOR,LEAF__COLOR,TRANSPARENT,TRANSPARENT    ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT   ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT,//8

    TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT     ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT    ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT    ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT    ,LEAF__COLOR,TRANSPARENT,TRANSPARENT,TRANSPARENT    ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT   ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT,//9
    TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT     ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,TRANSPARENT,TRANSPARENT,TRANSPARENT    ,LEAF__COLOR,TRANSPARENT,TRANSPARENT,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR   ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT,//10
    TRANSPARENT,TRANSPARENT,TRANSPARENT,APPLE_COLOR     ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,TRANSPARENT    ,LEAF__COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR   ,APPLE_COLOR,TRANSPARENT,TRANSPARENT,TRANSPARENT,//11
    TRANSPARENT,TRANSPARENT,APPLE_COLOR,APPLE_COLOR     ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR   ,APPLE_COLOR,APPLE_COLOR,TRANSPARENT,TRANSPARENT,//12

    TRANSPARENT,TRANSPARENT,APPLE_COLOR,APPLE_COLOR     ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR   ,APPLE_COLOR,APPLE_COLOR,TRANSPARENT,TRANSPARENT,//13
    TRANSPARENT,TRANSPARENT,APPLE_COLOR,APPLE_COLOR     ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR   ,APPLE_COLOR,APPLE_COLOR,TRANSPARENT,TRANSPARENT,//14
    TRANSPARENT,TRANSPARENT,APPLE_COLOR,APPLE_COLOR     ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR   ,APPLE_COLOR,APPLE_COLOR,TRANSPARENT,TRANSPARENT,//15
    TRANSPARENT,TRANSPARENT,APPLE_COLOR,APPLE_COLOR     ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR   ,APPLE_COLOR,APPLE_COLOR,TRANSPARENT,TRANSPARENT,//16

    TRANSPARENT,TRANSPARENT,APPLE_COLOR,APPLE_COLOR     ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR   ,APPLE_COLOR,APPLE_COLOR,TRANSPARENT,TRANSPARENT,//17
    TRANSPARENT,TRANSPARENT,APPLE_COLOR,APPLE_COLOR     ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR   ,APPLE_COLOR,APPLE_COLOR,TRANSPARENT,TRANSPARENT,//18
    TRANSPARENT,TRANSPARENT,APPLE_COLOR,APPLE_COLOR     ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR   ,APPLE_COLOR,APPLE_COLOR,TRANSPARENT,TRANSPARENT,//19
    TRANSPARENT,TRANSPARENT,APPLE_COLOR,APPLE_COLOR     ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR   ,APPLE_COLOR,APPLE_COLOR,TRANSPARENT,TRANSPARENT,//20

    TRANSPARENT,TRANSPARENT,APPLE_COLOR,APPLE_COLOR     ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR   ,APPLE_COLOR,APPLE_COLOR,TRANSPARENT,TRANSPARENT,//21
    TRANSPARENT,TRANSPARENT,APPLE_COLOR,APPLE_COLOR     ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR   ,APPLE_COLOR,APPLE_COLOR,TRANSPARENT,TRANSPARENT,//22
    TRANSPARENT,TRANSPARENT,APPLE_COLOR,APPLE_COLOR     ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR   ,APPLE_COLOR,APPLE_COLOR,TRANSPARENT,TRANSPARENT,//23
    TRANSPARENT,TRANSPARENT,APPLE_COLOR,APPLE_COLOR     ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR   ,APPLE_COLOR,APPLE_COLOR,TRANSPARENT,TRANSPARENT,//24

    TRANSPARENT,TRANSPARENT,APPLE_COLOR,APPLE_COLOR     ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR   ,APPLE_COLOR,APPLE_COLOR,TRANSPARENT,TRANSPARENT,//25
    TRANSPARENT,TRANSPARENT,APPLE_COLOR,APPLE_COLOR     ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR   ,APPLE_COLOR,APPLE_COLOR,TRANSPARENT,TRANSPARENT,//26
    TRANSPARENT,TRANSPARENT,TRANSPARENT,APPLE_COLOR     ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR   ,APPLE_COLOR,TRANSPARENT,TRANSPARENT,TRANSPARENT,//27
    TRANSPARENT,TRANSPARENT,TRANSPARENT,APPLE_COLOR     ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR   ,APPLE_COLOR,TRANSPARENT,TRANSPARENT,TRANSPARENT,//28

    TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT     ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR   ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT,//29
    TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT     ,TRANSPARENT,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,TRANSPARENT   ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT,//30
    TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT     ,TRANSPARENT,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,TRANSPARENT,TRANSPARENT    ,TRANSPARENT,TRANSPARENT,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,TRANSPARENT   ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT,//31
    TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT     ,TRANSPARENT,TRANSPARENT,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR    ,APPLE_COLOR,TRANSPARENT,TRANSPARENT,TRANSPARENT    ,TRANSPARENT,TRANSPARENT,TRANSPARENT,APPLE_COLOR    ,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR,APPLE_COLOR ,APPLE_COLOR,APPLE_COLOR,TRANSPARENT,TRANSPARENT   ,TRANSPARENT,TRANSPARENT,TRANSPARENT,TRANSPARENT,//32
};
static uint32_t appleGoldSpray[PIXEL_PER_BLOCK][PIXEL_PER_BLOCK]={0};

static void points(Snake* s1,Snake* s2){
    setZoom(2);
    setFontColor(FONT_COLOR);
    setBackGroundColor(black);
    setCursor(DIM_X-(CHAR_WIDTH_*11*2),75);
    print("Scoreboard");
    setCursor(DIM_X-(CHAR_WIDTH_*10*2),120);
    char * points="P1: 000";
    itoa(s1->length-2,points+4,10,3);
    print(points);
    char * points2="P2: 000";
    if (option==2){
        setCursor(DIM_X-(CHAR_WIDTH_*10*2),165);
        itoa(s2->length-2,points2+4,10,3);
        print(points2);
    }
}
static void cleanPoints(){
    setCursor(DIM_X-(CHAR_WIDTH_*11*2),75);
    print("          ");
    setCursor(DIM_X-(CHAR_WIDTH_*10*2),120);
    print("       ");
    setCursor(DIM_X-(CHAR_WIDTH_*10*2),165);
    print("       ");
}


static void margen(){
    drawRectangle((Point){0,0},(Point){DIM_LEFT_MARGIN,DIM_Y},black);
    drawRectangle((Point){DIM_RIGHT_MARGIN,0},(Point){DIM_X,DIM_Y},black);
    drawRectangle((Point){DIM_LEFT_MARGIN,0},(Point){DIM_RIGHT_MARGIN,DIM_TOP_MARGIN},black);
    drawRectangle((Point){DIM_LEFT_MARGIN,DIM_BOTTOM_MARGIN},(Point){DIM_RIGHT_MARGIN,DIM_Y},black);
}

static uint32_t getColorBlock(int blockNumber){
    int fill=blockNumber/BLOCKS_DIM;
    return (blockNumber+fill)%2?COLOR1:COLOR2;
}

static void drawBlock(int blockNumber,int color,int defaultColor){//color if needed, defaultColor if the color is the default
    int y=blockNumber?(blockNumber/BLOCKS_DIM):0;
        drawRectangle(
            (Point){DIM_LEFT_MARGIN+(blockNumber%BLOCKS_DIM)*PIXEL_PER_BLOCK,DIM_TOP_MARGIN+y*PIXEL_PER_BLOCK},
            (Point){DIM_LEFT_MARGIN+((blockNumber%BLOCKS_DIM)+1)*PIXEL_PER_BLOCK,DIM_TOP_MARGIN+(y+1)*PIXEL_PER_BLOCK},
            defaultColor?getColorBlock(blockNumber):color
            );
}


static void drawVoidRectangle(Point topLeft,Point bottomRigth,uint32_t hexcolor,int width){
    drawRectangle(topLeft,(Point){bottomRigth.x,topLeft.y+width},hexcolor);
    drawRectangle((Point){topLeft.x,bottomRigth.y-width},bottomRigth,hexcolor);
    drawRectangle((Point){topLeft.x,topLeft.y+width},(Point){topLeft.x+width,bottomRigth.y-width},hexcolor);
    drawRectangle((Point){bottomRigth.x-width,topLeft.y+width},(Point){bottomRigth.x,bottomRigth.y-width},hexcolor);
}
static void fondo(){
    int color=0;
    for (int i = 0; i < TOTAL_OF_BLOCKS; i++,color++){
        drawBlock(i,0,1);
    }
}
static void makeAppleGolden(){
    for (int i = 0; i < PIXEL_PER_BLOCK; i++)
        for (int j = 0; j < PIXEL_PER_BLOCK; j++)
            appleGoldSpray[i][j]=appleSpray[i][j]==APPLE_COLOR?GAPPLE_COLOR:appleSpray[i][j];
}

static void setDefaults(){
    option=1;
    lastOption=0;
    noColisionsP1=1;
    noColisionsP2=1;
    noWinner=1;
    for (int i = 0; i < TOTAL_OF_BLOCKS; i++)
    {
        map[i]=0;
    }
    gappleMode=0;
}

static void drawselection(){
    drawVoidRectangle((Point){MENU_LEFT_MARGIN,MENU_TOP_MARGIN+(option-1)*3*CHAR_HEIGHT_},(Point){MENU_RIGHT_MARGIN,MENU_TOP_MARGIN+option*3*CHAR_HEIGHT_},FONT_COLOR,3);
    if (lastOption!=0)
    {
        drawVoidRectangle((Point){MENU_LEFT_MARGIN,MENU_TOP_MARGIN+(lastOption-1)*3*CHAR_HEIGHT_},(Point){MENU_RIGHT_MARGIN,MENU_TOP_MARGIN+lastOption*3*CHAR_HEIGHT_},MENU_BACKGROUND_COLOR,3);
    }    
    lastOption=option;
}
static Block getBlockPosition(int blockNumber){
    int x=blockNumber%BLOCKS_DIM;
    int y=blockNumber/BLOCKS_DIM;
    return (Block){x,y};
}

static void printControls(){
    setZoom(2);

    setCursor(DIM_X-(CHAR_WIDTH_*10*2),MENU_BOTTOM_MARGIN-CHAR_HEIGHT_*2*7);
    setBackGroundColor(black);
    setFontColor(FONT_COLOR);
    print("Controls:");
    
    setFontColor(COLOR_SNAKE_1);
    setCursor(DIM_X-(CHAR_WIDTH_*10*2),MENU_BOTTOM_MARGIN-CHAR_HEIGHT_*2*6);
    print("Player 1:");
    setCursor(DIM_X-(CHAR_WIDTH_*6*2),MENU_BOTTOM_MARGIN-CHAR_HEIGHT_*2*5);
    print("W");
    setCursor(DIM_X-(CHAR_WIDTH_*8*2),MENU_BOTTOM_MARGIN-CHAR_HEIGHT_*2*4);
    print("A S D");

    setFontColor(COLOR_SNAKE_2);
    setCursor(DIM_X-(CHAR_WIDTH_*10*2),MENU_BOTTOM_MARGIN-CHAR_HEIGHT_*2*3);
    print("Player 2:");
    setCursor(DIM_X-(CHAR_WIDTH_*6*2),MENU_BOTTOM_MARGIN-CHAR_HEIGHT_*2*2);
    print("I");
    setCursor(DIM_X-(CHAR_WIDTH_*8*2),MENU_BOTTOM_MARGIN-CHAR_HEIGHT_*2);
    print("J K L");
}

static void printOptions(){
    setZoom(2);
    setFontColor(FONT_COLOR);
    setBackGroundColor(MENU_BACKGROUND_COLOR);
    setCursor(80,75);
    print("1P");
    setCursor(80,120);
    print("2P");
    setCursor(50,165);
    print("NORMAL");
    setCursor(64,215);
    print("EXIT");
    setCursor((DIM_X/2)-(32*CHAR_WIDTH_),DIM_Y-CHAR_HEIGHT_*3);
    setBackGroundColor(black);
    setFontColor(FONT_COLOR);
    print ("Press enter to select an option");
}

static void cleanOptions(){
    drawRectangle((Point){MENU_LEFT_MARGIN,MENU_TOP_MARGIN},(Point){MENU_RIGHT_MARGIN,MENU_BOTTOM_MARGIN},0x000000);
    setBackGroundColor(black);
    setCursor((DIM_X/2)-(32*CHAR_WIDTH_),DIM_Y-CHAR_HEIGHT_*3);
    print ("                               ");
}

static void startCount(){
    setZoom(2);
    setFontColor(FONT_COLOR);
    setBackGroundColor(black);
    char c='3';
    for (int i = 3; 0 < i; i--){
        setCursor((DIM_X/2)-CHAR_WIDTH_,CHAR_HEIGHT_);
        putChar(c);
        playSoundForTicks(A4,5);
        sleep(9);
        c--;
    }
    setCursor((DIM_X/2)-(3*CHAR_WIDTH_),CHAR_HEIGHT_);
    print("GO!");
    playSoundForTicks(A4,9);
    setCursor((DIM_X/2)-(3*CHAR_WIDTH_),CHAR_HEIGHT_);
    print("   ");
}

static void setColisions(Snake* snake){
    if (snake->player==1){
            noColisionsP1=0;
    }else{
        noColisionsP2=0;
    }
}

static void drawApple(){
    int apple=randInt(0,TOTAL_OF_BLOCKS-1);
    while (map[apple]!=0){
        apple=randInt(0,TOTAL_OF_BLOCKS-1);
    }
    actualApple=APPLE_RED_STATUS;
    //Gold apple generation
    if (randInt(0,TOTAL_OF_BLOCKS-1)%20==0 || gappleMode){
        actualApple=APPLE_GOLD_STATUS;
    }
    map[apple]=actualApple;    
    
    int y=apple?(apple/BLOCKS_DIM):0;
    setCursor(DIM_LEFT_MARGIN+(apple%BLOCKS_DIM)*PIXEL_PER_BLOCK,DIM_TOP_MARGIN+y*PIXEL_PER_BLOCK);
    drawSpray(PIXEL_PER_BLOCK,PIXEL_PER_BLOCK,actualApple==APPLE_RED_STATUS?appleSpray:appleGoldSpray);
    }

static void controls(Snake* snake1,Snake* snake2){
    int c=getChar();
    Direction dir1=snake1->dir;
    Direction dir2=snake2->dir;
    while (c!=-2){
        if ((c=='W' || c=='w') && dir1!=DOWN){
            snake1->dir=UP;
        }else if ((c=='S' || c=='s') && dir1!=UP){
            snake1->dir=DOWN;
        }else if ((c=='A' || c=='a') && dir1!=RIGHT){
            snake1->dir=LEFT;
        }else if ((c=='D' || c=='d') && dir1!=LEFT){
            snake1->dir=RIGHT;
        }  
        if ((c=='I' || c=='i') && dir2!=DOWN){
            snake2->dir=UP;
        }else if ((c=='K' || c=='k') && dir2!=UP){
            snake2->dir=DOWN;
        }else if ((c=='J' || c=='j') && dir2!=RIGHT){
            snake2->dir=LEFT;
        }else if ((c=='L' || c=='l') && dir2!=LEFT){
            snake2->dir=RIGHT;
        }
        c=getChar();
    }
}

static void drawFullSnakeAtFirst(Snake* snake){
    drawBlock(snake->body[snake->head],snake->color,0);
    drawBlock(snake->body[snake->tail],snake->color,0);
}

static void drawSnakeHead(Snake *snake){
    drawBlock(snake->body[snake->head],snake->color,0);
}

static void cleanSnakeTail(Snake* snake){
    drawBlock(snake->body[snake->tail],0,1);
}

static void snakeSetter(Snake* snake,int headBlock, int tailBlock){
    snake->body[1]=headBlock;
    snake->body[0]=tailBlock;
    map[snake->body[snake->head]]=1;
    map[snake->body[snake->tail]]=1;
    
}
static void grow(Snake* snake){
    map[snake->body[snake->head]]=1;
    drawSnakeHead(snake);
}

static void actualizeSnakeAndCheckColisions(Snake* snake){
    //Check Colisions with the walls 
    int nextHeadIndex=snake->head+1;
    nextHeadIndex=nextHeadIndex%TOTAL_OF_BLOCKS;
    Block head=getBlockPosition(snake->body[snake->head]);
    if (snake->dir==UP){
        if (head.y==0){
            setColisions(snake);
            return;
        }
        snake->body[nextHeadIndex]=snake->body[snake->head]-BLOCKS_DIM;
    }else if (snake->dir==DOWN){
        if(head.y==BLOCKS_DIM-1){
            setColisions(snake);
            return;
        }
        snake->body[nextHeadIndex]=snake->body[snake->head]+BLOCKS_DIM;
    }else if (snake->dir==LEFT){
        if (head.x==0){
            setColisions(snake);
            return;
        }
        snake->body[nextHeadIndex]=snake->body[snake->head]-1;
    }else if (snake->dir==RIGHT){
        if (head.x==BLOCKS_DIM-1){
            setColisions(snake);
            return;
        }
        snake->body[nextHeadIndex]=snake->body[snake->head]+1;
    }
///////////////////////////////////////////////////////////////////////
//Check Colisions with the snake/s
    snake->head=nextHeadIndex;
    if (map[snake->body[snake->head]]==1){
        setColisions(snake); 
        return;
    }
///////////////////////////////////////////////////////////////////////
//Check Colisions with the apple
    if (map[snake->body[snake->head]]==APPLE_RED_STATUS||map[snake->body[snake->head]]==APPLE_GOLD_STATUS){
        Sound s1={.frequency=E4,.ticks=0};
        setSound(s1);
        if (map[snake->body[snake->head]]==APPLE_GOLD_STATUS){
            snake->toGrow+=5;
            Sound s2={.frequency=A4,.ticks=0};
            setSound(s2);
        }else{
            snake->toGrow++;
        }
        drawApple();
    }
///////////////////////////////////////////////////////////////////////
//Actualize the snake (grow or move)
    if (snake->toGrow>0){
        snake->toGrow--;
        snake->length++;
        grow(snake);
    }else{
        map[snake->body[snake->head]]=1;
        map[snake->body[snake->tail]]=0;
        drawSnakeHead(snake);
        cleanSnakeTail(snake);
        snake->tail++;
        snake->tail=snake->tail%TOTAL_OF_BLOCKS;
    }
///////////////////////////////////////////////////////////////////////
//Check if there is a winner
    if (snake->length==TOTAL_OF_BLOCKS){
            noWinner=0;
            return;
    }
///////////////////////////////////////////////////////////////////////
}

static void startGame(){
    Snake p1={.body={0},.head=1,.tail=0,.length=2,.player=1,.dir=RIGHT,.color=COLOR_SNAKE_1};
    snakeSetter(&p1,204,203);
    drawFullSnakeAtFirst(&p1);
    Snake p2={.body={0},.head=1,.tail=0,.length=2,.player=2,.dir=LEFT,.color=COLOR_SNAKE_2};
    if (option==2){
        snakeSetter(&p2,215,216);
        drawFullSnakeAtFirst(&p2);
    }
    startCount();//contador de inicio para la partida
    drawApple();
    do{
        points(&p1,&p2);
        for (int i = 0; i < 5; i++){
            sleep(1);
            actulizeSound();
        }
        controls(&p1,&p2);
        actualizeSnakeAndCheckColisions(&p1);
        if (option==2){
            actualizeSnakeAndCheckColisions(&p2);
        }
    } while (noColisionsP1 && noColisionsP2 && noWinner);
}

static void setWinner(){
    setZoom(2);
    setFontColor(FONT_COLOR);
    setBackGroundColor(black);
    setCursor((DIM_X/2)-(14*CHAR_WIDTH_),CHAR_HEIGHT_);
    if (!noColisionsP1 && !noColisionsP2){
        setCursor((DIM_X/2)-(5*CHAR_WIDTH_),CHAR_HEIGHT_);
        print("Draw!");
    }else if (!noColisionsP1){
        print("Player 2 wins!");
    }else if (!noColisionsP2){
        print("Player 1 wins!");
    }
}
static void gameOverOrWinner(){
    setZoom(2);
    setFontColor(FONT_COLOR);
    setBackGroundColor(black);
    if (!noWinner){
                setCursor((DIM_X/2)-(7*CHAR_WIDTH_),CHAR_HEIGHT_);
                print("Winner!");
                return;
    }
    setCursor((DIM_X/2)-(10*CHAR_WIDTH_),CHAR_HEIGHT_);
    print("Game Over!");
}

static uint64_t ticks=0;
static uint64_t initialTicks=0;
static char flagSound=0;


void startSelectSound(){
    flagSound=1;
    playSound(F4);
    initialTicks=getTicks();
}
void stopAndChangeSound(){
    if(ticks-initialTicks>2 && flagSound==1){
        playSound(G4);
        flagSound=2;
        initialTicks=getTicks();
        return;
    }
    if (ticks-initialTicks>2 && flagSound==2 && option==GAPPLE){
        playSound(A4);
        flagSound=3;
        initialTicks=getTicks();
        return;
    }
    if (ticks-initialTicks>2 && (flagSound==2||flagSound==3)){
        initialTicks=0;
        stopSound();
        return;
    }      
}

static void selector(){
    drawselection();
    int c=getChar();
    do
    {
        ticks=getTicks();
        if ((c == 'W' || c == 'w' || c == 'I' || c == 'i')&& option>ONE_PLAYER){
            option--;
            startSelectSound();
            drawselection();
        }else if ((c == 'S' || c == 's' || c == 'K' || c == 'k')&& option<EXIT){
            option++;
            startSelectSound();
            drawselection();
        }
        if (option==GAPPLE && c=='\n'){
            startSelectSound();
            gappleMode=(!gappleMode);
            setFontColor(FONT_COLOR);
            setBackGroundColor(MENU_BACKGROUND_COLOR);
            setCursor(50,165);
            print(gappleMode?"GAPPLE":"NORMAL");
        }
        stopAndChangeSound();  
        _hlt();
        c=getChar();
    } while (c!='\n'||option==GAPPLE);
    stopSound();
    if (option==ONE_PLAYER || option==TWO_PLAYERS){
        cleanOptions(); //limpia el menu
        startGame();//inicia la partida
        if (option==TWO_PLAYERS){
            setWinner();
        }else if (option==ONE_PLAYER){
            gameOverOrWinner();
        }
        if (noWinner){
            playSoundForTicks(C4,8);
            playSoundForTicks(B3,8);
            playSoundForTicks(207,8);//G3#
            playSoundForTicks(G3,8);
        }else{
            playSoundForTicks(F4,8);
            playSoundForTicks(A4,8);
            playSoundForTicks(G4,8);
            playSoundForTicks(D5,8);

        }
        
        
        setCursor((DIM_X/2)-(23*CHAR_WIDTH_),DIM_Y-CHAR_HEIGHT_*3);
        setBackGroundColor(black);
        setFontColor(FONT_COLOR);
        print ("Press enter to continue");
        c=getChar();
        while(c!='\n'){
            c=getChar();
        }
        setCursor((DIM_X/2)-(25*CHAR_WIDTH_),DIM_Y-CHAR_HEIGHT_*3);
        print ("                         ");
        setCursor((DIM_X/2)-(16*CHAR_WIDTH_),CHAR_HEIGHT_);
        print("                ");
        cleanPoints();
        fondo();
    }else if (option==3){
        cleanFullScreen();//en caso de seleccionar EXIT limpia la pantalla
    }
}


static void chooseOptions(){
    setDefaults();//reinicio de variables
    drawRectangle((Point){MENU_LEFT_MARGIN,MENU_TOP_MARGIN},(Point){MENU_RIGHT_MARGIN,MENU_BOTTOM_MARGIN},MENU_BACKGROUND_COLOR);//dibuja el marco del menu
    printOptions();//imprime las opciones P1|P2|EXIT
    selector();
}

void snake(){
    margen();//dibuja un marco negro para limpiar la pantalla
    fondo();//dibuja el mantel
    drawVoidRectangle((Point){DIM_LEFT_MARGIN-3,DIM_TOP_MARGIN-3},(Point){DIM_RIGHT_MARGIN+3,DIM_BOTTOM_MARGIN+3},EDGE_COLOR,3);
    printControls();
    if (!goldAppleInit){
        makeAppleGolden();
    }
    while (option!=EXIT){//reinicia las partidas
        chooseOptions();//selector de opciones P1|P2|EXIT
    }
    setDefaults();
    cleanFullScreen();
}
    /*

    Player 1 controlers
      W
    A S D

    Player 2 controlers
      I
    J K L

    */