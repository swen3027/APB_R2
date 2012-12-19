#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"

#include "tinysh.h"
#include "shell_commands.h"



#include "efs.h"
#include "mkfs.h"
#include "ls.h"
extern EmbeddedFileSystem efs;
extern EmbeddedFile filer, filew;
extern DirList list;
void foo_fnt(int argc, char **argv)
{
  MESSAGE("foo command called\n\r");
  display_args(argc,argv);
}
/* string to decimal/hexadecimal conversion
 */
unsigned long tinysh_atoxi(char *s)
{
  int ishex=0;
  unsigned long res=0;

  if(*s==0) return 0;

  if(*s=='0' && *(s+1)=='x')
    {
      ishex=1;
      s+=2;
    }

  while(*s)
    {
      if(ishex)
	res*=16;
      else
	res*=10;

      if(*s>='0' && *s<='9')
	res+=*s-'0';
      else if(ishex && *s>='a' && *s<='f')
	res+=*s+10-'a';
      else if(ishex && *s>='A' && *s<='F')
	res+=*s+10-'A';
      else
	break;
      
      s++;
    }

  return res;
}
void atoxi_fnt(int argc, char **argv)
{
  int i;

  for(i=1;i<argc;i++)
    {
      MESSAGE("\"%s\"-->%u (0x%x)\n\r",
             argv[i],tinysh_atoxi(argv[i]),tinysh_atoxi(argv[i]));
    }
}

void ls_fnt(int argc, char** argv)
{
//	MESSAGE("Directory of root\n\r");
	if(argc != 2)
	{
		MESSAGE("usage: ls {path_name}\n\r");
		return;

	}
	MESSAGE("listing directory %s:\n\r", argv[1]);
	ls_openDir( &list, &(efs.myFs) , argv[1]);
	while ( ls_getNext( &list ) == 0 ) {
		list.currentEntry.FileName[LIST_MAXLENFILENAME-1] = '\0';
		MESSAGE( "%s ( %d bytes )\n\r", list.currentEntry.FileName,list.currentEntry.FileSize ) ;
		}

	
	return;
}
void mkdir_fnt(int argc, char** argv)
{
	char* filename;
	if(argc == 2)
	{
		filename = argv[1];
		MESSAGE("filename: %s\n\r", argv[1]);
		switch(mkdir(&(efs.myFs), filename))
		{
			case 0:
				MESSAGE("\n\r");
				return 0;
			case -1:
				MESSAGE("Directory already exists\n\r");
				return -1;
			case -2:
				MESSAGE("Parent directory doesn't exist\n\r");
				return -2;
			case -3:
				MESSAGE("No free space\n\r");
				return -3;

			default:
				MESSAGE("Unhandled exception from mkdir command\n\r");
				return 1;
		}
	}
	else
	{
		MESSAGE("usage: mkdir {filename}\n\r");

	}
	MESSAGE("read %d args\n\r", argc);
}	
void write_file_cli(int argc, char** argv)
{
	char* filename;
	char a,b,c;
	int quit =1;
	signed char retval;
	File output;
	MESSAGE("opening file\n\r");
	retval = file_fopen(&output, &(efs.myFs), argv[1], 'w');
	MESSAGE("opened file\n\r");
	switch(retval)
	{
		case -2: //File already exists, open it for appending
			MESSAGE("File already exists, append to it?(Y/N):");
			c = UART0_Getchar();
			if(c == 'Y' || c == 'y')
			{
				if(file_fopen(&output, &(efs.myFs), argv[1], 'a') == -3)
				{
					MESSAGE("No more space!\n\r");
					return;
				}
				else
				{
					MESSAGE("appended to %s\n\r", argv[1]);
				}
			
			}
			else
				return;
			break;
		case -3:
			MESSAGE("No more space!\n\r");
			return;

		case 0:
			MESSAGE("file opened successfully\n\r");
			break;
		default:
			MESSAGE("unknown error occured\n\r");
	}
	MESSAGE("chars are being entered into the file until ~`! is recieved:\n\r");
	a = 0;
	b = 0;
	c = 0;
	
	while(quit)
	{
		a = UART0_Getchar();
		if(a == '~')
		{
			b = UART0_Getchar();
			if(b == '`')
			{
				c = UART0_Getchar();
				if(c == '!')
				{
					quit = 1;
					MESSAGE("quit recieved\n\r");
					file_fclose(&output);
					return;
				}
				else
				{
					file_write(&output, 1, &a);
					file_write(&output, 1, &b);
					file_write(&output, 1, &c);
				}
			}
			else
			{
				file_write(&output, 1, &a);
				file_write(&output, 1, &b);
			}
	
		}
		else
		{

			file_write(&output, 1, &a);

		}
		

	}

}
