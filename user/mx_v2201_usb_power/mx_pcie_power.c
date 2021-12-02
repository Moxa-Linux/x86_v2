#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#define MY_MACIG 'G'
#define MOXA_PLED_MINOR 105

#define IOCTL_POWER_OFF _IOW(MOXA_PLED_MINOR,0,int)
#define IOCTL_POWER_ON  _IOW(MOXA_PLED_MINOR,1,int)


void usage (char *argv0){

	printf("Usage: %s [ -d || -e ] [-s slot]\n", argv0);
	printf("	-d	:	power off\n");
	printf("	-e	:	power on\n");
	printf("	slot	:	0 or 1\n");
	exit(-1);
}

struct slot_info {
        int     num;	// slot
        int     value;  // 1 for on, 0 for off
};

int main(int argc, char *argv[]){
	
	int c;
	char buf[200];
	int fd = -1;
	char optstring[] = "d::e::s:";
	
	struct slot_info slot;
	slot.num = -1;
	slot.value = -1;

	while (( c = getopt(argc, argv, optstring) )!= -1 )
	{
		switch (c) {
			case 'd':
				if (slot.value == -1)
					slot.value = 0;
				else
					usage(argv[0]);
				break;

			case 'e':
				if (slot.value == -1)
                                        slot.value = 1;
				else
					usage(argv[0]);
				break;

			case 's':
				if ( optarg == NULL && slot.num == -1 )
					usage(argv[0]);
				else
					slot.num = atoi(optarg);
				break;
			default:
				usage(argv[0]);
				break;

		}
	}


	if ( (slot.num >= 0) && (slot.num <= 1) && (slot.value == 0 ||slot.value == 1) )
		printf("PCIE: slot=%d, value=%d\n", slot.num, slot.value);
	else
		usage(argv[0]);

	if ((fd = open("/dev/pled", O_RDWR)) < 0) {
		perror("open failed\n");
		return -1;
	}
	
	if ( slot.value == 0 ){
		printf("power off\n");
		if(ioctl(fd, IOCTL_POWER_OFF, &slot) < 0) {
			perror("ioctl failed\n");
			return -1;
		}
	}else if ( slot.value == 1 ){
		printf("power on\n");
		if(ioctl(fd, IOCTL_POWER_ON, &slot) < 0) {
                        perror("ioctl failed\n");
                        return -1;
                }
	}
	//else{
		
	//}
	
	return 0;

}
