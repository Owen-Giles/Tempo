//gcc mp3App.c getSongs.c -o mp3PlayerApp $(pkg-config --libs --cflags raylib)
#include "raylib.h"
#include <stdlib.h>
#include "getSongs.h"
#include <string.h>
#include <stdio.h>

#define APP_HEIGHT 350
#define APP_WIDTH  250
#define BG_COLOR (Color){30, 30, 30, 255}

void drawAppBackground(Rectangle appWindow, Rectangle coverArt);

void windowBarToggle();

bool pressButton(Rectangle button, Texture2D buttonArt, Texture2D pressedButtonArt);

void playNextSong(Music *music, char songsList[50][33], int songsFound,
                   int previousSongsIndex[5], int *randSongIndex);

void spiningRecord(Texture2D disk, bool pause, Rectangle area, Texture2D needle);

int main()
{
   // Removes window bar and hides real window
   SetConfigFlags(FLAG_WINDOW_TRANSPARENT);
   SetWindowState(FLAG_WINDOW_UNDECORATED);

   InitWindow(APP_WIDTH, APP_HEIGHT ,"mp3 OutPut");
   SetAudioStreamBufferSizeDefault(4096);
   InitAudioDevice();
   SetExitKey(KEY_NULL);
   SetTargetFPS(30);
   
   char      songsList[50][33],
             path[50];
   int       previousSongsIndex[5]   = {0},
             songsFound              = 0,
             index                   = 0,
             randSongIndex           = 0;
   float     timePlayed              = 0.0f,
             musicVolume             = 1.0f;
   bool      pause                   = false,
             songFinished            = false, 
             topMostLayer            = false;
   Rectangle appWindow               = {5, 2, APP_WIDTH - 10, APP_HEIGHT - 5},
             forwardSongButton       = {175, 270, 40, 40},
             previousSongButton      = {35, 270, 40, 40},
             pausePlayButton         = {100, 265, 50, 50},
             coverArt                = {55, 50, 140, 140},
             settingsButtonRec       = {215, 10, 20, 20},
             volumeUp                = {7, 70, 10, 25},
             volumeDown              = {7, 110, 10, 25};
   Music     songPlaying             = LoadMusicStream(path);
             songPlaying.looping     = false;
   Sound     clickButton             = LoadSound("clickSound.mp3");
   Texture2D forwardTrack            = LoadTexture("textures/forwardTrack.png"),
             pressedForwardTrack     = LoadTexture("textures/pressedForwardTrack.png"),
             backTrack               = LoadTexture("textures/backTrack.png"),
             pressedBackTrack        = LoadTexture("textures/pressedBackTrack.png"),
             playButton              = LoadTexture("textures/playButton.png"),
             pauseButton             = LoadTexture("textures/pauseButton.png"),
             pressedPlayButton       = LoadTexture("textures/pressedPlayButton.png"),
             pressedPauseButton      = LoadTexture("textures/pressedPauseButton.png"),
             settingsButton          = LoadTexture("textures/settingsButton.png"),
             pressedSettingsButton   = LoadTexture("textures/settingsButtonPressed.png"),
             recordDisk              = LoadTexture("textures/disk.png"),
             needle                  = LoadTexture("textures/needle.png"),
             volumeUpButton          = LoadTexture("textures/volumeUp.png"),
             volumeDownButton        = LoadTexture("textures/volumeDown.png"),
             pressedVolumeUpButton   = LoadTexture("textures/pressedVolumeUp.png"),
             pressedVolumeDownButton = LoadTexture("textures/pressedVolumeDown.png"),
             icon,
             pressedIcon;

   songsFound = getSongs(songsList);

   // Determines the random song
   randSongIndex = rand() % songsFound;
   srand(randSongIndex);
   snprintf(path, sizeof(path), "cSongs/%s", songsList[randSongIndex]);

   PlayMusicStream(songPlaying);

   // Main loop
   while(!WindowShouldClose())
   {
      UpdateMusicStream(songPlaying);

      BeginDrawing();
      ClearBackground(BLANK);

      // Window bar logic
      windowBarToggle();

      // Draws the bg
      drawAppBackground(appWindow, coverArt);

      //Volume buttons
      if(pressButton(volumeUp, volumeUpButton, pressedVolumeUpButton))
      {
         PlaySound(clickButton);

         musicVolume += 0.2f;
         if(musicVolume > 1.0f)
         {
            musicVolume = 1.0f;
         }
         SetMasterVolume(musicVolume);
      }
      if(pressButton(volumeDown, volumeDownButton, pressedVolumeDownButton))
      {
         PlaySound(clickButton);

         musicVolume -= 0.2f;
         if(musicVolume < 0.0f)
         {
            musicVolume = 0.0f;
         }
         SetMasterVolume(musicVolume);
      }

      // Settings button logic (topmost application)
      if(pressButton(settingsButtonRec, settingsButton, pressedSettingsButton))
      {
         PlaySound(clickButton);
         
         topMostLayer = !topMostLayer;

         if(topMostLayer)
         {
            SetWindowState(FLAG_WINDOW_TOPMOST);
         }
         else
         {
            ClearWindowState(FLAG_WINDOW_TOPMOST);
            DrawText("Topmost: off", (appWindow.width - MeasureText("Topmost: off", 5)) /
            2, 20, 5, BLACK);
         }
      }

      // Lets user know what mode they are in
      if(topMostLayer)
      {
         DrawText("Topmost: on", (appWindow.width - MeasureText("Topmost: on", 5)) / 2, 20, 5, BLACK);
      }

      // Draws text of the song and centers it
      DrawText(TextFormat("%s", songsList[randSongIndex]), appWindow.x + (appWindow.width - 
         MeasureText(songsList[randSongIndex], 20)) / 2, 200, 20, DARKGRAY);

      // Pause music logic
      icon        = pause ? playButton : pauseButton;
      pressedIcon = pause ? pressedPlayButton : pressedPauseButton;
      if(IsKeyPressed(KEY_SPACE) || pressButton(pausePlayButton, icon, pressedIcon))
      {
         PlaySound(clickButton);
         
         pause = !pause;
      
         if(pause)
         {
            PauseMusicStream(songPlaying);
         }
         else
         {
            ResumeMusicStream(songPlaying);
         }
      }

      // Determines if music is done playing
      songFinished = !pause && !IsMusicStreamPlaying(songPlaying);

      // Plays another song when finished
      if(songFinished)
      {
         playNextSong(&songPlaying, songsList, songsFound, previousSongsIndex, &randSongIndex);
      }

      // Skip music logic
      if(IsKeyPressed(KEY_RIGHT) || pressButton(forwardSongButton, forwardTrack, pressedForwardTrack))
      {
         PlaySound(clickButton);

         playNextSong(&songPlaying, songsList, songsFound, previousSongsIndex, &randSongIndex);
      }

      // Previous music logic
      if(IsKeyPressed(KEY_LEFT) || pressButton(previousSongButton, backTrack, pressedBackTrack))
      {
         PlaySound(clickButton);

         randSongIndex = previousSongsIndex[index];
         index++;

         // Resets so you only see most recent 5 repeated
         if(index >= 5 || index < 0)
         {
            index = 0;
         }

         UnloadMusicStream(songPlaying);
         snprintf(path, sizeof(path), "cSongs/%s", songsList[randSongIndex]);
         songPlaying = LoadMusicStream(path);
         songPlaying.looping = false;
         PlayMusicStream(songPlaying);

         if(pause)
         {
            pause = !pause;
         }
      }

      // Minimize window logic
      if(IsKeyPressed(KEY_ESCAPE))
      {
         MinimizeWindow();
      }

      spiningRecord(recordDisk, pause, coverArt, needle);

      // Time bar logic
      timePlayed = GetMusicTimePlayed(songPlaying) / GetMusicTimeLength(songPlaying);
      DrawRectangle(35, 245, 180, 10, LIGHTGRAY);
      DrawRectangle(35, 245, (int)(timePlayed *180.0f), 10, DARKGRAY);
      DrawRectangleLines(35, 245, 180, 10, GRAY); 

      EndDrawing();  
   }

   // Clean up and close
   UnloadTexture(forwardTrack);
   UnloadTexture(pressedForwardTrack);
   UnloadTexture(backTrack);
   UnloadTexture(pressedBackTrack);
   UnloadTexture(playButton);
   UnloadTexture(pauseButton);
   UnloadTexture(pressedPlayButton);
   UnloadTexture(pressedPauseButton);
   UnloadSound(clickButton);
   UnloadMusicStream(songPlaying);
   CloseAudioDevice();
   CloseWindow();
   return 0;
}

