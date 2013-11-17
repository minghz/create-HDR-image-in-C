/* This is a program that combines two PGM images by averaging */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>


/* Global Variables */
#define MAXLINE 80

char line[MAXLINE];
int width;
int height;

void SkipLines(FILE** fp, int lines){
	printf("step - in SkipLines\n");
	int i;
	for(i = lines; i > 0; i--){
		fgets(line, MAXLINE, *fp);
	}

}

void Get_w_h(FILE* fp, int* width, int* height){
	
	printf("step - in Get_w_h\n");
	
  fgets(line, MAXLINE, fp); //skip first line

	fgets (line, MAXLINE, fp); // 2nd line is width 
	sscanf(line, "%d", width);
	fgets (line, MAXLINE, fp); // 3rd line is height 
	sscanf(line, "%d", height);
	
}

//not working
int PlotComparagram(FILE* im1, FILE* im2){
	printf("step - in PlotComparagram\n");
	// my comparaplot dimentions will always be from 0 to 255
	// on both axis
	int w, h;
	
	unsigned char ch1, ch2;
	unsigned char comp[255][255];
	
	Get_w_h(im1, &w, &h);
	SkipLines(&im1, 1);
	SkipLines(&im2, 4);
	printf("width: %d, height: %d\n", w, h);
	
	int i1, i2, k1, k2;
	for(i1 = 0; i1 < 256; i1 ++){
	printf("i1\n");
		for(i2 = 0; i2 < 256; i2 ++){
			for(k1 = 0; k1 < w; k1 ++){
				for(k2 = 0; k2 < h; k2 ++){
				
					ch1 = fgetc(im1);
					ch2 = fgetc(im2);
					
					if(ch1 == i1 && ch2 == i2)
						comp[i1][i2] ++;
				
					if(((i1+1) % 1000) == 0)
						printf("char: %c", comp[i1][i2]);
				}
			}
		}
	}
	
	printf("step - before drawing comparagraph\n");
	FILE* nf;
	int a, b;
	nf = fopen("comparagraph.pgm", "w");
	fprintf(nf, "%s\n%d\n%d\n%d\n", "P5", 255, 255, 255);
	
		for(a = 0; a < 256; a ++){
			for(b = 0; b < 256; b++){
					fprintf(nf, "%c", comp[a][b]);
			
			}
		}
	fclose(nf);
}

unsigned char DrawChannel(unsigned char cn1, unsigned char cn2, unsigned char cn3,
						  float k1, float k2, float k3,
						  unsigned char *c1w, unsigned char *c2w, unsigned char *c3w)
{

	float ren;
	float e = 2.71828183;
	float c;
	float x;
	float cert1, cert2, cert3; // output for certainty functions at
							   // different exposures
	float c1, c2, c3;
	int i1,i2,i3;
	i1 = (int)cn1;
	i2 = (int)cn2;
	i3 = (int)cn3;
	
	float sigma = 8.0;
	float gamma1 = 0.77;
	float gamma2 = 1.3;
	
	/* doing the expander with gamma function
	 * or simply by means of exponential funciton
	 */
	c1 = pow(i1, gamma1);
	c2 = pow(i2, gamma1);
	c3 = pow(i3, gamma1);
	//c1 = cn1;
	//c2 = cn2;
	//c3 = cn3;g
	
	/* median expose */
	c = pow(e, -sigma*( ((cn1-128)/255.0) * ((cn1-128)/255.0) ) );
	*c1w = ((float)c1*(c));
	cert1 = c;
	
	/* under expose */
	c = pow(e, -sigma*( ((cn2-128)/255.0) * ((cn2-128)/255.0) ) );
	*c2w = ((float)c2*(c));
	cert2 = c;
	
	/* over expose */
	c = pow(e, -sigma*( ((cn3-128)/255.0) * ((cn3-128)/255.0) ) );
	*c3w = ((float)c3*(c));
	cert3 = c;
	
	ren = (k1*(*c1w) + k2*(*c2w) + k3*(*c3w)) / (k1*cert1 + k2*cert2 + k3*cert3);
	
	return (unsigned char)(pow(ren, gamma2));
	//return ren;
}

