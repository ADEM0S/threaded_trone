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
#include "include/queue.h"

// #include "include/gpio_utils.h"
#include "include/events_handler.h"
#include "include/load_image.h"


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


  dyn_array_t* frames_list = init_dyn_array(sizeof(struct QUEUE_ELEMENT_STRUCT)); // list of 10 frames
  dyn_array_t* events_list = init_dyn_array(sizeof(EVENT_STRUCT)); // list of 10 frames

  // Init the 2 threads
  pthread_mutex_t frames_mutex;
  pthread_mutex_init(&frames_mutex, NULL);

  pthread_mutex_t events_mutex;
  pthread_mutex_init(&events_mutex, NULL);

  printf("MUTEXES INITED\n");

  // init the frame, the capture ? => cap changed by the gpio_states
  ld_arg_t ld_arg;
  ld_arg.frames_list = frames_list;
  ld_arg.events = events_list;
  ld_arg.loop_video = loop_video;
  ld_arg.frames_mutex = &frames_mutex;
  ld_arg.events_mutex = &events_mutex;
  ld_arg.videos_list = vids_list_ptr;

  pthread_t image_load_thread;
  pthread_create(&image_load_thread, NULL, &load_handler, (void*)&ld_arg);



  events_arg_t ev_arg;

  ev_arg.buzz_led_pin = LED;
  ev_arg.buzz_pin = BUZZER;
  ev_arg.white_pin = WHITE_BUTTON;
  ev_arg.yellow_pin = YELLOW_BUTTON;
  ev_arg.red_pin = RED_BUTTON;
  ev_arg.blue_pin = BLUE_BUTTON;
  ev_arg.green_pin = GREEN_BUTTON;

  ev_arg.events_mutex = &events_mutex;
  ev_arg.events = events_list;


  pthread_t events_thread;
  pthread_create(&events_thread, NULL, &events_handler, (void*)&ev_arg);

  usleep(100000);
  printf("BEFORE GUI INIT\n");
  namedWindow(MAIN_WIN_NAME, WINDOW_NORMAL);
  printf("IN GUI INIT\n");
  setWindowProperty(MAIN_WIN_NAME, WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN); // WINDOW_FULLSCREEN
  printf("GONNA ENTER THE LOOP\n");
  while (1)
  {
    usleep(100);
    if (DEBUG) printf("show_image thread locks the mutex.\n");

    pthread_mutex_lock(&frames_mutex);
    if (frames_list->length <= 0)
    {
      if (DEBUG) printf("show_image thread unlocks the mutex from empty frames list.\n");
      pthread_mutex_unlock(&frames_mutex);
      continue;
    }


    queue_element_t* queued_frame = (queue_element_t*) extract_first(frames_list);
    pthread_mutex_unlock(&frames_mutex);
    if (DEBUG) printf("show_image thread unlocks the mutex from normal end.\n");

    if (DEBUG) printf("IMSHOW BEFORE\n");
    imshow(MAIN_WIN_NAME, queued_frame->frame);
    imwrite("current img.jpg", queued_frame->frame);
    if (DEBUG) imwrite("current img.jpg", queued_frame->frame);
    if (DEBUG) printf("IMSHOW AFTER\n");
    (queued_frame->frame).release();

    printf("MTTS = %f\n", queued_frame->mtts);
    usleep(queued_frame->mtts);
    free(queued_frame);
  }

  printf("\n\n=========================\nEND OF THE GIFFER PROGRAM\n=========================\n\n");

  return EXIT_SUCCESS;
}
