#include "lcd/lcd.h"
#include <string.h>
#include "utils.h"

void Inp_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOC);

    gpio_init(GPIOA, GPIO_MODE_IPD, GPIO_OSPEED_50MHZ, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_6);
    gpio_init(GPIOC, GPIO_MODE_IPD, GPIO_OSPEED_50MHZ, GPIO_PIN_13);
}

void IO_init(void)
{
    Inp_init(); // inport init
    Lcd_Init(); // LCD init
}
int lock = 0;
int level = 1;
int box = 0;
int score_board[3][3];
int main(void)
{   

    IO_init();         // init OLED
    int** map = (int**)malloc(20*sizeof(int*));
    int** is_move = (int**)malloc(20*sizeof(int*));
    for(int i=0;i<20;i++){
        map[i] = (int*)malloc(10*sizeof(int));
        is_move[i] = (int*)malloc(10*sizeof(int));
    }
    for(int i=0;i<20;i++){
        for(int j=0;j<10;j++){
            map[i][j]=0;/*0 is empty; 1 is movable; 2 is obstacle; 3 is target cell; 4 is sprite, 5 success*/
            is_move[i][j] = 0;//0 is not move,1 is move
        }
    }
    for(int i=0; i<3;i++){
        for(int j=0; j<3;j++){
            score_board[i][j]=999;
        }
    }
    int flag_finish_welcome=0;
    LCD_Clear(BLACK);
    if( load_welcome_scene() == 1 ){
            flag_finish_welcome = 1;
    }
    while(1){
        if(flag_finish_welcome == 1 && Get_Button(JOY_CTR) ){
            break;
        }
    }
    while(1){
        LCD_Clear(BLACK);
        level_scene();
        LCD_Clear(BLACK);
        play_scene(box,level,map);
        play(map,is_move);
        LCD_Clear(BLACK);
        display_score_board();
        uint64_t start_time,accum_time;
        while(1){
            if(lock==0){
                if(Get_Button(JOY_LEFT)||Get_Button(JOY_DOWN)||Get_Button(JOY_RIGHT)||Get_Button(JOY_CTR)||Get_Button(BUTTON_1)){
                    lock = 1;
                    start_time = get_timer_value();
                    break;
                }
            }
            else{
                accum_time=get_timer_value()-start_time;
                if(accum_time<((SystemCoreClock/4000.0)*300)){
                }
                else{
                    lock=0;
                }
            }

        }
        LCD_Clear(BLACK);
        for(int i=0;i<20;i++){
            for(int j=0;j<10;j++){
                map[i][j]=0;/*0 is empty; 1 is movable; 2 is obstacle; 3 is target cell; 4 is sprite, 5 success*/
                is_move[i][j] = 0;//0 is not move,1 is move
            }
        }
    }


    for(int i=0;i<20;i++){
        free(map[i]);
        free(is_move[i]);
    }
    free(map);
    free(is_move);

}
void display_score_board(){
    LCD_ShowString(40,1,"L1",WHITE);
    LCD_ShowString(80,1,"L2",WHITE);
    LCD_ShowString(120,1,"L3",WHITE);
    LCD_ShowString(26,20,"1",WHITE);
    LCD_ShowString(26,40,"2",WHITE);
    LCD_ShowString(26,60,"3",WHITE);
    LCD_DrawRectangle(20,0,35,79,WHITE);
    LCD_DrawRectangle(35,0,70,79,WHITE);
    LCD_DrawRectangle(70,0,105,79,WHITE);
    LCD_DrawRectangle(105,0,140,79,WHITE);
    LCD_DrawRectangle(20,1,140,20,WHITE);
    LCD_DrawRectangle(20,20,140,40,WHITE);
    LCD_DrawRectangle(20,40,140,60,WHITE);
    LCD_DrawRectangle(20,60,140,79,WHITE);
    for(int j=0;j<3;j++){
        for(int i=0;i<3;i++){
            LCD_ShowNum(40+j*35,22+i*20,score_board[i][j],3,WHITE);
        }
    }
}
void play(int** map,int **is_move){
    int num_success=0;
    uint64_t start_time,accum_time;
    int sprite_x,sprite_y;
    int return_value;
    int steps=0;
    while(1){
        
    for(int i=0;i<20;i++){
        for(int j=0;j<10;j++){
            if(map[i][j]==4){
                sprite_x=i;
                sprite_y=j;
            }
        }
    }
    if(lock==0){
            return_value=0;
            if (Get_Button(JOY_LEFT))
            {       
                map[sprite_x][sprite_y]=0;
                is_move[sprite_x][sprite_y]=1;
                lock = 1;
                start_time = get_timer_value();
                if(sprite_x>0 && map[sprite_x-1][sprite_y] != 2 && map[sprite_x-1][sprite_y] != 3 && map[sprite_x-1][sprite_y] != 5){
                    if(map[sprite_x-1][sprite_y] != 1 ){
                        sprite_x -= 1;
                    }
                    
                    if(map[sprite_x-1][sprite_y] == 1 ){
                        return_value=move_blue_box(sprite_x-1,sprite_y,1,map,is_move);
                        if(return_value!=0){
                            sprite_x -= 1;
                            if(return_value == 2){
                                num_success++;
                            }
                        }
                    } 
                    steps++;
                }
                map[sprite_x][sprite_y]=4;
            }
            if (Get_Button(JOY_DOWN))
            {
                map[sprite_x][sprite_y]=0;
                is_move[sprite_x][sprite_y]=1;
                lock=1;
                start_time = get_timer_value();
                if(sprite_y<9 && map[sprite_x][sprite_y+1] != 2 && map[sprite_x][sprite_y+1] != 3&& map[sprite_x][sprite_y+1] != 5){
                    if(map[sprite_x][sprite_y+1] != 1 ){
                        sprite_y += 1; 
                    }
                    if(map[sprite_x][sprite_y+1] == 1){
                        return_value= move_blue_box(sprite_x,sprite_y+1,2,map,is_move);
                        if(return_value!=0){
                            sprite_y += 1;
                            if(return_value == 2){
                                num_success++;
                            }
                        }
                    } 
                    steps++;
                }
                map[sprite_x][sprite_y]=4;

            }
            if (Get_Button(JOY_RIGHT))
            {
                map[sprite_x][sprite_y]=0;
                is_move[sprite_x][sprite_y]=1;
                lock=1;
                start_time = get_timer_value();
                if(sprite_x<19 && map[sprite_x+1][sprite_y] != 2 && map[sprite_x+1][sprite_y] != 3 && map[sprite_x+1][sprite_y] != 5){
                    if(map[sprite_x+1][sprite_y] != 1 ){
                        sprite_x += 1;
                    }
                    if(map[sprite_x+1][sprite_y] == 1){
                        return_value=move_blue_box(sprite_x+1,sprite_y,3,map,is_move);
                      if(return_value!=0){
                            sprite_x += 1;
                            if(return_value == 2){
                                num_success++;
                            }
                        }
                    } 
                    steps++;
                }
                map[sprite_x][sprite_y]=4;

            }
            if (Get_Button(JOY_CTR))
            {
                map[sprite_x][sprite_y]=0;
                is_move[sprite_x][sprite_y]=1;
                lock=1;
                start_time = get_timer_value();
                if(sprite_y>0 && map[sprite_x][sprite_y-1] != 2 && map[sprite_x][sprite_y-1] != 3 && map[sprite_x][sprite_y-1] != 5){
                    if(map[sprite_x][sprite_y-1] != 1 ){
                        sprite_y -= 1; 
                    }
                    if(map[sprite_x][sprite_y-1] == 1 ){
                        return_value = move_blue_box(sprite_x,sprite_y-1,4,map,is_move);
                      if(return_value!=0){
                            sprite_y -= 1;
                            if(return_value == 2){
                                num_success++;
                            }
                        }
                    } 
                    steps++;
                }
                map[sprite_x][sprite_y]=4;

            }
        }
        else{
            accum_time=get_timer_value()-start_time;
            if(accum_time<((SystemCoreClock/4000.0)*300)){
            }
            else{
                lock=0;
            }
        }   
        //LCD_Clear(BLACK);
        update_scene(map,steps,is_move);
        if(num_success == (box+1)){
            if(steps<score_board[0][level]){
                score_board[2][level]=score_board[1][level];
                score_board[1][level]=score_board[0][level];
                score_board[0][level]=steps;
                
            }
            else if(steps<score_board[1][level]&& steps>score_board[0][level]){
                score_board[2][level]=score_board[1][level];
                score_board[1][level]=steps;
            }
            else if(steps<score_board[2][level]&& steps>score_board[1][level]){
                score_board[2][level]=steps;
            }
            break;
        }
    }
    LCD_Clear(BLACK);
}
//void move(int **map){}
int move_blue_box(int sprite_x,int sprite_y,int direction, int** map,int**is_move){/*1 left 2 down 3 right 4 up*/
    if(direction == 1){
        if(sprite_x>0 && map[sprite_x-1][sprite_y] != 2 && map[sprite_x-1][sprite_y] != 5 && map[sprite_x-1][sprite_y] != 1){
            map[sprite_x][sprite_y] = 0;
            is_move[sprite_x][sprite_y]=1;
            sprite_x--;
            if(map[sprite_x][sprite_y] == 3){
                map[sprite_x][sprite_y] = 5;
                return 2;
            }
            else{
                map[sprite_x][sprite_y] = 1;
            }
            return 1;
        }
        return 0;
    }
    else if(direction == 2){
        if(sprite_y<9 && map[sprite_x][sprite_y+1] != 2 && map[sprite_x][sprite_y+1] != 5 && map[sprite_x][sprite_y+1] != 1){
            map[sprite_x][sprite_y] = 0;
            is_move[sprite_x][sprite_y]=1;
            sprite_y++;
            if(map[sprite_x][sprite_y] == 3){
                map[sprite_x][sprite_y] = 5;
                return 2;
            }
            else{
                map[sprite_x][sprite_y] = 1;
            }
            return 1;
        }
        return 0;
    }
    else if(direction == 3){
        if(sprite_x<19 && map[sprite_x+1][sprite_y] != 2 && map[sprite_x+1][sprite_y] != 5 && map[sprite_x+1][sprite_y] != 1){
            map[sprite_x][sprite_y] = 0;
            is_move[sprite_x][sprite_y]=1;
            sprite_x++;
            if(map[sprite_x][sprite_y] == 3){
                map[sprite_x][sprite_y] = 5;
                return 2;
            }
            else{
                map[sprite_x][sprite_y] = 1;
            }
            return 1;
        }
        return 0;
    }
    else if(direction == 4){
        if(sprite_y>0 && map[sprite_x][sprite_y-1] != 2 && map[sprite_x][sprite_y-1] != 5 && map[sprite_x][sprite_y-1] != 1){
            map[sprite_x][sprite_y] = 0;
            is_move[sprite_x][sprite_y]=1;
            sprite_y--;
            if(map[sprite_x][sprite_y] == 3){
                map[sprite_x][sprite_y] = 5;
                return 2;
            }
            else{
                map[sprite_x][sprite_y] = 1;
            }
            return 1;
        }
        return 0;
    }
}
void update_scene(int** map,int steps,int **is_move){
    // LCD_Address_Set(0,0,10,70);
    // for(u16 i = 0;i < 10;i++){
    //     for(u16 j = 0;j < 70;j++){
    //         LCD_WR_DATA(BLACK);
    //     }
    // }
    LCD_ShowNum(80,0,steps,3,YELLOW);
    for(int i=0;i<20;i++){
        for(int j=0; j<10; j++){
            if(is_move[i][j]){
                LCD_Address_Set(i*8,j*8,i*8+8,j*8+8);
                for(int m = i*8;m < i*8+9;m++){
                   for(int n = j*8;n < j*8+9;n++){
                        LCD_WR_DATA(BLACK);
                    }
                }
                is_move[i][j]=0;
            }
        }
    }
    for(int i=0;i<20;i++){
        for(int j=0; j<10; j++){
            if(map[i][j]==1 && is_move[i][j] == 0){
                LCD_draw_box(i*8,j*8,8,BLUE);
            }
            else if(map[i][j]==2 && is_move[i][j] == 0){
                LCD_draw_box(i*8,j*8,8,RED);
            }
            else if(map[i][j]==3){
                LCD_DrawRectangle(i*8,j*8,(i+1)*8,(j+1)*8,WHITE);
            }
            else if(map[i][j]==4 && is_move[i][j] == 0){
                 LCD_DrawCircle(i*8+4,j*8+4,4,YELLOW);
                 LCD_DrawCircle(i*8+4-2,j*8+4-2,1,YELLOW);
                 LCD_DrawCircle(i*8+4+2,j*8+4-2,1,YELLOW);
                 LCD_DrawLine(i*8+4-1,j*8+4+2,i*8+4+1,j*8+4+2,YELLOW);
            }
            else if (map[i][j]==5){
                LCD_draw_box(i*8,j*8,8,GREEN);
            }
        }
    }

   /* LCD_DrawCircle(sprite_x*8+4,sprite_y*8+4,4,YELLOW);
    LCD_DrawCircle(sprite_x*8+4-2,sprite_y*8+4-2,1,YELLOW);
    LCD_DrawCircle(sprite_x*8+4+2,sprite_y*8+4-2,1,YELLOW);
    LCD_DrawLine(sprite_x*8+4-1,sprite_y*8+4+2,sprite_x*8+4+1,sprite_y*8+4+2,YELLOW);*/
}
void play_scene(int box, int level,int ** map){
    if(level==0){/*0 is empty; 1 is movable; 2 is obstacle; 3 is target cell; 4 is sprite*/
        LCD_draw_box(32,64,8,RED);
        map[32/8][64/8]=2;
        LCD_draw_box(112,48,8,RED);
        map[112/8][48/8]=2;
        if(box==0){
            LCD_draw_box(8,64,8,BLUE);
            map[8/8][64/8]=1;
            LCD_DrawRectangle(24,32,32,40,WHITE);
            map[24/8][32/8]=3;
        }
        if(box==1){
            LCD_draw_box(8,64,8,BLUE);
            map[8/8][64/8]=1;
            LCD_DrawRectangle(24,32,32,40,WHITE);
            map[24/8][32/8]=3;
            LCD_draw_box(144,40,8,BLUE);
            map[144/8][40/8]=1;
            LCD_DrawRectangle(48,48,56,56,WHITE);
            map[48/8][48/8]=3;
        }
        if(box==2){
            LCD_draw_box(8,64,8,BLUE);
            map[8/8][64/8]=1;
            LCD_DrawRectangle(24,32,32,40,WHITE);
            map[24/8][32/8]=3;
            LCD_draw_box(144,40,8,BLUE);
            map[144/8][40/8]=1;
            LCD_DrawRectangle(48,48,56,56,WHITE);
            map[48/8][48/8]=3;
            LCD_draw_box(64,64,8,BLUE);
            map[64/8][64/8]=1;
            LCD_DrawRectangle(112,16,120,24,WHITE);
            map[112/8][16/8]=3;
        }
    }
    if(level==1){
        LCD_draw_box(32,64,8,RED);
        map[32/8][64/8]=2;
        LCD_draw_box(112,48,8,RED);
        map[112/8][48/8]=2;
        LCD_draw_box(144,16,8,RED);
        map[144/8][16/8]=2;
        LCD_draw_box(80,40,8,RED);
        map[80/8][40/8]=2;
        if(box==0){
            LCD_draw_box(8,64,8,BLUE);
            map[8/8][64/8]=1;
            LCD_DrawRectangle(24,32,32,40,WHITE);
            map[24/8][32/8]=3;
        }
        if(box==1){
            LCD_draw_box(8,64,8,BLUE);
            map[8/8][64/8]=1;
            LCD_DrawRectangle(24,32,32,40,WHITE);
            map[24/8][32/8]=3;
            LCD_draw_box(144,40,8,BLUE);
            map[144/8][40/8]=1;
            LCD_DrawRectangle(48,48,56,56,WHITE);
            map[48/8][48/8]=3;
        }
        if(box==2){
            LCD_draw_box(8,64,8,BLUE);
            map[8/8][64/8]=1;
            LCD_DrawRectangle(24,32,32,40,WHITE);
            map[24/8][32/8]=3;
            LCD_draw_box(144,40,8,BLUE);
            map[144/8][40/8]=1;
            LCD_DrawRectangle(48,48,56,56,WHITE);
            map[48/8][48/8]=3;
            LCD_draw_box(64,64,8,BLUE);
            map[64/8][64/8]=1;
            LCD_DrawRectangle(112,16,120,24,WHITE);
            map[112/8][16/8]=3;
        }
    }
    if(level==2){
        LCD_draw_box(32,64,8,RED);
        map[32/8][64/8]=2;
        LCD_draw_box(112,48,8,RED);
        map[112/8][48/8]=2;
        LCD_draw_box(144,16,8,RED);
        map[144/8][16/8]=2;
        LCD_draw_box(80,40,8,RED);
        map[80/8][40/8]=2;
        LCD_draw_box(8,24,8,RED);
        map[8/8][24/8]=2;
        LCD_draw_box(64,8,8,RED);
        map[64/8][8/8]=2;
        if(box==0){
            LCD_draw_box(8,64,8,BLUE);
            map[8/8][64/8]=1;
            LCD_DrawRectangle(24,32,32,40,WHITE);
            map[24/8][32/8]=3;
        }
        if(box==1){
            LCD_draw_box(8,64,8,BLUE);
            map[8/8][64/8]=1;
            LCD_DrawRectangle(24,32,32,40,WHITE);
            map[24/8][32/8]=3;
            LCD_draw_box(144,40,8,BLUE);
            map[144/8][40/8]=1;
            LCD_DrawRectangle(48,48,56,56,WHITE);
            map[48/8][48/8]=3;
        }
        if(box==2){
            LCD_draw_box(8,64,8,BLUE);
            map[8/8][64/8]=1;
            LCD_DrawRectangle(24,32,32,40,WHITE);
            map[24/8][32/8]=3;
            LCD_draw_box(144,40,8,BLUE);
            map[144/8][40/8]=1;
            LCD_DrawRectangle(48,48,56,56,WHITE);
            map[48/8][48/8]=3;
            LCD_draw_box(64,64,8,BLUE);
            map[64/8][64/8]=1;
            LCD_DrawRectangle(112,16,120,24,WHITE);
            map[112/8][16/8]=3;
        }
    }
    LCD_DrawCircle(68,36,4,YELLOW);
    LCD_DrawCircle(68-2,36-2,1,YELLOW);
    LCD_DrawCircle(68+2,36-2,1,YELLOW);
    LCD_DrawLine(68-1,36+2,68+1,36+2,YELLOW);
    map[64/8][32/8]=4;
}
void level_scene(){
    uint64_t start_time,accum_time;
    while(1){
        LCD_Address_Set(0,0,8,150);
        for(u16 i = 75;i < 150;i++){
            for(u16 j = 0;j < 8;j++){
                LCD_WR_DATA(BLACK);
            }
        }
        LCD_ShowString(10,10,"Level 1",WHITE);
        LCD_ShowString(10,30,"Level 2",WHITE);
        LCD_ShowString(10,50,"Level 3",WHITE);
        LCD_ShowString(110,30,"Boxes",WHITE);
        LCD_ShowNum(115,45,1,1,WHITE);
        LCD_ShowNum(125,45,2,1,WHITE);
        LCD_ShowNum(135,45,3,1,WHITE);
            
        LCD_DrawRectangle(10,10 + 20 * level,65,25+ 20 * level,RED);
        
        LCD_DrawRectangle(115 + 10 * box,45,122 + 10 * box,60,RED);
        if(lock == 0){

            if (Get_Button(JOY_LEFT))
            {   
                lock = 1;
                start_time = get_timer_value();
                box -= 1;
            //continue;
            }
            if (Get_Button(JOY_DOWN))
            {
                lock = 1;
                start_time = get_timer_value();
                level += 1; 

            }
            if (Get_Button(JOY_RIGHT))
            {
                lock = 1;
                start_time = get_timer_value();
                box += 1;
            }
            if (Get_Button(JOY_CTR))
            {
                lock = 1;
                start_time = get_timer_value();
                level -= 1;
            }
            if (Get_Button(BUTTON_1))
            {
                lock=1;
                start_time = get_timer_value();
            }
            if (Get_BOOT0()){
                return;
            }
            if(level>2){
                level -= 3;
            }
            if(level<0){
                level +=3;
            }
            if(box<0){
                box += 3;
            }
            if(box>2){
                box -= 3;
            }
        }
        else{
            accum_time=get_timer_value()-start_time;
            if(accum_time<((SystemCoreClock/4000.0)*300)){
            }
            else{
                lock=0;
            }
        }
    }
}


