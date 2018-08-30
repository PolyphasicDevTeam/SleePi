#include <boost/python.hpp>
#include <iostream>
#include <stdio.h>
#include <fcntl.h>  /* File Control Definitions          */
#include <termios.h>/* POSIX Terminal Control Definitions*/
#include <unistd.h> /* UNIX Standard Definitions         */
#include <errno.h>  /* ERROR Number Definitions          */
#include <fstream>

#define MAX_EEG_CHANNELS 6

typedef struct PACKETStruct
{
	unsigned char	  readstate;
	unsigned int 	  extract_pos;
	unsigned int 	  info;
	unsigned int 	  requestedinfo;
	unsigned int 	  chn;
	unsigned char     number;
	unsigned char     old_number;
	unsigned char     switches;
	unsigned char     aux;
	unsigned int      buffer[MAX_EEG_CHANNELS*2];
	unsigned int      tempbuf[MAX_EEG_CHANNELS*2];
} PACKETStruct;


bool parse_byte_P2(unsigned char actbyte);
PACKETStruct PACKET;
int fd;

int eeg_init()
{
	fd = open("/dev/ttyUSB0",O_RDWR | O_NOCTTY);
	if(fd == 1)
	{
		printf("\n  Error! in Opening ttyUSB0\n");
		return 1;
	}
	else
	{
		printf("\n  ttyUSB0 Opened Successfully\n");
	}


	struct termios tty;
	tcgetattr(fd,&tty);
	speed_t ispeed = cfgetispeed(&tty);
	speed_t ospeed = cfgetospeed(&tty);
	printf("baud rate in: 0%o\n", ispeed);
	printf("baud rate out: 0%o\n", ospeed);
	//tcgetattr(fd, &SerialPortSettings);
	//cfsetispeed(&SerialPortSettings,B57600);
	//cfsetospeed(&SerialPortSettings,B57600);
	//SerialPortSettings.c_cflag |= CREAD | CLOCAL;
	//SerialPortSettings.c_cc[VMIN]  = 10; // Read 10 characters
	//SerialPortSettings.c_cc[VTIME] = 0;  // Wait indefinitely

	//tcsetattr(fd,TCSANOW,&SerialPortSettings);


	return 0;
}

int eeg_destroy()
{
	close(fd);
	return 0;
}


int ch_val[MAX_EEG_CHANNELS];
std::string eeg_get()
{
	std::string ret = "";
	unsigned char read_buffer[512];                
	int  bytes_read = 0;                 
	bytes_read = read(fd,&read_buffer,512);
	for (size_t i = 0; i < bytes_read; ++i)
	{
		unsigned int tmp = read_buffer[i];
		//std::cout << tmp << std::endl;
		if(parse_byte_P2(read_buffer[i]))
		{
			for (size_t i = 0; i < MAX_EEG_CHANNELS; ++i) 
			{
				if (i > 0) { ret += " "; }
				ret += std::to_string(ch_val[i]);
			}
			ret += "X";
		}

	}
	return ret;
}

bool parse_byte_P2(unsigned char actbyte)
{
	bool ret = 0;
	switch (PACKET.readstate) 
	{
		case 0: if (actbyte==165) PACKET.readstate++; 
					  break;
		case 1: if (actbyte==90)  PACKET.readstate++; 
					  else PACKET.readstate=0;
					  break;
		case 2: PACKET.readstate++; 
				  break;
		case 3: PACKET.number = actbyte;
				  //std::cout << "Frame number " << PACKET.number << std::endl;
				  PACKET.extract_pos=0;PACKET.readstate++;
				  break;
		case 4: 
				  if (PACKET.extract_pos < 12)
				  {
					  if ((PACKET.extract_pos & 1) == 0)
					  {
						  PACKET.buffer[PACKET.extract_pos>>1]=actbyte*256;
					  }
					  else 
					  {
						  PACKET.buffer[PACKET.extract_pos>>1]+=actbyte;
					  }
					  PACKET.extract_pos++;
				  }
				  else
				  {
					  for (size_t i = 0; i < MAX_EEG_CHANNELS; ++i)
					  {
						  ch_val[i] = PACKET.buffer[i];
						  //std::cout << ch_val[i] << std::endl;
					  }
					  ret = 1;
					  PACKET.switches= actbyte;
					  PACKET.readstate=0;
				  }
				  break;
		default: PACKET.readstate=0;
	}		
	return ret;
}

BOOST_PYTHON_MODULE(openeeg)
{
	using namespace boost::python;
	def("eeg_init",eeg_init);
	def("eeg_destroy",eeg_destroy);
	def("eeg_get",eeg_get);
}
