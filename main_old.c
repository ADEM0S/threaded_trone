#define DEBUG 1


#include <opencv4/opencv2/highgui/highgui.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <iostream>
#include <string>
#include <unistd.h>
#include <cerrno>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

// Raspberry
// #include <wiringPi.h>

using namespace std;
using namespace cv;

#include "include/utils.h"
#include "include/video.h"
#include "include/pins.h"
#include "include/dyn_array.h"
#include "include/audio_manipulation.h"
#include "include/video_manipulation.h"
#include "include/events.h"

// #include "include/gpio_utils.h"
#include "include/load_image.h"
#include "include/show_image.h"


// PINS
#define LED 1
#define BUZZER 2
#define WHITE_BUTTON 3
#define YELLOW_BUTTON 4
#define RED_BUTTON 5
#define BLUE_BUTTON 6
#define GREEN_BUTTON 7

#define INPUT 0
#define OUTPUT 1


uint8_t GIF_LENGTH = 5;


int main(int argc, char **argv)
{
  printf("\n===============================\nBEGINNING OF THE GIFFER PROGRAM\n===============================\n\n");


  // pinMode(LED, OUTPUT);
  // pinMode(BUZZER, INPUT);
  // pinMode(WHITE_BUTTON, INPUT);
  // pinMode(YELLOW_BUTTON, INPUT);
  // pinMode(RED_BUTTON, INPUT);
  // pinMode(BLUE_BUTTON, INPUT);
  // pinMode(GREEN_BUTTON, INPUT);

  int result = 0;
  int flags = MIX_INIT_MP3;

  if (SDL_Init(SDL_INIT_AUDIO) < 0) {
    printf("Failed to init SDL\n");
    exit(1);
  }

  if (flags != (result = Mix_Init(flags))) {
    printf("Could not initialize mixer (result: %d).\n", result);
    printf("Mix_Init: %s\n", Mix_GetError());
    exit(1);
  }

  Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, AUDIO_S16SYS, 2, 1024);


  dyn_array_t* vids_list_ptr = init_dyn_array(sizeof(struct VIDEO_STRUCT));
  read_structs(vids_list_ptr);

  video_t *loop_video = get_main_loop_video(vids_list_ptr);

  if (loop_video == NULL)
  {
    printf("loop_video is null.\n");
    return EXIT_FAILURE;
  }

  if (DEBUG) printf("====================\nloop video\n====================\n\n");
  if (DEBUG) printf(" - \"%s\" ; \"%s\" ; %d ; %d \n\n\n", loop_video->type, loop_video->path, loop_video->has_audio, loop_video->is_hd);

  video_t *gif_video = init_video();

  dyn_array_t* frames_list = init_dyn_array(sizeof(Mat)); // list of 10 frames
  dyn_array_t* events_list = init_dyn_array(sizeof(EVENT_STRUCT)); // list of 10 frames

  // Init the 3 threads
  pthread_mutex_t frames_mutex;
  pthread_mutex_unlock(&frames_mutex); // inited to locked ....

  // init the frame, the capture ? => cap changed by the gpio_states
  ld_arg_t ld_arg;
  ld_arg.frames_list = frames_list;
  ld_arg.events = events_list;
  ld_arg.loop_video = loop_video;
  ld_arg.gif_video = gif_video;
  ld_arg.frames_mutex = &frames_mutex;

  pthread_t image_load_thread;
  pthread_create(&image_load_thread, NULL, &load_handler, (void*)&ld_arg);

  usleep(100000);

  shw_arg_t shw_arg;
  shw_arg.frames_list = frames_list;
  shw_arg.frames_mutex = &frames_mutex;
  shw_arg.window_name = MAIN_WIN_NAME;


  namedWindow(shw_arg.window_name, WINDOW_NORMAL);
  setWindowProperty(shw_arg.window_name, WND_PROP_FULLSCREEN, CAP_PROP_FORMAT);
  setWindowProperty(shw_arg.window_name, WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN); // WINDOW_FULLSCREEN


  pthread_t image_show_thread;
  pthread_create(&image_show_thread, NULL, &show_handler, (void*)&shw_arg);

  // gpio_states_t gpio_states;


  // pthread_t gpio_thread;
  // pthread_create(&gpio_thread, NULL, &gpio_handler, (void*)&gpio_states);


  // int buzz_led_state, buzz_state, white_state, yellow_state, red_state, blue_state, green_state;

  while (1)
  {
    sleep(1);
  }

  // getc(stdin);

  // int f_count = 0;
  //
  // int loop = 1;
  // //
  // // Video *loop_video = get_main_loop_video(vids_list_ptr);
  //
  // namedWindow(MAIN_WIN_NAME, WINDOW_NORMAL);
  // setWindowProperty(MAIN_WIN_NAME, WND_PROP_FULLSCREEN, CAP_PROP_FORMAT);
  // setWindowProperty(MAIN_WIN_NAME, WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN); // WINDOW_FULLSCREEN
  //
  // while (loop)
  // {
  //   VideoCapture cap(loop_video->path);
  //   if (!cap.isOpened()) break;
  //
  //   Mat frame;
  //
  //   while (1)
  //   {
  //     cap >> frame;
  //     f_count++;
  //     imshow(MAIN_WIN_NAME, frame);
  //
  //     if (f_count == (int)cap.get(CAP_PROP_FRAME_COUNT))
  //     {
  //       f_count = 0;
  //       cap.set(CAP_PROP_POS_FRAMES, f_count);
  //     }
  //
  //     handle_pins();
  //
  //     char c = (char) waitKey(50);
  //     if (c == 27)
  //     {
  //       loop = 0;
  //       break; // goes to the mainloop initiation
  //     }
  //
  //     if (c == 32) // TAB // EXAMPLE for the moment, must be locked on a button pin
  //     {
  //       char type[10];
  //       strcpy(type, "drink");
  //
  //       if (play_random_video(GIF_LENGTH, vids_list_ptr, type) == -1)
  //       {
  //         printf("Error playing random video.\n");
  //         return EXIT_FAILURE;
  //       }
  //     }
  //     if (c == 97) // 1
  //     {
  //       if (play_video( get_video_by_path(vids_list_ptr, "videos/point/point_pour_le_mamouth.mp4"), 0) == -1)
  //       {
  //         printf("Error playing random video.\n");
  //         return EXIT_FAILURE;
  //       }
  //     }
  //     if (c == 98) // 0
  //     {
  //       char type[15];
  //       strcpy(type, "miscelleanous");
  //       if (play_random_video(0, vids_list_ptr, type) == -1)
  //       {
  //         printf("Error playing random video.\n");
  //         return EXIT_FAILURE;
  //       }
  //     }
  //   }
  //   frame.release();
  //   cap.release();
  // }
  //
  // destroyAllWindows();
  // free_array(vids_list_ptr);
  //
  // SDL_Quit();
  //
  // dyn_array_free(frames_list, free_mat);
  // dyn_array_free(events_list, free_event);

  // pin_close(LED);
  // pin_close(BUZZER);
  // pin_close(WHITE_BUTTON);
  // pin_close(YELLOW_BUTTON);
  // pin_close(RED_BUTTON);
  // pin_close(BLUE_BUTTON);
  // pin_close(GREEN_BUTTON);

  printf("\n\n=========================\nEND OF THE GIFFER PROGRAM\n=========================\n\n");

  return EXIT_SUCCESS;
}
