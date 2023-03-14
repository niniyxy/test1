
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
#include "f1.h"
#include "f2.h"
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

/* main() */
int main(int argc, char **argv)
	{ FILE *inputFile;

	/* check for correct number of arguments 检查参数个数 */
	if (argc != 3)
		{ /* wrong arg count */
		/* print an error message        */
		printf("Usage: %s input_file output_file\n", argv[0]);
		/* and return an error code      */
		return EXIT_WRONG_ARG_COUNT;
		} /* wrong arg count */

	/* now start reading in the data         */
	/* try to open the file for text I/O   尝试打开输入输出文件  */
	/* in ASCII mode b/c the header is text  在ASCII模式中 b/c   文件头为文本 */
	/* yxy: 调用函数打开文件*/
	int err_value=-1;
	inputFile=openfile(&err_value,argv[1]);

    if(err_value!=-1)
       return err_value;

	/* scan whitespace if present            */

	inputFile=check_com(&err_value,argv[1],inputFile,&commentLine);
    if(err_value!=-1)
         return err_value;

/* 调用yxy封装函数实现读取 */
	read_file(&err_value,inputFile,argv[1]);
	if(err_value!=-1)
         return err_value;


/* 调用yxy封装函数实现写入 */
   write_file(&err_value,argv[2]);
   if(err_value!=-1)
         return err_value;

	} /* main() */
