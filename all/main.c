/***********************************/
/* COMP 1921M Programming Project  */
/* 2021-22 Spring Semester         */
/*                                 */
/* Hamish Carr                     */
/***********************************/

/***********************************/
/* A first iteration to take a pgm */
/* file in binary and convert to   */
/* ASCII or vice versa             */
/***********************************/

/***********************************/
/* Main Routine                    */
/***********************************/

/* library for I/O routines        */
#include <stdio.h>

/* library for memory routines     */
#include <stdlib.h>

#define EXIT_NO_ERRORS 0
#define EXIT_WRONG_ARG_COUNT 1
#define EXIT_BAD_INPUT_FILE 2
#define EXIT_BAD_OUTPUT_FILE 3

#define MAGIC_NUMBER_RAW_PGM 0x3550
#define MAGIC_NUMBER_ASCII_PGM 0x3250
#define MIN_IMAGE_DIMENSION 1
#define MAX_IMAGE_DIMENSION 65536
#define MAX_COMMENT_LINE_LENGTH 128

/***********************************/
/* main routine                    */
/*                                 */
/* CLI parameters:                 */
/* argv[0]: executable name        */
/* argv[1]: input file name        */
/* argv[2]: output file name       */
/* returns 0 on success            */
/* non-zero error code on fail     */
/***********************************/


typedef struct FileInfo {
  char *commentLine;
  unsigned int width;
  unsigned int height;
  unsigned int maxGray;
  unsigned char *imageData;
  unsigned short *magic_Number;
  unsigned char magic_number[2];
} FileInfo;

