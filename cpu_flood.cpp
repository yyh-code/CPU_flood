#include <stdio.h>
#include <malloc.h>
#include <windows.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

int M;   
int N;   
double dx, dy;   
double xllcorner;
double yllcorner;
int nodata;
int ny, nx;
char ncols[15];
char nrows[15];
char xllcorner_label[15];
char yllcorner_label[15];
char cellsize[15];
char NODATA_value[15];
int *fd;
int *fa;
int *fa_pre;
int *fa_tmp;
int *count;
int *count_pre;
int count_x;

void gridread()    /*读取流向文件*/
{
	int i, j;
	char infile[10];
	FILE *fp;
	printf("输入流向文件名：");
	scanf("%s", infile);
	fp = fopen(infile, "r");
	if (fp == NULL)
	{
	printf("cannot open file\n");
	return;
	}
	fscanf(fp, "%s %d", &ncols, &M);
	fscanf(fp, "%s %d", &nrows, &N);
	fscanf(fp, "%s %lf", &xllcorner_label, &xllcorner);
	fscanf(fp, "%s %lf", &yllcorner_label, &yllcorner);
	fscanf(fp, "%s %lf", &cellsize, &dx);
	fscanf(fp, "%s %d", &NODATA_value, &nodata);  
	dy = dx;
	fd = (int*)calloc(N*M, sizeof(int));
	fa = (int*)calloc(N*M, sizeof(int));
	fa_pre = (int*)calloc(N*M, sizeof(int));
	fa_tmp = (int*)calloc(N*M, sizeof(int));
	count = (int*)calloc(N*M, sizeof(int));
	count_pre = (int*)calloc(N*M, sizeof(int));
	for (i = 0; i<N; i++)   
	{
		for (j = 0; j < M; j++)   
		{
			fscanf(fp, "%d ", &fd[i*M + j]);
			fa[i*M + j] = 1;
			fa_tmp[i*M + j] = 1;
			fa_pre[i*M + j] = 0;
			count_pre[i*M + j] = 0;
			count[i*M + j] = 0;
		}
		fscanf(fp, "\n");
	}
	fclose(fp);
}
void flood_cal(int i, int j, int rows, int cols, int k){

	if (i >= rows || j >= cols)
		return;
	int self = i * cols + j;
	int nie, nise, nis, nisw, niw, ninw, nin, nine;
	int count_self = 0, fa_tra1 = 0, fa_pre1 = 0;
	nie = self + 1;
	nise = self + cols + 1;
	nis = self + cols;
	nisw = self + cols - 1;
	niw = self - 1;
	ninw = self - cols - 1;
	nin = self - cols;
	nine = self - cols + 1;
	count_self = count_pre[self];
	if (count_self == k){
		fa_tra1 = fa_tmp[self];
		fa_pre1 = fa_pre[self];
		fa_pre[self] = fa_tra1;
		if (j < cols - 1 && fd[self] == 1){
			fa[nie] = fa[nie] + fa_tra1 - fa_pre1;
			count[nie] = count_self + 1;
		}
		if (i < rows - 1 && j < cols - 1 && fd[self] == 2)
		{
			fa[nise] = fa[nise] + fa_tra1 - fa_pre1;
			count[nise] = count_self + 1;
		}
		if (i<rows - 1 && fd[self] == 4){
			fa[nis] = fa[nis] + fa_tra1 - fa_pre1;
			count[nis] = count_self + 1;
		}
		if (i<rows - 1 && j>0 && fd[self] == 8){
			fa[nisw] = fa[nisw] + fa_tra1 - fa_pre1;
			count[nisw] = count_self + 1;
		}
		if (j>0 && fd[self] == 16){
			fa[niw] = fa[niw] + fa_tra1 - fa_pre1;
			count[niw] = count_self + 1;
		}
		if (i > 0 && j > 0 && fd[self] == 32){
			fa[ninw] = fa[ninw] + fa_tra1 - fa_pre1;
			count[ninw] = count_self + 1;
		}
		if (i > 0 && fd[self] == 64){
			fa[nin] = fa[nin] + fa_tra1 - fa_pre1;
			count[nin] = count_self + 1;
		}
		if (i > 0 && j < cols - 1 && fd[self] == 128){
			fa[nine] = fa[nine] + fa_tra1 - fa_pre1;
			count[nine] = count_self + 1;
		}
		count_x++;
	}
	fa_pre1 = 0, fa_tra1 = 0, count_self = 0;
}

int main(){
	
	gridread();	printf("In process\n");
	clock_t start_time = clock();
	for (int k = 0;; k++)
	{
		count_x = 0;
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < M; j++)
			{
				flood_cal(i, j, N, M, k);
			}
			
		}
		for (int self = 0; self < N*M; self++)
		{
			count_pre[self] = count[self];
			fa_tmp[self] = fa[self];
		}
		if (count_x == 0)
		{
			printf("迭代次数：%d\n", k);
			break;
		}
	}
	clock_t end_time = clock();
	float clockTime = static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC * 1000;
	printf("Running time is:   %3.2f ms\n", clockTime);
	FILE *outfile;
	outfile = fopen("Gridout.txt", "w");
	if (outfile == NULL)
	{
		printf("无法打开文件\n");
	}

	fprintf(outfile, "%s       %d\n", "ncols", M);
	fprintf(outfile, "%s       %d\n", "nrows", N);
	fprintf(outfile, "%s 	%.12lf\n", "xllcorner", xllcorner);
	fprintf(outfile, "%s 	%.12lf\n", "yllcorner", yllcorner);
	fprintf(outfile, "%s 	%.12lf\n", "cellsize", dx);
	fprintf(outfile, "%s  	%d\n", "NODATA_value", nodata);
	for (int i = 0; i<N; i++)
	{
		for (int j = 0; j<M; j++)
		{
			fprintf(outfile, "%d ", fa[i*M + j]);
		}
		fprintf(outfile, "\n");
	}
	fclose(outfile);
	printf("finished!\n");
	free(fa);
	free(fa_pre);
	free(fd);
	free(count);
	free(count_pre);
	free(fa_tmp);
}
