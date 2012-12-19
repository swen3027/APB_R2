#include "tinysh.h"


void tinysh_char_out(unsigned char c)
{
	UART0_Sendchar(c);
}

static void display_args(int argc, char **argv)
{
  int i;
  for(i=0;i<argc;i++)
    {
      printf("argv[%d]=\"%s\"\n\r",i,argv[i]);
    }
}

static void foo_fnt(int argc, char **argv)
{
  printf("foo command called\n\r");
  display_args(argc,argv);
}

static tinysh_cmd_t myfoocmd={0,"foo","foo command","[args]",
                              foo_fnt,0,0,0};

static void item_fnt(int argc, char **argv)
{
  printf("item%d command called\n\r",(int)tinysh_get_arg());
  display_args(argc,argv);
}

static tinysh_cmd_t ctxcmd={0,"ctx","contextual command","item1|item2",
                            0,0,0,0};
static tinysh_cmd_t item1={&ctxcmd,"item1","first item","[args]",item_fnt,
                           (void *)1,0,0};
static tinysh_cmd_t item2={&ctxcmd,"item2","second item","[args]",item_fnt,
                           (void *)2,0,0};

static void reset_to_0(int argc, char **argv)
{
  int *val;
  val=(int *)tinysh_get_arg();
  *val=0;
}

static void atoxi_fnt(int argc, char **argv)
{
  int i;

  for(i=1;i<argc;i++)
    {
      printf("\"%s\"-->%u (0x%x)\n\r",
             argv[i],tinysh_atoxi(argv[i]),tinysh_atoxi(argv[i]));
    }
}

static tinysh_cmd_t atoxi_cmd={0,"atoxi","demonstrate atoxi support","[args-to-convert]",atoxi_fnt,0,0,0};

void vTinyshTask(void * pvParameters)
{
	int again = 1;
	unsigned char c;
	tinysh_set_prompt(":>");
	tinysh_add_command(&myfoocmd);
	tinysh_add_command(&ctxcmd);
	tinysh_add_command(&item1);
	tinysh_add_command(&item2);
	{
	tinysh_cmd_t quitcmd={0, "quit", "exit shell", 0, reset_to_0, (void *) &again,0,0};	
	tinysh_add_command(&quitcmd);
	}

	
	while(again)
	{
		c=UART0_Getchar();
		tinysh_char_in((unsigned char) c);
		
	}
	printf("\n\rTinysh exiting\n\r");
	while(1);
}
