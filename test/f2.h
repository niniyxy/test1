FILE * openfile(int *x,char * filename)
{
    FILE *inputFile = fopen(filename, "r");

	/* if it fails, return error code        */
	if (inputFile == NULL)
		*x=EXIT_BAD_INPUT_FILE;

	/* read in the magic number            读取 magic number     */
	magic_number[0] = getc(inputFile);
	magic_number[1] = getc(inputFile);
	printf("hello");
    printf("%s",magic_number);
	/* sanity check on the magic number    对magic number  进行检查   */
	if (*magic_Number != MAGIC_NUMBER_ASCII_PGM)
		{ /* failed magic number check   */
		/* be tidy: close the file       */
		fclose(inputFile);

		/* print an error message */
		printf("Error: Failed to read pgm image from file %s\n", filename);

		/* and return                    */
		*x=EXIT_BAD_INPUT_FILE;

		} /* failed magic number check   */
    return inputFile;
}

/* check for a comment line       检查是否为注释行    commentLine为指针    */
FILE *check_com(int *x,char *argv1,FILE *inputFile,char **commentLine_add)
	{ int scanCount = fscanf(inputFile, " ");

	/* check for a comment line       检查是否为注释行    commentLine为指针    */
	    char nextChar = fgetc(inputFile);
	if (nextChar == '#')
		{ /* comment line         是注释行       */
		/* allocate buffer               */
		*commentLine_add = (char *) malloc(MAX_COMMENT_LINE_LENGTH);
		/* fgets() reads a line          */
		/* capture return value          */
		char *commentString = fgets(*commentLine_add, MAX_COMMENT_LINE_LENGTH, inputFile);
		/* NULL means failure            */
		if (commentString == NULL)
			{ /* NULL comment read   */
			/* free memory           */
			free(*commentLine_add);
			/* close file            */
			fclose(inputFile);

			/* print an error message */
			printf("Error: Failed to read pgm image from file %s\n", argv1);

			/* and return            */
			*x=EXIT_BAD_INPUT_FILE;
			} /* NULL comment read   */
		} /* comment line */
	else
		{ /* not a comment line  不是注释行 则 */
		/* put character back        ungetch（）的功能是把一个字符退回到键盘缓冲区中     */
		ungetc(nextChar, inputFile);
		}
    return inputFile;
	}
void read_file(int *x,FILE *inputFile,char *argv1)
{
/* read in width, height, grays         读入宽度 高度 灰度  */
	/* whitespace to skip blanks         ？？      */
	int scanCount = fscanf(inputFile, " %u %u %u", &width, &height, &maxGray);

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
		printf("Error: Failed to read pgm image from file %s\n", argv1);

		/* and return                    */
		*x= EXIT_BAD_INPUT_FILE; return;
		} /* failed size sanity check    */

	/* allocate the data pointer         计算需要开辟多大空间存储图像并申请分配内存    */
	nImageBytes = (width) * (height) * sizeof(unsigned char);
	imageData = (unsigned char *) malloc(nImageBytes);

	/* sanity check for memory allocation    */
	if (imageData == NULL)
		{ /* malloc failed             如果分配空间失败 */
		/* free up memory                */
		free(commentLine);

		/* close file pointer            */
		fclose(inputFile);

		/* print an error message */
		printf("Error: Failed to read pgm image from file %s\n", argv1);

		/* return error code             */
		*x= EXIT_BAD_INPUT_FILE; return;
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
			printf("Error: Failed to read pgm image from file %s\n", argv1);

			/* and return            */
			 *x=EXIT_BAD_INPUT_FILE; return;
			} /* fscanf failed */

		/* set the pixel value     成功读入 则将读入的grayValue赋值给nextGrayValue 指向的内存空间 */
		*nextGrayValue = (unsigned char) grayValue;
		} /* per gray value */

	/* we're done with the file, so close it  至此输入文件全部读完，关闭文件 */
	fclose(inputFile);
}

void write_file(int *x,char* argv2)
{
    /* open a file for writing       打开输出文件   */
	FILE *outputFile = fopen(argv2, "w");

	/* check whether file opening worked     */
	if (outputFile == NULL)
		{ /* NULL output file */
		/* free memory                   */
		free(commentLine);
		free(imageData);

		/* print an error message        */
		printf("Error: Failed to write pgm image to file %s\n", argv2);

		/* return an error code          */
		*x=EXIT_BAD_OUTPUT_FILE; return ;
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
		printf("Error: Failed to write pgm image to file %s\n", argv2);

		/* return an error code          */
		*x=EXIT_BAD_OUTPUT_FILE;return ;
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
			printf("Error: Failed to write pgm image to file %s\n", argv2);

			/* return an error code  */
			*x=EXIT_BAD_OUTPUT_FILE;return ;
			} /* data write failed   */
		} /* per gray value */

	/* at this point, we are done and can exit with a success code */
	//*x=EXIT_NO_ERRORS;
	return ;
}