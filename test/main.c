
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

	/* check for correct number of arguments ���������� */
	if (argc != 3)
		{ /* wrong arg count */
		/* print an error message        */
		printf("Usage: %s input_file output_file\n", argv[0]);
		/* and return an error code      */
		return EXIT_WRONG_ARG_COUNT;
		} /* wrong arg count */

	/* now start reading in the data         */
	/* try to open the file for text I/O   ���Դ���������ļ�  */
	/* in ASCII mode b/c the header is text  ��ASCIIģʽ�� b/c   �ļ�ͷΪ�ı� */
	/* yxy: ���ú������ļ�*/
	int err_value=-1;
	inputFile=openfile(&err_value,argv[1]);

    if(err_value!=-1)
       return err_value;

	/* scan whitespace if present            */

	inputFile=check_com(&err_value,argv[1],inputFile,&commentLine);
    if(err_value!=-1)
         return err_value;

/* ����yxy��װ����ʵ�ֶ�ȡ */
	read_file(&err_value,inputFile,argv[1]);
	if(err_value!=-1)
         return err_value;


/* ����yxy��װ����ʵ��д�� */
   write_file(&err_value,argv[2]);
   if(err_value!=-1)
         return err_value;

	} /* main() */