unsigned char DrawGray(unsigned char cn1, unsigned char cn2, unsigned char cn3,
						  float k1, float k2, float k3,
						  unsigned char *c1w, unsigned char *c2w, unsigned char *c3w)
{

	float ren;
	float e = 2.71828183;
	float c;
	float x;
	float cert1, cert2, cert3; // output for certainty functions at
							   // different exposures
	float c1, c2, c3;
	float i1,i2,i3;
	
	i1 = (float)cn1;
	i2 = (float)cn2;
	i3 = (float)cn3;
	
	float sigma = 5.0;
	float gamma1 = 0.77;
	float gamma2 = 1.3;
	
	c1 = pow(i1, gamma1);
	c2 = pow(i2, gamma1);
	c3 = pow(i3, gamma1);
	//c1 = cn1;
	//c2 = cn2;
	//c3 = cn3;
	
	/* I believe I will need to get the response function of the camera
	* in order to perform a compander
	* in betwen the compander, I have the HDR image processing
	* analysis, which include the certainty functions at different
	* exposures.
	* Its better to use a compander because this way we are dealing 
	* directly with the quantity of light, not a filtered version
	* of it.
	*/
	
	/* median expose */ 
	c = pow(e, -sigma*( ((cn1-128)/255.0) * ((cn1-128)/255.0) ) );
	*c1w = ((float)c1*(c));
	cert1 = c;
	
	/* under expose */
	c = pow(e, -sigma*( ((cn2-128)/255.0) * ((cn2-128)/255.0) ) );
	*c2w = ((float)c2*(c));
	cert2 = c;
	
	/* over expose */
	c = pow(e, -sigma*( ((cn3-128)/255.0) * ((cn3-128)/255.0) ) );
	*c3w = ((float)c3*(c));
	cert3 = c;
	
	ren = (k1*(*c1w) + k2*(*c2w) + k3*(*c3w)) / (k1*cert1 + k2*cert2 + k3*cert3);
	
	return (unsigned char)(pow(ren, gamma2));
	//return ren;
}


void HandlerPPM (char *file1, char *file2, char *file3){
	
	/* in PPM format, there are tree bits per pixel, each bit
		corresponding to the levels of RGB in the pixel */
	printf("step - .ppm file\n");
	
	FILE *fp1;//image 1
	FILE *fp2;//image 2
	FILE *fp3;//image 3
	
	FILE *ceri1;//image 1
	FILE *ceri2;//image 2
	FILE *ceri3;//image 3
	
	FILE *rend;//final HDR rendered file
	
	int width;
	int height;
		
	fp1 = fopen(file1, "rb");
	fp2 = fopen(file2, "rb");
	fp3 = fopen(file3, "rb");
	
	/* Get width and height from first image */
	Get_w_h(fp1, &width, &height);
	/* Skip lines so we are ready to read */
	SkipLines(&fp1, 1);
	SkipLines(&fp2, 4);
	SkipLines(&fp3, 4);
	
	
	//write to rendered file
	rend = fopen("rendered.ppm", "w");
	fprintf(rend, "%s\n%d\n%d\n%d\n", "P6", width, height, 255);
	
	ceri1 = fopen("cert1.ppm", "w");
	fprintf(ceri1, "%s\n%d\n%d\n%d\n", "P6", width, height, 255);
	
	ceri2 = fopen("cert2.ppm", "w");
	fprintf(ceri2, "%s\n%d\n%d\n%d\n", "P6", width, height, 255);
	
	ceri3 = fopen("cert3.ppm", "w");
	fprintf(ceri3, "%s\n%d\n%d\n%d\n", "P6", width, height, 255);
		
	int size;
	float ku, km, ko; //exposures for the three images
	unsigned char rendR, rendG, rendB;
	ku = 1;
	km = 4;
	ko = 16;
	
	unsigned char bR1, bR2, bR3;
	unsigned char bG1, bG2, bG3;
	unsigned char bB1, bB2, bB3;
	
	unsigned char c1wR, c2wR, c3wR;
	unsigned char c1wG, c2wG, c3wG;
	unsigned char c1wB, c2wB, c3wB;

	printf("step - writing channels\n");
	for(size = width * height; size > 0; size --){
	
		/* Red Channel */
		bR1 = fgetc(fp1);//median
		bR2 = fgetc(fp2);//under
		bR3 = fgetc(fp3);//over
		rendR = DrawChannel(bR1, bR2, bR3, km, ku, ko, &c1wR, &c2wR, &c3wR);
		
		bG1 = fgetc(fp1);
		bG2 = fgetc(fp2);
		bG3 = fgetc(fp3);
		rendG = DrawChannel(bG1, bG2, bG3, km, ku, ko, &c1wG, &c2wG, &c3wG);
		
		bB1 = fgetc(fp1);
		bB2 = fgetc(fp2);
		bB3 = fgetc(fp3);
		rendB = DrawChannel(bB1, bB2, bB3, km, ku, ko, &c1wB, &c2wB, &c3wB);

		/* write rendered file */
		fprintf(rend, "%c", rendR);
		fprintf(rend, "%c", rendG);
		fprintf(rend, "%c", rendB);
		
		/* write certainty c1 */
		fprintf(ceri1, "%c", c1wR);
		fprintf(ceri1, "%c", c1wG);
		fprintf(ceri1, "%c", c1wB);
	
		/* write certainty c1 */
		fprintf(ceri2, "%c", c2wR);
		fprintf(ceri2, "%c", c2wG);
		fprintf(ceri2, "%c", c2wB);
		
		/* write certainty c1 */
		fprintf(ceri3, "%c", c3wR);
		fprintf(ceri3, "%c", c3wG);
		fprintf(ceri3, "%c", c3wB);
	}
	
	fclose(fp1);
	fclose(fp2);
	fclose(fp3);
	
	fclose(rend);
	
	fclose(ceri1);
	fclose(ceri2);
	fclose(ceri3);
	
	
	printf("step - closed files\n");
}

