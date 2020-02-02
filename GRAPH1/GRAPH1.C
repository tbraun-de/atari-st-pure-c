#include <stdio.h>
#include <stdlib.h>
#include <tos.h>
#include <errno.h>
#include <vdi.h>
#include <aes.h>

int contrl[12], intin[128], ptsin[128], intout[128], ptsout[128];
int x_size, y_size, max_intensity;
int intensity, real_intensity;
int handle;
static int oldcolors[16][3];
static int color_mode;

short physscr;
void *graphscr;

#define TEXT 0
#define GRAPH 1
static short logscr = TEXT;
static void *textscr;
void *screenmap;

static void showscreen(register int a) {
  if (a == physscr) return;
  if (a == GRAPH) Setscreen((void *)-1L, graphscr, -1);
  else if (a == TEXT) Setscreen((void *)-1L, textscr, -1);
  else return;
  physscr = a;
}

static void usescreen(register int a) {
  if (a == logscr) return;
  if (a == GRAPH) Setscreen(graphscr, (void *)-1L, -1);
  else if (a == TEXT) Setscreen(textscr, (void *)-1L, -1);
  else return;
  logscr = 1;
}

void exit_graphics(char *s) 
{
  showscreen(TEXT);
  usescreen(TEXT);
  if (s) printf("%s", s);
  v_exit_cur(handle);
  v_clsvwk(handle);
  Cursconf(1, 0);
}

void punt(char *s) 
{
  exit_graphics(s);
  exit(1);
}


void clear() 
{
  register int i;
  register long *p;
  p = (long *) graphscr;
  for (i=8000; i; --i) *(p++) = 0xFFFFFFFF;
}

void init_graphics(int req_mode)
{
  int dummy;
  int work_in[11], work_out[57], rgb_in[3];
  register short i;

  textscr = Physbase();
  if ((screenmap = malloc((int)65535)) == NULL) {
    printf("Couldn't allocate memory for new screen.");
    exit(1);
  }
  graphscr = (void *)((long)screenmap & 0x8000L + 32768L);
  if(appl_init() < 0) {
    printf("Couldn't initialize application!\n");
    exit(1);
  }
  handle = graf_handle(&dummy, &dummy, &dummy, &dummy);
  for (i = 0; i<10; i++) work_in[i] = 1;
  work_in[10] = 2;
  v_opnvwk(work_in, &handle, work_out);

  x_size = work_out[0]+1;
  y_size = work_out[1]+1;
  v_enter_cur(handle);
  v_hide_c(handle);
  intensity = -1;
  color_mode = req_mode;
  if(x_size == 640) {
    vsl_type(handle, (short) 7);
  } else {
    punt("need monochrome mode!");
  }
  showscreen(GRAPH);
  clear();
}


int main(int args, char *argv[], char *envp[]) {   
  init_graphics(1);
  gemdos(0x1); 
  exit_graphics(NULL);
  return 0;
}