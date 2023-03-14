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
	/* check for correct number of arguments ���������� */
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
	/* problems with endianness	        �洢Ϊ�����ֽ��Ա���������� */
	/* Raw:    0x5035 or P5	50��ӦP  35��Ӧ�ַ�5         */
	/* ASCII:  0x5032 or P2	50��ӦP  32��Ӧ�ַ�2  	         */
	unsigned char magic_number[2] = {'0','0'};
	unsigned short *magic_Number = (unsigned short *) magic_number;

	/* we will store ONE comment	         */
	char *commentLine = NULL;

	/* the logical width & height	         */
	/* note: cannot be negative	         */
	unsigned int width = 0, height = 0;

	/* maximum gray value (assumed)	���Ҷ�ֵ ������ģ�     */
	/* make it an integer for ease	         */
	unsigned int maxGray = 255;

	/* pointer to raw image data	   ָ��ԭʼͼ�����ݵ�ָ��      */
	unsigned char *imageData = NULL;

	/* now start reading in the data         */
	/* try to open the file for text I/O   ���Դ���������ļ�  */
	/* in ASCII mode b/c the header is text  ��ASCIIģʽ�� b/c   �ļ�ͷΪ�ı� */
	FILE *inputFile = fopen(argv[1], "r");

	/* if it fails, return error code        */
	if (inputFile == NULL)
		return EXIT_BAD_INPUT_FILE;

	/* read in the magic number            ��ȡ magic number     */
	magic_number[0] = getc(inputFile);
	magic_number[1] = getc(inputFile);

	/* sanity check on the magic number    ��magic number  ���м��   */
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

	/* check for a comment line       ����Ƿ�Ϊע����    commentLineΪָ��    */
	char nextChar = fgetc(inputFile);
	if (nextChar == '#')
		{ /* comment line         ��ע����       */
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
		{ /* not a comment line  ����ע���� �� */
		/* put character back        ungetch�����Ĺ����ǰ�һ���ַ��˻ص����̻�������     */
		ungetc(nextChar, inputFile);
		} /* not a comment line */

	/* read in width, height, grays         ������ �߶� �Ҷ�  */
	/* whitespace to skip blanks         ����      */
	scanCount = fscanf(inputFile, " %u %u %u", &(width), &(height), &(maxGray));

	/* sanity checks on size & grays         */
	/* must read exactly three values       �����������ֵ��ȷ��   */
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

	/* allocate the data pointer         ������Ҫ���ٶ��ռ�洢ͼ����������ڴ�    */
	long nImageBytes = width * height * sizeof(unsigned char);
	imageData = (unsigned char *) malloc(nImageBytes);

	/* sanity check for memory allocation    */
	if (imageData == NULL)
		{ /* malloc failed             �������ռ�ʧ�� */
		/* free up memory                */
		free(commentLine);

		/* close file pointer            */
		fclose(inputFile);

		/* print an error message */
		printf("Error: Failed to read pgm image from file %s\n", argv[1]);

		/* return error code             */
		return EXIT_BAD_INPUT_FILE;
		} /* malloc failed */

/* pointer for efficient read code    ���δ������ļ�����nImageBytes���޷�������ֵ��grayValue���ж���ȷ�Ժ�д�뵽nextGrayValueָ����ڴ���   */
	for (unsigned char *nextGrayValue = imageData; nextGrayValue < imageData + nImageBytes; nextGrayValue++)
		{ /* per gray value */
		/* read next value               */
		int grayValue = -1;
		int scanCount = fscanf(inputFile, " %u", &grayValue);

		/* sanity check	     �����������ʧ��     */
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

		/* set the pixel value     �ɹ����� �򽫶����grayValue��ֵ��nextGrayValue ָ����ڴ�ռ� */
		*nextGrayValue = (unsigned char) grayValue;
		} /* per gray value */

	/* we're done with the file, so close it  ���������ļ�ȫ�����꣬�ر��ļ� */
	fclose(inputFile);

	/* open a file for writing       ������ļ�   */
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

	/* check that dimensions wrote correctly  ���������Ϣ�Ƿ�д��ɹ�*/
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

        /* pointer for efficient write code     ����д��  */
        for (unsigned char *nextGrayValue = imageData; nextGrayValue < imageData + nImageBytes; nextGrayValue++)
                { /* per gray value */
		/* get next char's column     ��ȡ��ǰ�ַ����ڵ�λ��   ����ǰ�Ǳ������һ���ַ�����nextColֵΪ0*/
		int nextCol = (nextGrayValue - imageData + 1) % width;

		/* write the entry & whitespace   д��nextGrayValueֵ��������nextColֵȷ��д��ո��س� */
		nBytesWritten = fprintf(outputFile, "%d%c", *nextGrayValue, (nextCol ? ' ' : '\n') );

		/* sanity check on write     ���д��ʧ�ܵĴ���    */
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
//yxy����test
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