void HandlerPGM (char *file1, char* file2, char* file3){
	
	FILE *fp1;//image 1
	FILE *fp2;//image 2
	FILE *fp3;//image 3
	
	FILE *rend;//final HDR rendered file
	FILE *ceri1;//certainty image 1
	FILE *ceri2;//certainty image 2
	FILE *ceri3;//over
		
	printf("step - file pointers done\n");
	
	/* Check File Types */
	fp1 = fopen(file1, "rb");	
	fp2 = fopen(file2, "rb");
	fp3 = fopen(file3, "rb");
	
	/* Check File Size */
	int width;
	int height;
	
	fgets(line, MAXLINE, fp1); /* skipping first line */
	fgets (line, MAXLINE, fp1); /* 2nd line is width */
	sscanf(line, "%d", &width);
	fgets (line, MAXLINE, fp1); /* 3rd line is height */
	sscanf(line, "%d", &height);
	
	//skipping to apropriate place for writing
	fgets(line, MAXLINE, fp1);
	// skip setup lines of file 2
	fgets(line, MAXLINE, fp2);
	fgets(line, MAXLINE, fp2);
	fgets(line, MAXLINE, fp2);
	fgets(line, MAXLINE, fp2);
	// skip setup lines of file 3
	fgets(line, MAXLINE, fp3);
	fgets(line, MAXLINE, fp3);
	fgets(line, MAXLINE, fp3);
	fgets(line, MAXLINE, fp3);
	/* after this the tree files at start of Line 5 */
	
	//write to rendered file
	rend = fopen("rendered.pgm", "w");
	fprintf(rend, "%s\n%d\n%d\n%d\n", "P5", width, height, 255);

	//write to certainty image1
	ceri1 = fopen("cert1.pgm", "w");
	fprintf(ceri1, "%s\n%d\n%d\n%d\n", "P5", width, height, 255);

	//write to certainty image2
	ceri2 = fopen("cert2.pgm", "w");
	fprintf(ceri2, "%s\n%d\n%d\n%d\n", "P5", width, height, 255);
	
	//write to certainty image3
	ceri3 = fopen("cert3.pgm", "w");
	fprintf(ceri3, "%s\n%d\n%d\n%d\n", "P5", width, height, 255);
	
	int size;
	unsigned char c1, c2, c3, ren;
	float e = 2.71828183;
	float c;
	float x, f1, f2, f3;
	//float a, b, c; camera parameters for response function
	float ku, km, ko; //exposures for the three images
	
	float cert1, cert2, cert3; // output for certainty functions
	
	for(size = width * height; size > 0; size --){
	
		c1 = fgetc(fp1);
		c2 = fgetc(fp2);
		c3 = fgetc(fp3);
		
		/* response function */
		//f1 = pow( ( (pow(e, b)*pow(c1, a))/(pow(e, b)*pow(c1, a) + 1) ), c);
		ren = DrawGray(c1, c2, c3, km, ku, ko, &c1, &c2, &c3);

		fprintf(rend, "%c", ren); //write to final render
		
		//write to certainty files
		fprintf(ceri1, "%c", c1); //median
		fprintf(ceri2, "%c", c2); //under
		fprintf(ceri3, "%c", c3); //over
	
	}
	
	fclose(fp1);
	fclose(fp2);
	fclose(fp3);
	
	fclose(rend);
	fclose(ceri1);
	fclose(ceri2);
	fclose(ceri3);
	
	printf("step - closed files\n");
}

