#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	if (argc != 3) {
		printf("Incorrect number of parameters\n");
		exit(-1);
	}
	
	// Получаем имена каналов через аргументы командной строки
	char *input_fifo_name = argv[1];
	char *output_fifo_name = argv[2];
	
	// Создаем или используем уже созданные каналы
	mknod(input_fifo_name, S_IFIFO | 0666, 0);
	mknod(output_fifo_name, S_IFIFO | 0666, 0);
	
	int fd_input, fd_output;
	
	if((fd_output = open(output_fifo_name, O_WRONLY)) < 0){
      printf("Can\'t open FIFO for writing\n");
      exit(-1);
    }
	
	if((fd_input = open(input_fifo_name, O_RDONLY)) < 0){
      printf("Can\'t open FIFO for reading\n");
      exit(-1);
    }
	
    char str_buf[1];
	int num = 1;
	int size;
	while (num < 10) {
		// Отправляем другому процессу число
		str_buf[0] = num;
		size = write(fd_output, str_buf, 1);
		if (size != 1) {
			printf("Writing to FIFO \"%s\" failed", output_fifo_name);
			exit(-1);
		}
		printf("SEND %d\n", num);
		
		// Получаем от процесса число
		size = read(fd_input, str_buf, 1);
		if (size != 1) {
			printf("Reading from FIFO \"%s\" failed", input_fifo_name);
			exit(-1);
		}
		printf("RECEIVED %d\n", str_buf[0]);
		num = str_buf[0] + 1;
	}
	
	// Отправляем сигнал об окончании счёта
	str_buf[0] = 0;
	size = write(fd_output, str_buf, 1);
	if (size != 1) {
		printf("Writing to FIFO \"%s\" failed", output_fifo_name);
		exit(-1);
	}
	printf("SENT END OF COUNTING SIGNAL: 0\n");
	
	if (close(fd_input) < 0) {
		printf("Failed to close FIFO \"%s\"", input_fifo_name);
		exit(-1);
	}
	if (close(fd_output) < 0) {
		printf("Failed to close FIFO \"%s\"", output_fifo_name);
		exit(-1);
	}
	
    return 0;
}
