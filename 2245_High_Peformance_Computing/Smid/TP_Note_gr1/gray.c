#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <limits.h>
#include <math.h>

#include <x86intrin.h> // gcc specific

#include <cpuid.h>

// RGB image size : height x width x 3 bytes
#define MAX_HEIGHT  1024
#define MAX_WIDTH   1024
#define N MAX_HEIGHT*MAX_WIDTH*3

#define Y_RED_SCALE     0.29891f
#define Y_GREEN_SCALE   0.58661f
#define Y_BLUE_SCALE    0.11448f
    
/*************************************************************************
                           timing utilities
**************************************************************************/

static long time1, time_diff, sum_diff= 0, max=0;

static inline void reset_stats(){
    sum_diff= 0;
    max=0;
}

static inline void start_timer(){
    time1= __rdtsc();
}

static inline void pause_timer(){
    time_diff= __rdtsc()-time1;
    sum_diff += time_diff;
    if (time_diff > max) max= time_diff;
}

static inline void display_stats(char *msg,int nb_loops){
    static long old_avg;
    long avg= (float) (sum_diff-max)/(nb_loops-1);
    printf("%s: average= %12li\n",msg,avg);
    if (old_avg == 0){ // first call
        old_avg= avg;
    }else{ // second call
        float avg_speedup= (float) old_avg/avg;
        printf("Speedup: average= %5.2f\n",avg_speedup);
        old_avg= 0;
    };
}

/*************************************************************************
                           image utilities
**************************************************************************/

int read_color_ppm(char *filename,
                   int *height, int *width,
                   unsigned char img_red[], unsigned char img_green[], unsigned char img_blue[]){
    
    FILE *file = fopen(filename, "rb");
    if (file==NULL){
        printf("cannot open file %s\n",filename);
        return 1;
    }
    /* Read the PPM file header. */
    int flag=0;
    int maxcolors_val;
    char magic[1024];
    char line[1024];
    int count=0;
    while (fgets(line, 1024, file) != NULL) {
        flag = 0;
        for (int i = 0; i < strlen(line); i++) {
            if (isgraph(line[i]) && (flag == 0)) {
                if ((line[i] == '#') && (flag == 0)) {
                    flag = 1;
                }
            }
        }
        if (flag == 0) {
            if (count == 0) {
                count += sscanf(line, "%s %d %d %d", magic, width, height, &maxcolors_val); 
            } else if (count == 1) {
                count += sscanf(line, "%d %d %d", width, height, &maxcolors_val);
            } else if (count == 2) {
                count += sscanf(line, "%d %d", height, &maxcolors_val);
            } else if (count == 3) {
                count += sscanf(line, "%d", &maxcolors_val);
            }
        }
        if (count == 4) {
            break;
        }
    }
    int is_ascii;
    if (strcmp(magic, "P3") == 0) {
        is_ascii = 1;
    } else if (strcmp(magic, "P6") == 0) {
        is_ascii = 0;
    } else {
        fprintf(stderr, "Error: Input file not in PPM format!\n");
        return 2;
    }
//    fprintf(stdout, "Info: magic=%s, x_val=%d, y_val=%d, maxcolors_val=%d\n",
//            magic, *width, *height, maxcolors_val);
    /* Read the rest of the PPM file. */
    int pcount=0;
    int char_val;
    while ((char_val= fgetc(file)) != EOF) {
        ungetc(char_val, file);
        int r_val, g_val, b_val;
        if (is_ascii == 1) {
            if (fscanf(file, "%d %d %d", &r_val, &g_val, &b_val) != 3){
                if (pcount != (*width) * (*height)){ 
                    fprintf(stderr, "Error: missing value %i in PPM file !",pcount);
                    return 3;
                }else{ // leading space before EOF
                    break;
                }
            }
        } else {
            r_val = fgetc(file);
            g_val = fgetc(file);
            b_val = fgetc(file);
        }
        img_red[pcount]= r_val;
        img_green[pcount]= g_val;
        img_blue[pcount]= b_val;
        pcount += 1;
    }
    fclose(file);
    return 0;
} 

int write_gray_ppm(char *filename, 
                   int height, int width, int is_ascii,
                   float img_gray[]){
    
    FILE *file = fopen(filename, "w");
    if (file==NULL){
        printf("cannot open file %s for writing\n",filename);
        return 1;
    }
    /* Write the magic number string. */
    if (is_ascii == 1) {
        fprintf(file, "P3\n");
    } else {
        fprintf(file, "P6\n");
    }
    /* Write a comment containing the file name. */
    fprintf(file, "# %s\n", filename);
    /* Write the image dimensions. */
    fprintf(file, "%d %d\n", width, height);
    /* Write the maximum color/grey level allowed. */
    fprintf(file, "%d\n", 255);
    /* Write the image data. */
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            unsigned char gray_val= (unsigned char) img_gray[i*width+j];
                if (is_ascii == 1) {
                if ((j!=0) && (j%4) == 0) {
                    fprintf(file, "\n");
                }
                fprintf(file, "%4d %4d %4d ",gray_val,gray_val,gray_val);
            } else {
                fprintf(file, "%c%c%c",gray_val,gray_val,gray_val);
            }
        }
    }
    fclose(file);
    return 0;
}