void FormatCheck(char *file1, char *file2, char *file3){

	printf("step - checking format\n");

	FILE *fp1;//image 1
	FILE *fp2;//image 2
	FILE *fp3;//image 3
	char line[MAXLINE];
	bool is_PGM = 0;
	bool is_PPM = 0;

	/*checking file types*/
	fp1 = fopen(file1, "rb");
	fgets(line, MAXLINE, fp1); /*getting first line*/
	
	if(line[1] == '5') is_PGM = 1;
	if(line[1] == '6') is_PPM = 1;
	if(line[1] != '5' && line[1] != '6'){
		printf("Error1: not supported format (PGM, PPM)\n");
		exit(1);
	}
	fp2 = fopen(file2, "rb");
	fgets(line, MAXLINE, fp2); /*getting first line*/
	if(line[1] == '5') is_PGM = 1;
	if(line[1] == '6') is_PPM = 1;
	if(line[1] != '5' && line[1] != '6'){
		printf("Error1: not supported format (PGM, PPM)\n");
		exit(1);
	}
	fp3 = fopen(file3, "rb");
	fgets(line, MAXLINE, fp3); /*getting first line*/
	if(line[1] == '5') is_PGM = 1;
	if(line[1] == '6') is_PPM = 1;
	if(line[1] != '5' && line[1] != '6'){
		printf("Error1: not supported format (PGM, PPM)\n");
		exit(1);
	}
	
	if(is_PPM && is_PGM){
		printf("Error2: inconsistent formats (PGM and PPM)\n");
		exit(1);
	}
	
	fclose(fp1);
	fclose(fp2);
	fclose(fp3);
	
	printf("step - before file Handlers\n");
	
	if(is_PPM)
		HandlerPPM((char*)file1, (char*)file2, (char*)file3);
	if(is_PGM)
		HandlerPGM((char*)file1, (char*)file2, (char*)file3);


}

int main (int argc, char **argv){
	
	//char file1[MAXLINE];
	//char file2[MAXLINE];
	//char file3[MAXLINE];
	
	if(argc < 2){
		printf ("Enter the name of the PGM files: (lighter first)\n");
		//scanf("%s", file1);
		//scanf("%s", file2);
		//scanf("%s", file3);
		char *file1="a.ppm";
		char *file2="b.ppm";
		char *file3="c.ppm";
		/*FILE* f1;
		FILE* f2;
		f1 = fopen("a.pgm", "rb");
		f2 = fopen("b.pgm", "rb");
		printf("step - before PlotComparagram\n");
		PlotComparagram(f1, f2);
		printf("step - after PlotComparagram\n");
		fclose(f1);
		fclose(f2);*/
		FormatCheck((char*)file1, (char*)file2, (char*)file3);
	}
	
	return 0;
}
