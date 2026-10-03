//Add notes
#include <stdio.h>
#include <string.h>
#include <dirent.h>

#include "getSongs.h"

int getSongs(char songList[50][33])
{
   struct dirent *directoryEntry;
   DIR  *directory = opendir("cSongs");
   char *extensionMP3;
   int   index = 0;

   // Checks if the folder exists
   if (directory == NULL)
   {
      printf("ERROR: Could not find directory for cSongs\n");
      return 1;
   }

   // Reads directory until the end of directory
   while ((directoryEntry = readdir(directory)) != NULL)
   {
      extensionMP3 = strrchr(directoryEntry->d_name, '.');

      // Checks if it is the right file type
      if (extensionMP3 && (strcmp(extensionMP3, ".mp3") == 0 || strcmp(extensionMP3, ".wav") == 0 || strcmp(extensionMP3, ".ogg") == 0))
      {
         // Only gets 50 files
         if(index < 50)
         {
            // Copies file lists into array
         	strncpy(songList[index], directoryEntry->d_name, 32);
            songList[index][32] = '\0';
            printf("%s\n", songList[index]);
                
            index++;
         }
      }
   }
   
   printf("NOTE: %d Songs Found\n", index);
   closedir(directory);
   return index;
}