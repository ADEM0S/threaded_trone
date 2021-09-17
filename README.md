# Trone

pigpio documentation
https://abyz.me.uk/rpi/pigpio/cif.html

Juste trop swag enft


# TODO LIST

- EVENTS: if "some precised" button pushed -> random_video ?(path)? -> fire event {video_path} -> change for other list name and variables names : 'events' to strings 'paths' -> load_image enters in the if() + while(1) for the specific random video

- IGNORED FOR THE MOMENT /!\\ Multithreaded program for better efficiency
  ==>  Problem with showing the frames in the dynamic array... frame pointers are working...


- Rajouter des videos pour un max de plaisir
- Savoir quel bouton va lancer quel video-gif
- Tester la partie GPIO


# Multithreading

- GPIO_thread:

  using gpio events

  usigned event range = 0->31

  typedef void (*eventFunc_t) (int event, uint32_t tick);

  int eventTrigger(unsigned event)
    fires a custom event

  int eventSetFunc(unsigned event, eventFunc_t f)
    executes a callback for a certain event


  while (1)
  usleep(SOME_SAMPLE_PERIOD);

  if ( pigpioRead(buzz_pin) ) // if up