int main(int argc, char **argv)
	{ /* main() */
	/* check for correct number of arguments 检查参数个数 */
	if (argc != 3)
		{ /* wrong arg count */
		/* print an error message        */
		printf("Usage: %s input_file output_file\n", argv[0]);
		/* and return an error code      */
		return EXIT_WRONG_ARG_COUNT;
		} /* wrong arg count */

	/* variables for storing the image       */
    	/* this is NOT good modularisation       */
    	/* and you will eventually replace it    */
    	/* for now, leave it here                */

	/* the magic number		         */
	/* stored as two bytes to avoid	         */
	/* problems with endianness	        存储为两个字节以避免端序问题 */
	/* Raw:    0x5035 or P5	50对应P  35对应字符5         */
	/* ASCII:  0x5032 or P2	50对应P  32对应字符2  	         */
	unsigned char magic_number[2] = {'0','0'};
	unsigned short *magic_Number = (unsigned short *) magic_number;

	/* we will store ONE comment	         */
	char *commentLine = NULL;

	/* the logical width & height	         */
	/* note: cannot be negative	         */
	unsigned int width = 0, height = 0;

	/* maximum gray value (assumed)	最大灰度值 （假设的）     */
	/* make it an integer for ease	         */
	unsigned int maxGray = 255;

	/* pointer to raw image data	   指向原始图像数据的指针      */
	unsigned char *imageData = NULL;

	/* now start reading in the data         */
	/* try to open the file for text I/O   尝试打开输入输出文件  */
	/* in ASCII mode b/c the header is text  在ASCII模式中 b/c   文件头为文本 */
	FILE *inputFile = fopen(argv[1], "r");

	/* if it fails, return error code        */
	if (inputFile == NULL)
		return EXIT_BAD_INPUT_FILE;

	/* read in the magic number            读取 magic number     */
	magic_number[0] = getc(inputFile);
	magic_number[1] = getc(inputFile);

	/* sanity check on the magic number    对magic number  进行检查   */
	if (*magic_Number != MAGIC_NUMBER_ASCII_PGM)
		{ /* failed magic number check   */
		/* be tidy: close the file       */
		fclose(inputFile);

		/* print an error message */
		printf("Error: Failed to read pgm image from file %s\n", argv[1]);

		/* and return                    */
		return EXIT_BAD_INPUT_FILE;
		} /* failed magic number check   */

	/* scan whitespace if present            */
	int scanCount = fscanf(inputFile, " ");

	/* check for a comment line       检查是否为注释行    commentLine为指针    */
	char nextChar = fgetc(inputFile);
	if (nextChar == '#')
		{ /* comment line         是注释行       */
		/* allocate buffer               */
		commentLine = (char *) malloc(MAX_COMMENT_LINE_LENGTH);
		/* fgets() reads a line          */
		/* capture return value          */
		char *commentString = fgets(commentLine, MAX_COMMENT_LINE_LENGTH, inputFile);
		/* NULL means failure            */
		if (commentString == NULL)
			{ /* NULL comment read   */
			/* free memory           */
			free(commentLine);
			/* close file            */
			fclose(inputFile);

			/* print an error message */
			printf("Error: Failed to read pgm image from file %s\n", argv[1]);

			/* and return            */
			return EXIT_BAD_INPUT_FILE;
			} /* NULL comment read   */
		} /* comment line */
	else
		{ /* not a comment line  不是注释行 则 */
		/* put character back        ungetch（）的功能是把一个字符退回到键盘缓冲区中     */
		ungetc(nextChar, inputFile);
		} /* not a comment line */

	/* read in width, height, grays         读入宽度 高度 灰度  */
	/* whitespace to skip blanks         ？？      */
	scanCount = fscanf(inputFile, " %u %u %u", &(width), &(height), &(maxGray));

	/* sanity checks on size & grays         */
	/* must read exactly three values       检查读入的三个值正确性   */
	if 	(
		(scanCount != 3				)	||
		(width 	< MIN_IMAGE_DIMENSION	) 	||
		(width 	> MAX_IMAGE_DIMENSION	) 	||
		(height < MIN_IMAGE_DIMENSION	) 	||
		(height > MAX_IMAGE_DIMENSION	) 	||
		(maxGray	!= 255		)
		)
		{ /* failed size sanity check    */
		/* free up the memory            */
		free(commentLine);

		/* be tidy: close file pointer   */
		fclose(inputFile);

		/* print an error message */
		printf("Error: Failed to read pgm image from file %s\n", argv[1]);

		/* and return                    */
		return EXIT_BAD_INPUT_FILE;
		} /* failed size sanity check    */

	/* allocate the data pointer         计算需要开辟多大空间存储图像并申请分配内存    */
	long nImageBytes = width * height * sizeof(unsigned char);
	imageData = (unsigned char *) malloc(nImageBytes);

	/* sanity check for memory allocation    */
	if (imageData == NULL)
		{ /* malloc failed             如果分配空间失败 */
		/* free up memory                */
		free(commentLine);

		/* close file pointer            */
		fclose(inputFile);

		/* print an error message */
		printf("Error: Failed to read pgm image from file %s\n", argv[1]);

		/* return error code             */
		return EXIT_BAD_INPUT_FILE;
		} /* malloc failed */

/* pointer for efficient read code    依次从输入文件读入nImageBytes个无符号像素值到grayValue，判断正确性后写入到nextGrayValue指向的内存中   */
	for (unsigned char *nextGrayValue = imageData; nextGrayValue < imageData + nImageBytes; nextGrayValue++)
		{ /* per gray value */
		/* read next value               */
		int grayValue = -1;
		int scanCount = fscanf(inputFile, " %u", &grayValue);

		/* sanity check	     以下三种情况失败     */
		if ((scanCount != 1) || (grayValue < 0) || (grayValue > 255))
			{ /* fscanf failed */
			/* free memory           */
			free(commentLine);
			free(imageData);

			/* close file            */
			fclose(inputFile);

			/* print error message   */
			printf("Error: Failed to read pgm image from file %s\n", argv[1]);

			/* and return            */
			return EXIT_BAD_INPUT_FILE;
			} /* fscanf failed */

		/* set the pixel value     成功读入 则将读入的grayValue赋值给nextGrayValue 指向的内存空间 */
		*nextGrayValue = (unsigned char) grayValue;
		} /* per gray value */

	/* we're done with the file, so close it  至此输入文件全部读完，关闭文件 */
	fclose(inputFile);

	/* open a file for writing       打开输出文件   */
	FILE *outputFile = fopen(argv[2], "w");

	/* check whether file opening worked     */
	if (outputFile == NULL)
		{ /* NULL output file */
		/* free memory                   */
		free(commentLine);
		free(imageData);

		/* print an error message        */
		printf("Error: Failed to write pgm image to file %s\n", argv[2]);

		/* return an error code          */
		return EXIT_BAD_OUTPUT_FILE;
		} /* NULL output file */

	/* write magic number, size & gray value */
	size_t nBytesWritten = fprintf(outputFile, "P2\n%d %d\n%d\n", width, height, maxGray);

	/* check that dimensions wrote correctly  检查上面信息是否写入成功*/
	if (nBytesWritten < 0)
		{ /* dimensional write failed    */
		/* free memory                   */
		free(commentLine);
		free(imageData);

		/* print an error message        */
		printf("Error: Failed to write pgm image to file %s\n", argv[2]);

		/* return an error code          */
		return EXIT_BAD_OUTPUT_FILE;
		} /* dimensional write failed    */

        /* pointer for efficient write code     依次写入  */
        for (unsigned char *nextGrayValue = imageData; nextGrayValue < imageData + nImageBytes; nextGrayValue++)
                { /* per gray value */
		/* get next char's column     获取当前字符所在的位置   若当前是本行最后一个字符，则nextCol值为0*/
		int nextCol = (nextGrayValue - imageData + 1) % width;

		/* write the entry & whitespace   写入nextGrayValue值，并根据nextCol值确定写入空格或回车 */
		nBytesWritten = fprintf(outputFile, "%d%c", *nextGrayValue, (nextCol ? ' ' : '\n') );

		/* sanity check on write     如果写入失败的处理    */
		if (nBytesWritten < 0)
			{ /* data write failed   */
			/* free memory           */
			free(commentLine);
			free(imageData);

			/* print error message   */
			printf("Error: Failed to write pgm image to file %s\n", argv[2]);

			/* return an error code  */
			return EXIT_BAD_OUTPUT_FILE;
			} /* data write failed   */
		} /* per gray value */
    fclose(outputFile);
//yxy测试test
FileInfo a,*fileValues;fileValues=&a;
outputFile=fopen("\\t1.txt","w");
printf("hello world");
fileValues->imageData=imageData;
fileValues->width=width/3;
printf("\n%d  ,%d,%d",*fileValues->imageData,fileValues->width,nImageBytes);
/*for (unsigned char *nextGrayValue = fileValues->imageData; nextGrayValue < fileValues->imageData + nImageBytes; nextGrayValue++){
    int nextCol = (nextGrayValue - fileValues->imageData + 1) % fileValues->width;
    nBytesWritten = fprintf(outputFile, "%d%c", *nextGrayValue, (nextCol ? ' ' : '\n') );
    // Check if it was possible to write into file
    if (nBytesWritten < 0){
      // If not, terminate with error code
      free(fileValues->commentLine);
      free(fileValues->imageData);
      printf("fail\n");
      }}
*/
for(int i=0;i<3;i++)
{unsigned char *nextGrayValue;int j;
    for(j=1, nextGrayValue= fileValues->imageData+width*i;j<=3;j++,nextGrayValue++)

    {
     int nextCol = (nextGrayValue - fileValues->imageData + 1) % fileValues->width;
     nBytesWritten = fprintf(outputFile, "%d%c", *nextGrayValue, (j%3 ?' '  : '\n') );
    }
    }
	/* at this point, we are done and can exit with a success code */
	return EXIT_NO_ERRORS;


	} /* main() */
