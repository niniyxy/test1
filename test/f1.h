#define EXIT_NO_ERRORS 0
#define EXIT_WRONG_ARG_COUNT 1
#define EXIT_BAD_INPUT_FILE 2
#define EXIT_BAD_OUTPUT_FILE 3

#define MAGIC_NUMBER_RAW_PGM 0x3550
#define MAGIC_NUMBER_ASCII_PGM 0x3250
#define MIN_IMAGE_DIMENSION 1
#define MAX_IMAGE_DIMENSION 65536
#define MAX_COMMENT_LINE_LENGTH 128
/* the magic number		         */
	/* stored as two bytes to avoid	         */
	/* problems with endianness	        存储为两个字节以避免端序问题 */
	/* Raw:    0x5035 or P5	50对应P  35对应字符5         */
	/* ASCII:  0x5032 or P2	50对应P  32对应字符2  	         */
	unsigned char magic_number[2] = {'0','0'};
	unsigned short *magic_Number = (unsigned short *) magic_number;
	/* variables for storing the image       */
    	/* this is NOT good modularisation       */
    	/* and you will eventually replace it    */
    	/* for now, leave it here                */


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
    long nImageBytes;