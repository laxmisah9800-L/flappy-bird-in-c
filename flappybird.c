#include<stdio.h>
#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h> // Include TTF for on screen text
#include<stdlib.h>

const int SCREEN_WIDTH = 400;
const int SCREEN_HEIGHT = 600;

typedef struct{
   float y;
   float velocity;
}Bird;

typedef struct
{
   float x;
   float height;
   int passed; // Track if this pipe has already given a point
}Pipe;

//Initialize the bird
Bird bird={300.0f,0.0f};
const float gravity=0.25f;
const float jump_strength = -6.0f;
const float pipe_gap=150;
const float pipe_width=60;
const float pipe_speed = 3.0f; //Speed of the pipes
Pipe pipes[3];

int score = 0;
TTF_Font *font = NULL;  // Added for text
// simple helper function to draw text on screen
void drawText(SDL_Renderer *renderer, const char *text, int x, int y, SDL_Color color, int size)
{
 if(!font) return;
 TTF_Font *tempFont = TTF_OpenFont("font.ttf",size);
 if(!tempFont) return;
 SDL_Surface *surface = TTF_RenderText_Solid(tempFont,text,color);
 if(!surface){TTF_CloseFont(tempFont); return;}
 SDL_Texture *texture =SDL_CreateTextureFromSurface(renderer,surface);
 SDL_Rect rect={x-surface->w/2,y,surface->w,surface->h}; // surface->w, surface->h x=200 width 100

 SDL_RenderCopy(renderer,texture,NULL,&rect);
  // cleanup
 SDL_FreeSurface(surface);
 SDL_DestroyTexture(texture);
 TTF_CloseFont(tempFont);

}

int main(int argc, char* argv[])
 //1.Initialize SDL(Video subsystem)
{
   if(SDL_Init(SDL_INIT_VIDEO)<0)
   {
   printf("SDL could not initialize! Error:%s\n",SDL_GetError());
   }

   // Initialize ttf
   if (TTF_Init()<0)
   {
     printf("TTF could not initialize! Error:%s\n",TTF_GetError()); 
   }
   font =TTF_OpenFont("font.ttf",48);
   if(!font){
      printf("font.ttf not found!\n");
      printf("Download any .ttf file rename it to font.ttf\n");
   }

    
   //2.Create the window
   SDL_Window* window = SDL_CreateWindow("My Game window",
    SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN);

    if(window == NULL)
   {
    printf("Window could not be created! Error:%s\n",SDL_GetError());
   }
   //3.Creating the renderer
   SDL_Renderer* renderer= SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
   
   if(renderer==NULL)
   {
      printf("Renderer could not be created! Error:%s\n",SDL_GetError());
   }
   

   //----- Initialize pipes-----
   for (int i = 0; i < 3; i++)
   {
     pipes[i].x=SCREEN_WIDTH+(i*250);
     pipes[i].height=(rand()%250)+100;// Random Height between 100-350
   }
   
      
   //The Game Loop
   int running =1; //A flag to keep the loop going
   SDL_Event e;  //container to store all the events 
   
   
   while (running)
   {
      //Handle Inputs
      while (SDL_PollEvent(&e))
      {
         //Did the user click the 'X' button
        if(e.type == SDL_QUIT){
         running=0; //Stop the loop
        }
        if ((e.type==SDL_MOUSEBUTTONDOWN||e.type==SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE||e.type==SDL_FINGERDOWN))
        {
             bird.velocity=jump_strength;
        }

      }
      //----Physics----
      bird.velocity=bird.velocity+gravity;
      bird.y=bird.y+bird.velocity;

      for (int i = 0; i < 3; i++)
      {
         //Moves pipe left
         pipes[i].x=pipes[i].x-pipe_speed;
         // scoreboard logic
         //if the pipe's right edge passes the bird left edge(x=50)
         if(pipes[i].passed == 0 && pipes[i].x+ pipe_width <50){
            score++;
            pipes[i].passed = 1; //mark as passed so we don't score again
            printf("score: %d\n",score);//print to console for now
         }
         //Recycle pipe if it leaves the screen
         if(pipes[i].x<-pipe_width){
            pipes[i].x=pipes[i].x+(3*250);
            pipes[i].height=(rand()%250)+100;
            pipes[i].passed = 0;
         
         }
      }
      
     
      if(bird.y<0||bird.y>(SCREEN_HEIGHT-30))
      {  
        SDL_Color red = {255,0,0,255};
        drawText(renderer,"Game Over",SCREEN_WIDTH/2,SCREEN_HEIGHT/2-50 , red,48);
        SDL_RenderPresent(renderer);
        SDL_Delay(800);
         
         score=0;
         bird.y=300.0f;
         bird.velocity=0.0f;


         
      }   
      
     //Change the color of screen to sky blue
     SDL_SetRenderDrawColor(renderer,135,206,235,255);
     SDL_RenderClear(renderer);
     
      //define the bird
     SDL_Rect bird_rect={50,(int)bird.y,30,30};// x=50,y=300

     //drawing the pipe
     SDL_SetRenderDrawColor(renderer,34,139,34,255); // set pipe color to green 
     for (int i = 0; i < 3; i++)
     {
      //top pipe
      SDL_Rect top_pipe={(int)pipes[i].x,0,(int)pipe_width,(int)pipes[i].height};
      //bottom pipe
      SDL_Rect bottom_pipe={(int)pipes[i].x,(int)pipes[i].height+pipe_gap,(int)pipe_width,(SCREEN_HEIGHT-((int)pipes[i].height+pipe_gap))};
      SDL_RenderFillRect(renderer,&top_pipe);
      SDL_RenderFillRect(renderer,&bottom_pipe);
      
      //collision logic
     if (SDL_HasIntersection(&bird_rect,&top_pipe)|| SDL_HasIntersection(&bird_rect,&bottom_pipe))
     {

        SDL_Color red = {255,0,0,255};
        drawText(renderer,"Game Over",SCREEN_WIDTH/2,SCREEN_HEIGHT/2-50 , red,48);
        SDL_RenderPresent(renderer);
        SDL_Delay(800);
         score=0;
         bird.y=300.0f;
         bird.velocity=0.0f;
         for (int i = 0; i < 3; i++)
         {
             pipes[i].x=SCREEN_WIDTH+(i*250);
             pipes[i].height=(rand()%250)+100;// Random Height between 100-350
             pipes[i].passed = 0;
             
         }
     
     }


     }
     
     //Draw the bird(Yellow)
     SDL_SetRenderDrawColor(renderer,255,255,0,255);
     SDL_RenderFillRect(renderer,&bird_rect);

     //Draw scorecard
     char scoreText[16];
     sprintf(scoreText,"%d",score); // convert int - string
     SDL_Color white = {255,255,255,255};
     drawText(renderer,scoreText,SCREEN_WIDTH/2, 30, white,60);

     //show the result
     SDL_RenderPresent(renderer);
     SDL_Delay(16);//60 fps
   }
   //Cleanup
   if(font) TTF_CloseFont(font);
   TTF_Quit;
   SDL_DestroyRenderer(renderer);
   SDL_DestroyWindow(window);
   SDL_Quit(); 

}