#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "param.h"

int main(){

    FILE *file;
    //int *p = malloc(sizeof(int) * 1000);

    int *data = (int *)malloc(sizeof(int) * INPUT_W * INPUT_H * INPUT_C);
    int *weight = (int *)malloc(sizeof(int) * F_SIZE * F_SIZE * INPUT_C * F_NUM);
    int *output = (int *)malloc(sizeof(int) * OUTPUT_W * OUTPUT_H * F_NUM);
    // int data[INPUT_W * INPUT_H * INPUT_C] = {0};
    // int weight[F_SIZE * F_SIZE * INPUT_C * F_NUM] = {0};
    // int output[OUTPUT_W * OUTPUT_H * F_NUM] = {0};

    file = fopen("input.txt", "wt");
    srand( time(NULL) );

    for(int i = 0; i < (INPUT_W * INPUT_H * INPUT_C); i++){
        data[i] = rand()%11-5;
        fprintf(file, "%d\n", data[i]);
    }
    fclose(file);

    file = fopen("weight.txt", "wt");
    for(int i = 0; i < (F_SIZE*F_SIZE*INPUT_C*F_NUM); i++){
        weight[i] = rand()%3-1;
        fprintf(file, "%d\n", weight[i]);
    }
    fclose(file);

    file = fopen("output.txt", "wt");
    for(int f_ch = 0; f_ch < F_NUM; f_ch++){
        for(int out_h = 0; out_h < OUTPUT_H; out_h++){
            for(int out_w = 0; out_w < OUTPUT_W; out_w++){
                for(int in_c = 0; in_c < INPUT_C; in_c++){ //filter channel
                    for(int f_h = 0; f_h < F_SIZE; f_h++){
                        for(int f_w = 0; f_w < F_SIZE; f_w++){
                            output[f_ch * OUTPUT_W * OUTPUT_H + out_h * OUTPUT_W + out_w] = output[f_ch * OUTPUT_W * OUTPUT_H + out_h * OUTPUT_W + out_w] + 
                                                        data[(in_c * INPUT_H * INPUT_W) + out_h * INPUT_W + out_w + f_h * INPUT_W + f_w] * 
                                                        weight[(f_ch * INPUT_C * F_SIZE * F_SIZE) + (in_c * F_SIZE * F_SIZE) + (F_SIZE * f_h) + f_w]; 
                        }
                    }
                }                fprintf(file, "%d\n", output[f_ch * OUTPUT_W * OUTPUT_H + out_h * OUTPUT_W + out_w]);
            }
        }
    }    


}