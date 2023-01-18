#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#define SIZE 25
#define UNASSIGNED 0

clock_t start, end;
FILE *out;

FILE *safe_open(char* name, char* mode){
    FILE *f = fopen(name, mode);
    if(f==NULL){
        printf("File %s could not be opened!\n",name );
        exit(EXIT_FAILURE);
    }
    return f;
}

void print_grid(int grid[SIZE][SIZE]) {
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            printf("%4d", grid[row][col]);
            if((col+1)%(int)sqrt(SIZE)==0) printf("\t");

        }
        printf("\n");
        if((row+1)%(int)sqrt(SIZE)==0) printf("\n");
    }
}

void fprint_grid(FILE *f, int grid[SIZE][SIZE]) {
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            fprintf(f, "%4d", grid[row][col]);
            if((col+1)%(int)sqrt(SIZE)==0) fprintf(f, "\t");

        }
        fprintf(f,"\n");
        if((row+1)%(int)sqrt(SIZE)==0) fprintf(f, "\n");
    }
}

int readmatrix(int a[SIZE][SIZE], char* filename)
{
    

    FILE *pf = safe_open(filename, "r");

    for(int row = 0; row < SIZE; row++)
    {
        for(int col = 0; col < SIZE; col++)
        {
            fscanf(pf, "%d", &(a[row][col]));
            

        }
    }
    fclose (pf); 
    return 1; 
}

int is_exist_row(int grid[SIZE][SIZE], int row, int num){
    for (int col = 0; col < SIZE; col++) {
        if (grid[row][col] == num) {
            return 1;
        }
    }
    return 0;
}

int is_exist_col(int grid[SIZE][SIZE], int col, int num) {
    for (int row = 0; row < SIZE; row++) {
        if (grid[row][col] == num) {
            return 1;
        }
    }
    return 0;
}

int is_exist_box(int grid[SIZE][SIZE], int startRow, int startCol, int num) {
    for (int row = 0; row < (int)sqrt(SIZE); row++) {
        for (int col = 0; col < (int)sqrt(SIZE); col++) {
            if (grid[row + startRow][col + startCol] == num) {
                return 1;
            } 
        }
    }
    return 0;
}

int is_safe_num(int grid[SIZE][SIZE], int row, int col, int num) {
    return !is_exist_row(grid, row, num) 
            && !is_exist_col(grid, col, num) 
            && !is_exist_box(grid, row - (row % (int)sqrt(SIZE)), col - (col %(int)sqrt(SIZE)), num);
}

int find_unassigned(int grid[SIZE][SIZE], int *row, int *col) {
    for (*row = 0; *row < SIZE; (*row)++) {
        for (*col = 0; *col < SIZE; (*col)++) {
            if (grid[*row][*col] == 0) {
                return 1;
            }
        }
    }
    return 0;
}


int solve(int grid[SIZE][SIZE], int level, FILE* f) {
    
    int row = 0;
    int col = 0;
    
    if (!find_unassigned(grid, &row, &col)){
        return 1;
    }
    int correct = 0;
    for (int num = 1; num <= SIZE; num++ ) {        
        if (is_safe_num(grid, row, col, num)) {
            int val = 0;
	
           #pragma omp task firstprivate(grid, row, col,val,num, level) final(level>2) 
            {
                       
            // fprintf(f, "id=%d, level %d, PLACE %d, %d fit %d\n", omp_get_thread_num(), level, row, col, num);
                int copy_grid[SIZE][SIZE];
                for (int row = 0; row < SIZE; row++) {
                    for (int col = 0; col < SIZE; col++) {                      
                        copy_grid[row][col] = grid[row][col];
                    }                   
                }  
                copy_grid[row][col] = num;
                     
                val = solve(copy_grid, ++level, f);

                
                if(val) {
                    #pragma omp critical
                    {
                    correct=1;
                    fclose(out);
                    print_grid(copy_grid);
                    end = clock();
                    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;  
                    printf("\nIzvrsava se %f s\n",time_spent);                    
                    exit(0);  
                    }
                                      
                }
            }   
            grid[row][col] = UNASSIGNED;

        }

    }
            #pragma omp taskwait

    
    return correct;
}

int main(int argc, char** argv) {
    
    int sudoku[SIZE][SIZE];

    readmatrix(sudoku, argv[1]);
        
    printf("Size: %d", SIZE);   
    printf("\n");
    

        
    start = clock();
    printf("Solving Sudoku: \n");
    print_grid(sudoku);
    printf("---------------------\n");
    out=safe_open("logs.txt", "w");
    
    #pragma omp parallel num_threads(6)
    #pragma omp single nowait
    {
        solve(sudoku, 1, out);   
    }
    printf("Pre exit-a.\n");
    exit(EXIT_SUCCESS);
}
