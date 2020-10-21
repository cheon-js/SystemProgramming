#include <stdio.h>
#include <stdlib.h>

int main() {

	int i, j, mat_x, mat_y;
	int *mat1_ptr;
	int *mat2_ptr;
	int *mat_sum_ptr;

	printf("행렬의 크기를 입력 (x, y) : ");
	scanf("%d %d", &mat_x, &mat_y);

	mat1_ptr = (int *)malloc(mat_x * mat_y * sizeof(int));
	mat2_ptr = (int *)malloc(mat_x * mat_y * sizeof(int));
	mat_sum_ptr = (int *)malloc(mat_x * mat_y * sizeof(int));

	if(mat1_ptr == NULL || mat2_ptr == NULL || mat_sum_ptr == NULL) {
		printf("Memory Failed Allocate");
		exit(-1);
	}
	printf("Input Matrix 1 Numbers\n");
	for(i=0; i< mat_x * mat_y; i++)
		scanf("%d", &mat1_ptr[i]);
	printf("Input Matrix 2 Numbers\n");

        for(i=0; i< mat_x * mat_y; i++)
                scanf("%d", &mat2_ptr[i]);

        for(i=0; i< mat_x * mat_y; i++)
                mat_sum_ptr[i] = mat1_ptr[i] + mat2_ptr[i];

	printf("행렬의 합은 \n");
	for(i = 0; i< mat_x; i++){
		printf("\n");
		for(j = 0; j<mat_y; j++)
			printf("%d\n", mat_sum_ptr[i*j]);
	}
	free(mat1_ptr);
	free(mat2_ptr);
	free(mat_sum_ptr);
	
}