/******************************************************************************
* Draws the main bg and cover art for the invisible app.
******************************************************************************/
void drawAppBackground(Rectangle appWindow, Rectangle coverArt)
{
   DrawRectangleRounded(appWindow, .25f, 64, WHITE);
   DrawRectangleRoundedLinesEx(appWindow, .25f, 64, 2.0f, BLACK);
   DrawRectangleRounded(coverArt, .25f, 64, LIGHTGRAY);
   DrawRectangleRoundedLinesEx(coverArt, .25f, 64, 4.0f, BLACK);
   return;
}

/******************************************************************************
* Logic for app bar.
******************************************************************************/
void windowBarToggle()
{
   Vector2 mouseTracker = GetMousePosition();
   Rectangle grayBackground = {0, - 35, APP_WIDTH, APP_HEIGHT + 35};
   bool isHovering = (mouseTracker.x >= 0 && mouseTracker.x <301 && mouseTracker.y < 0 && mouseTracker.y > -35);

   //Detects when to show window bar
   if(isHovering && IsWindowState(FLAG_WINDOW_UNDECORATED))
   {
      ClearWindowState(FLAG_WINDOW_UNDECORATED);
   } 
   //Detects when to show window bar
   else if(!isHovering && !IsWindowState(FLAG_WINDOW_UNDECORATED))
   {
      SetWindowState(FLAG_WINDOW_UNDECORATED);
   }

   //Draws bg shade color
   if(!IsWindowState(FLAG_WINDOW_UNDECORATED))
   {
      DrawRectangleRounded(grayBackground, .25f, 64, BG_COLOR);
   }

   return;
}

