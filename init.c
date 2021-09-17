#include <iostream>
#include <string.h>
#include "include/video.h"
#include "include/dyn_array.h"

using namespace std;

#define VIDEO_FORMAT_OUT "(%s, %s, %d, %d)\n"
#define LEN_FORMAT_OUT "length : %ld\n"
#define FFMPEG_CMD_FORMAT "ffmpeg -y -i %s.mp4 %s.mp3"
#define CMD_LENGTH (23 + 2*VIDEO_PATH_LENGTH)



int serialize(const char *path, dyn_array_t* videos_list);
int init_mp3(dyn_array_t* videos_list);

int main(int argc, char** argv)
{
  printf("Beginning of the process.\n");

  printf("\n======== Init of the GIFs structs ========\n");

  dyn_array_t* vids_list = init_dyn_array(sizeof(video_t));

  video_t* afond_jack = init_video("drink", "videos/random_gif/drink/afond_jack.mp4", 0, 0);
  dyn_array_append(vids_list, afond_jack);
  video_t* dancing_solo_girl = init_video("drink", "videos/random_gif/drink/dancing_solo_girl.mp4", 0, 0);
  dyn_array_append(vids_list, dancing_solo_girl);
  video_t* dicaprio = init_video("drink", "videos/random_gif/drink/dicaprio.mp4", 0, 0);
  dyn_array_append(vids_list, dicaprio);
  video_t* dj_khaled = init_video("drink", "videos/random_gif/drink/dj_khaled.mp4", 0, 0);
  dyn_array_append(vids_list, dj_khaled);
  video_t* jump_shot = init_video("drink", "videos/random_gif/drink/jump_shot.mp4", 0, 0);
  dyn_array_append(vids_list, jump_shot);
  video_t* lannister = init_video("drink", "videos/random_gif/drink/lannister.mp4", 0, 0);
  dyn_array_append(vids_list, lannister);
  video_t* laugh = init_video("drink", "videos/random_gif/drink/laugh.mp4", 0, 0);
  dyn_array_append(vids_list, laugh);
  video_t* random_anim = init_video("drink", "videos/random_gif/drink/random_anim.mp4", 0, 0);
  dyn_array_append(vids_list, random_anim);
  video_t* wine_girl = init_video("drink", "videos/random_gif/drink/wine_girl.mp4", 0, 0);
  dyn_array_append(vids_list, wine_girl);
  video_t* wwe = init_video("drink", "videos/random_gif/drink/WWE.mp4", 0, 0);
  dyn_array_append(vids_list, wwe);

  video_t* coffin = init_video("coffin", "videos/random_gif/coffin/minecraft_coffin.mp4", 0, 0);
  dyn_array_append(vids_list, coffin);

  video_t* mainloop = init_video("mainloop", "videos/mainloop.mp4", 0, 0);
  dyn_array_append(vids_list, mainloop);

  video_t* aglou = init_video("miscelleanous", "videos/vids/aglou.mp4", 1, 1);
  dyn_array_append(vids_list, aglou);
  video_t* baise = init_video("miscelleanous", "videos/vids/BAISE.mp4", 1, 1);
  dyn_array_append(vids_list, baise);
  video_t* raf = init_video("miscelleanous", "videos/vids/RienAFoutre.mp4", 1, 1);
  dyn_array_append(vids_list, raf);
  video_t* sasageyo = init_video("miscelleanous", "videos/vids/sasageyo.mp4", 1, 1);
  dyn_array_append(vids_list, sasageyo);

  video_t* point_pour_le_mamouth = init_video("point", "videos/point/point_pour_le_mamouth.mp4", 1, 0);
  dyn_array_append(vids_list, point_pour_le_mamouth);

  printf("\n=> OK\n");


  printf("\n======== Random drink GIFs Serialization ========\n");

  if (serialize("assets/video_structs.dat", vids_list) != 0)
  {
    printf("=> GONE WRONG !!\nExiting the program.\n");
    return 1;
  }
  printf("\n=> OK\n");


  printf("\n======== Init MP3s files ========\n");

  if (init_mp3(vids_list) != 0)
  {
    printf("=> GONE WRONG !!\nExiting the program.\n");
    return 1;
  }
  printf("\n=> OK\n");

  dyn_array_free(vids_list, free_video);

  printf("\n======== End of the process ========\n\n");
  return 0;
}

int serialize(const char *path, dyn_array_t* videos_list)
{

  FILE* fptr;
  fptr = fopen(path, "w");
  if (fptr == NULL)
  {
    printf("Error opening the file : %s.\n", path);
    return 0;
  }

  fprintf(fptr, LEN_FORMAT_OUT, videos_list->length);

  for (size_t i = 0; i < videos_list->length; i++)
  {
    video_t* video = (video_t*) videos_list->items[i];
    fprintf(fptr, VIDEO_FORMAT_OUT, video->type, video->path, video->has_audio, video->is_hd);
  }

  if (fclose(fptr) != 0)
  {
    printf("Error closing the file.\n");
    return 0;
  }

  return 0;
}

char *str_trim_extention(char *path)
{
  char *ret = (char*) malloc( (VIDEO_PATH_LENGTH+1) * sizeof(char) );

  int length = strlen(path) - 4;

  for (int i = 0; i < length; i++)
  {
    printf("%c", path[i]);
    ret[i] = path[i];
  }
  ret[length] = '\0';
  return ret;
}


int init_mp3(dyn_array_t* videos_list)
{
  for (size_t i = 0; i < videos_list->length; i++)
  {
    video_t* video = (video_t*) videos_list->items[i];

    if (video->has_audio == 1)
    {
      char *trimmed_path = str_trim_extention(video->path);

      char *cmd = (char *) malloc(CMD_LENGTH * sizeof(char));
      sprintf(cmd, FFMPEG_CMD_FORMAT, trimmed_path, trimmed_path);

      FILE *fp = popen(cmd, "r");
      if (fp == NULL)
      {
        printf("Error while popen.\n");
        return EXIT_FAILURE;
      }
      free(cmd);
      free(trimmed_path);
      fclose(fp);
    }
  }

  return 0;
}