int load_welcome_scene(){
    u16 tick=0;
    uint64_t start_time,accum_time;
    while (1)
    {   
        tick++;
        LCD_Address_Set(0,0,63,160);
        for(u16 i = 0;i < 161;i++){
            for(u16 j = 60;j < 64;j++){
                LCD_WR_DATA(BLACK);
            }
        }
        welcome_scene(tick);
        //LCD_ShowPicture(50,50,80,80);
        if(lock==0){

            if (Get_Button(JOY_LEFT))
            {   
                lock = 1;
                start_time = get_timer_value();
                LCD_ShowString(5,25,"L", BLUE);
            //continue;
            }
            if (Get_Button(JOY_DOWN))
            {
                lock=1;
                start_time = get_timer_value();
                LCD_ShowString(25,45,"D", BLUE);
            }
            if (Get_Button(JOY_RIGHT))
            {
                lock=1;
                start_time = get_timer_value();
                LCD_ShowString(45,25,"R", BLUE);
            }
            if (Get_Button(JOY_CTR))
            {
                lock=1;
                start_time = get_timer_value();
                LCD_ShowString(25,25,"C", BLUE);
            }
            if (Get_Button(BUTTON_1))
            {
                lock=1;
                start_time = get_timer_value();
                LCD_ShowString(60,5,"SW1", BLUE);
            }
        }
        else{
            accum_time=get_timer_value()-start_time;
            if(accum_time<((SystemCoreClock/4000.0)*300)){

            }
            else{
                lock=0;
            }
        }
        if(tick>=160){
            break;
        }   
    }
    return 1;
}
void welcome_scene(u16 tick){
    LCD_ShowString(10,31,"Patrick's Paradox",BLUE);
    LCD_draw_box(15,15,15,BLUE);
    LCD_draw_box(31,15,15,RED);
    LCD_DrawRectangle(47,15,62,30,WHITE);
    LCD_draw_sprite(80,15,13,YELLOW);
    u16 x1=0,y1=60;
    u16 add=1;
    LCD_DrawLine(x1,y1,x1+add*tick,y1,WHITE);
    LCD_DrawLine(x1,y1+1,x1+add*tick,y1+1,WHITE);
    LCD_DrawLine(x1,y1+2,x1+add*tick,y1+2,WHITE);
}
void LCD_draw_box(u16 origin_x, u16 origin_y, u16 length, u16 color){
    LCD_DrawRectangle(origin_x,origin_y,origin_x+length,origin_y+length,color);
    LCD_Fill(origin_x,origin_y,origin_x+length,origin_y+length,color);
}
void LCD_draw_sprite(u16 x, u16 y, u16 r, u16 color){
    LCD_DrawCircle(x,y,r,color);
    LCD_DrawCircle(x-5,y-5,4,color);
    LCD_DrawCircle(x+5,y-5,4,color);
    LCD_DrawLine(x-4,y+5,x+4,y+5,color);
    //sLCD_Fill();
}