/******************************************************************************
* Logic for app buttons.
******************************************************************************/
bool pressButton(Rectangle button, Texture2D buttonArt, Texture2D pressedButtonArt)
{
   bool hovering = CheckCollisionPointRec(GetMousePosition(), button);

   //
   DrawRectangleRec(button, BLANK);
   DrawTexture(hovering ? pressedButtonArt : buttonArt, button.x, button.y, WHITE);

   return hovering && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

/******************************************************************************
* Logic for picking the next song.
******************************************************************************/
void playNextSong(Music *music, char songsList[50][33], int songsFound,
                   int previousSongsIndex[5], int *randSongIndex)
{
   *randSongIndex = rand() % songsFound;

   for(int i = 4; i > 0; i--)
   {
      previousSongsIndex[i] = previousSongsIndex[i - 1];
   }
   previousSongsIndex[0] = *randSongIndex;

   UnloadMusicStream(*music);
   char path[50];
   snprintf(path, sizeof(path), "cSongs/%s", songsList[*randSongIndex]);
   *music = LoadMusicStream(path);
   (*music).looping = false;
   PlayMusicStream(*music);
}

/******************************************************************************
* Draws the spining disk for cover art.
******************************************************************************/
void spiningRecord(Texture2D disk, bool pause, Rectangle area, Texture2D needle)
{
   static float rotation = 0.0f;

   Rectangle source = { 0, 0, (float)disk.width, (float)disk.height };
   Rectangle dest    = { area.x + area.width / 2.0f, area.y + area.height / 2.0f, area.width, area.height };
   Vector2 origin    = { area.width / 2.0f, area.height / 2.0f };

   if(!pause)
   {
      rotation += 1.5f;

      if (rotation > 360.0f) 
      {
         rotation -= 360.0f;
      }
   }

   DrawTexturePro(disk, source, dest, origin, rotation, WHITE);
   DrawTexture(needle, 55, 50, WHITE);

   return;
}


//THIS IS FOR THE NEXT AND PREV SONG

// store in array and fix that bug
//for next song 
// if index ! 0 
//    index ++
// else rand

// i think this will work probably 