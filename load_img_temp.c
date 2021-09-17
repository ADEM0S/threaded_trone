#define MAX_IMGS_LENGTH 5

typedef struct LOAD_ARG_STRUCT
{
  dyn_array_t* frames_list;
  dyn_array_t* events;
  video_t* gif_video;
  video_t* loop_video;
  pthread_mutex_t* frames_mutex;

} ld_arg_t;

void *load_handler(void *arg)
{
  ld_arg_t* ld_arg = (ld_arg_t*) arg;

  printf("=======================\nLOAD IMAGE THREAD SETUP\n=======================\n\n");
  printf("- loop video path : \"%s\"\n", ld_arg->loop_video->path);
  printf("- gif video path : \"%s\"\n\n", ld_arg->gif_video->path);



  int f_count = 0;
  int loop = 1;


  while (loop)
  {
    VideoCapture main_cap(ld_arg->loop_video->path);
    if (!main_cap.isOpened()) break;

    while (1)
    {
      Mat *frame_ptr = (Mat*) calloc(1, sizeof(Mat)); // => append to frames_list

      main_cap >> *frame_ptr;

      if (ld_arg->frames_list->length <= MAX_IMGS_LENGTH)
      {
        if (DEBUG) printf("load_image locks the mutex\n");

        pthread_mutex_lock(ld_arg->frames_mutex);

        if (DEBUG) printf("appending frame -> length : %ld\n", ld_arg->frames_list->length);
        dyn_array_append(ld_arg->frames_list, (void*) frame_ptr);

        if (DEBUG) printf("load_image unlocks the mutex\n\n");
        pthread_mutex_unlock(ld_arg->frames_mutex);

        f_count++;

        if (f_count >= (int)main_cap.get(CAP_PROP_FRAME_COUNT)) // replay the loop
        {
          f_count = 0;
          main_cap.set(CAP_PROP_POS_FRAMES, f_count);
        }

        // // GIF
        // if (ld_arg->events->length != 0) // an event
        // {
        //   //mutex lock the gif_video
        //   VideoCapture gif_cap(ld_arg->gif_video->path);
        //   Mat* g_frame_ptr;
        //   int is_gif_loaded = 0;
        //
        //   while(1)
        //   {
        //     // usleep(SOME_TIME);
        //     if (!is_gif_loaded)
        //     {
        //       Mat frame;
        //       gif_cap >> frame;
        //       g_frame_ptr = &frame;
        //       is_gif_loaded = true;
        //     }
        //
        //     if (ld_arg->frames_list->length <= MAX_IMGS_LENGTH)
        //     {
        //       printf("LAUNCHING A GIF\n");
        //       pthread_mutex_lock(ld_arg->frames_mutex);
        //       dyn_array_append(ld_arg->frames_list, (void*)g_frame_ptr);
        //       pthread_mutex_unlock(ld_arg->frames_mutex);
        //       is_gif_loaded = false;
        //     }
        //   }
        // }
      }
    }
  }


  return NULL;
}
