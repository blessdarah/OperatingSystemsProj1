#include <unistd.h>		 // open() close()
#include <fcntl.h>		 // O_* constants
#include <sys/stat.h>	 // mkfifo()
#include <sys/types.h> // mkfifo()
#include <string.h>		 // strlen() strcmp()
#include <stdio.h>		 // printf() scanf()
#include <stdlib.h>		 // exit()
#include <iostream>

using namespace std;

#include "my_const.h"

int main()
{

	// create the named pipe (FIFO) if not exist
	int f1 = mkfifo(player1to2, 0666);
	int f2 = mkfifo(player2to1, 0666);
	printf("@p1: f1 = %d  f2 = %d\n", f1, f2);

	char rd_data[MAX], wr_data[MAX];
	const char playTag = 'O';
	int location;

	printf("waiting for named pipes open ... \n");

	// P1&P2: order of open() is important to unblock processes
	// open() for WR will be blocked until the other side is open for RD
	int fd_wr = open(player1to2, O_WRONLY);
	// open() for RD will be blocked until the other side is open for WR
	int fd_rd = open(player2to1, O_RDONLY);

	printf("named pipes opened and ready\n");

	// player 1: engage first
	while (true)
	{
		printf("Where do you want to play? Select between cells 0 through 8 (Q to quit): ");
		cin >> location;

		// validate user location
		validatePlayLocation(location);
		fgets(wr_data, MAX, stdin);
		wr_data[strlen(wr_data) - 1] = '\0'; // '\n' is replaced by NULL ('\0')
		write(fd_wr, wr_data, strlen(wr_data) + 1);
		if (strcmp(wr_data, "Q") == 0)
			break;

		read(fd_rd, rd_data, sizeof(rd_data));
		printf("received: %s\n", rd_data);
	}
	close(fd_wr);
	close(fd_rd);
	unlink(player1to2);
	unlink(player2to1);
	printf("Prog1 exits\n");
}

bool validatePlayLocation(int location)
{
	if (location < 0 || location > 8)
}