int compare_image(int height, int width,
                  float image1[], float image2[]){
    for (int pcount=0;pcount<height*width;pcount++){
        if (fabs(image1[pcount] - image2[pcount])>1){
            printf("pixel %d from image1 differs from image2 by %f\n",
                   pcount,image1[pcount]-image2[pcount]);
            return 1;
        }
    }
    return 0;
}
    
/*************************************************************************
                           scalar function
**************************************************************************/
#pragma GCC push_options
#pragma GCC target ("no-sse")

int scal_rgb2gray(int height, int width,
                  unsigned char img_red[],unsigned char img_green[], unsigned char img_blue[],
                  float img_gray[]){
    for (int pcount= 0; pcount<height*width; pcount++){
        img_gray[pcount]= Y_RED_SCALE * img_red[pcount]+
                          Y_GREEN_SCALE * img_green[pcount]+
                          Y_BLUE_SCALE * img_blue[pcount];
    }
    return 0;
}

int simd_rgb2gray(int height, int width, unsigned char img_red[],unsigned char img_green[], 
                    unsigned char img_blue[], float img_gray[]) {
    //Vector of red, green and blue scale
    __m256i redScale = _mm256_set1_epi16(Y_RED_SCALE);
    __m256i greenScale = _mm256_set1_epi16(Y_GREEN_SCALE);
    __m256i blueScale = _mm256_set1_epi16(Y_BLUE_SCALE);
    
    for (int pcount = 0; i < height*width; i += 32) {
        //Load of r,g,b pixel vector
        __m256i r = _mm256_load_si256((__m256i *) (img_red+i));
        __m256i g = _mm256_load_si256((__m256i *) (img_green+i));
        __m256i b = _mm256_load_si256((__m256i *) (img_blue+i));
        
        //Multiply each color vector with his scale vector
        rr =_mm256_mullo_epi16(r,redScale);
        gg =_mm256_mullo_epi16(g,greenScale);
        bb =_mm256_mullo_epi16(b,blueScale);
        
        //Add each color vector to make grey vector
        g1=_mm256_add_epi16(rr,gg);
        gray=_mm256_add_epi16(g1,bb);
        //store 
        _mm256_store_si256((__m256i *)(img_gray+i),gray);
        
    }
    return 0;
}

#pragma GCC pop_options    
/*************************************************************************
                           intrinsics function
**************************************************************************/
// print 8 floats contained in a 256 bit register
void printf_m256(char *msg, __m256 var){
    
    float *val = (float *) &var;
    printf("%s ",msg);
    for (int i=0; i<8; i++){
        printf("%f ",val[i]);
    }
    printf("\n");
}

// print 8 integers contained in a 256 bit register
void printf_m256i(char *msg, __m256i var){
    
    int *val = (int *) &var;
    printf("%s ",msg);
    for (int i=0; i<8; i++){
        printf("%d ",val[i]);
    }
    printf("\n");
}

/*************************************************************************
                           main function
**************************************************************************/

int height, width;
    
unsigned char image_red[N] ;
unsigned char image_green[N] ;
unsigned char image_blue[N] ;

float scal_gray[N] ;
float simd_gray[N] ;

int main(int argc,char *argv[]){
    
    if (argc<2){
        printf("error: missing loops number on command line, usage ./gray <n>\n");
        return 1;
    }
    const int nb_loops= atoi(argv[1]);
    
    if (read_color_ppm("lena_color.ppm",&height,&width,image_red,image_green,image_blue)){
        return 2;
    }
        
    for(int i=0; i<nb_loops; i++){
        start_timer();
        scal_rgb2gray(height,width,image_red,image_green,image_blue,scal_gray);
        pause_timer();
    }

    if (write_gray_ppm("lena_gray1.ppm",height,width,1,scal_gray)){
        return 3;
    }
    
    display_stats("Scal stats",nb_loops);
    reset_stats();
    
    // complete here
    
    if (write_gray_ppm("lena_gray2.ppm",height,width,1,simd_gray)){
        return 5;
    }
    
    display_stats("SIMD stats",nb_loops);
    if (compare_image(height,width,scal_gray,simd_gray)){
        printf("*** error ****\n");
        return 6;
    }else{
        printf("OK\n");
        return 0;
    }
}
