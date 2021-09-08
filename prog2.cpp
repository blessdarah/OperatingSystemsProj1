#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "my_const.h"

int main()
{
	// create the named pipe (FIFO) if not created yet
	int f1 = mkfifo(myfifo_1to2, 0666);
	int f2 = mkfifo(myfifo_2to1, 0666);
	printf("@p2: f1 = %d  f2 = %d\n", f1, f2);

	char rd_data[MAX], wr_data[MAX];

	printf("waiting for named pipes open ... \n");

	// P1&P2: order of open is important to unblock process
	// open() for RD will be blocked until the other side is open for WR
	int fd_rd = open(myfifo_1to2, O_RDONLY);
	// open() for WR will be blocked until the other side is open for RD
	int	fd_wr = open(myfifo_2to1, O_WRONLY);

	printf("named pipes opened and ready\n");

	// prog2: read first
	while (true)
	{
		read(fd_rd, rd_data, sizeof(rd_data));
		printf("received: %s\n", rd_data);
		if (strcmp(rd_data, "Q") == 0)
			break;

		printf("Enter a message: ");
		fgets(wr_data, MAX, stdin);
		wr_data[strlen(wr_data) - 1] = '\0';
		write(fd_wr, wr_data, strlen(wr_data) + 1);
	}
	close(fd_rd);
	close(fd_wr);
	printf("Q received from Prog1; Prog2 exits\n");
}