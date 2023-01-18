#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#define SIZE 25
#define UNASSIGNED 0

clock_t start, end;

FILE *safe_open(char* name, char* mode){
    FILE *f = fopen(name, mode);
    if(f==NULL){
        printf("File %s could not be opened!\n",name );
        exit(EXIT_FAILURE);
    }
    return f;
}


int readmatrix(int a[SIZE][SIZE], char* filename)
{
    int una=0;

    FILE *pf = safe_open(filename, "r");

    for(int row = 0; row < SIZE; row++)
    {
        for(int col = 0; col < SIZE; col++)
        {
            fscanf(pf, "%d", &(a[row][col]));
            if(a[row][col]==0) una++;

        }
    }
    fclose (pf); 
    return una; 
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

int is_exist_row(int grid[SIZE][SIZE], int row, int num){
    for (int col = 0; col < SIZE; col++) {
        if (grid[row][col] == num) {
            return 1;
        }
    }
    return 0;
}

int is_exist_col(int grid[SIZE][SIZE], int col, int num) {
    int exists =0;
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


int solve(int grid[SIZE][SIZE]) {
    
    int row = 0;
    int col = 0;
    
    if (!find_unassigned(grid, &row, &col)){
        return 1;
    }
    

    for (int num = 1; num <= SIZE; num++ ) {        
        if (is_safe_num(grid, row, col, num)) {
            int val = 0;
            {
                int copy_grid[SIZE][SIZE];
                for (int row = 0; row < SIZE; row++) {
                    for (int col = 0; col < SIZE; col++) {                      
                        copy_grid[row][col] = grid[row][col];
                    }                   
                }
                
                copy_grid[row][col] = num;              
                val = solve(copy_grid);
                
                if(val) {
                    print_grid(copy_grid);
                    end = clock();
                    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;  
                    printf("\nProgram se izvrsavao %f sekundi.\n",time_spent);                    
                    exit(0);                    
                }
            }                       
        
            
            grid[row][col] = UNASSIGNED; //back to unasigned because next is not valid
        }
    }
    
    return 0;
}


int main(int argc, char** argv) {
    
    int sudoku[SIZE][SIZE];
    int number_of_un;
    number_of_un = readmatrix(sudoku, argv[1]);

    

        
    printf("Size: %d", SIZE);   
    printf("Unassigned %d\n", number_of_un);
    printf("\n");
        
    start = clock();
    printf("Solving Sudoku: \n");
    print_grid(sudoku);
    printf("---------------------\n");
    solve(sudoku);   
    exit(EXIT_SUCCESS);
}
