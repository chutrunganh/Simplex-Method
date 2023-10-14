//C
#include <stdio.h>
#include <stdbool.h>
#define MAX_CONSTRAINTS 10
#define MAX_VARIABLES 10

// Function to initialize the tableau
// row = numConstraints + 1
// col = numVariables + numConstraints + 2
void initializeTableau(float tableau[MAX_VARIABLES][MAX_CONSTRAINTS], int numConstraints, int numVariables) {

    // Fill in the objective function coefficients
    float coe_objective_func;
    puts("Enter the coefficients of the objective function: ");
    for (int i = 1; i <= numVariables; i++) {
        scanf("%f", &coe_objective_func);
        tableau[numConstraints+1][i] = (coe_objective_func)*(-1); //Last row
    }

    // Fill in the coefficients of the constraints
    puts("Enter the coefficients of the constraints: ");
    for (int i = 1; i <= numConstraints; i++) {
        for (int j = 1; j <= numVariables; j++) {
            float coe_constraint_func;
            scanf("%f", &coe_constraint_func);
            tableau[i][j] = coe_constraint_func;
        }
    }

   
    // Initialize slack variables and right-hand side (RHS)
    puts("Enter the RHS of the constraints: ");
    for (int i = 1; i <= numConstraints; i++) {
        for (int j = numVariables+1; j <= numVariables + numConstraints; j++) {
            tableau[i][j] = (i == j - numVariables) ? 1.0 : 0.0;
        }
        float RHS;
        scanf("%f", &RHS);
        tableau[i][numVariables + numConstraints+2] = RHS;  //last column
        tableau[numConstraints+1][numVariables + numConstraints+2] = 0;
    }

    // z column
    for (int i = 1; i <= numConstraints + 1; i++) {
        tableau[i][numConstraints+numVariables+1] = 0;
        tableau[numConstraints+1][numConstraints+numVariables+1] = 1;
    }
}

// Function to display the tableau
void showTableau(float tableau[MAX_VARIABLES][MAX_CONSTRAINTS], int numVariables, int numConstraints) {
    
    //printf x1,x2,...
    for (int i = 1; i <= numVariables; i++) {
        printf("  x%d  ",i);
    }
    //printf s1,s2,...
    for (int i = 1; i <= numConstraints; i++) {
        printf("  s%d  ",i);
    }
    //printf z, RHS
    printf("%6s","z");
    printf("%6s","RHS");
    puts("");
    
    //printf value in tableau
    for (int i = 1; i <= numConstraints+1; i++) {
        for (int j = 1; j <= numConstraints+numVariables+2; j++) {
            printf("%6.1f", tableau[i][j]);
        }
        puts("");
    }
    puts("---------------------------------------------");
}

//Check for all number in last row non-negative
bool checkAllNumberInLastRow(float tableau[MAX_VARIABLES][MAX_CONSTRAINTS], int numVariables, int numConstraints ) {
    for (int i = 1; i <= numVariables + numConstraints; i++) {
        float temp = tableau[numConstraints+1][i];
        if (temp < 0.000000){
                return false;
        }
    }
    return true;
}

//Find the smallest number in the last row (the pivot column)
int smallestNumberInLastRowIndex(float arr[MAX_VARIABLES], int numVariables, int numConstraints) {
    float smallestNumber = arr[1];
    int index = 1;
    for (int i = 1; i <= numVariables + numConstraints; i++) {
        if (arr[i] < smallestNumber) {
            smallestNumber = arr[i];
            index = i;
        }
    }
    return index; //return the position of the smallest negative number
}

//Find the smallest non-negative indicator (the pivot row)
int smallestIndicatorIndex(float tableau[MAX_VARIABLES][MAX_CONSTRAINTS], int numVariables, int numConstraints, int pivot_col_index ) {
    float indicator[MAX_CONSTRAINTS];

    for (int i = 1; i <= numConstraints; i++ ) {  //indicator = RHS / value in corresponding pivot column
        indicator[i] = (float)tableau[i][numConstraints+numVariables+2] / tableau[i][pivot_col_index];
    }
    
    //find smallest indicator index
    float smallestIndicator = 10000000.0;
    int index = 1;
    for (int i = 1; i <= numConstraints; i++) {
        if (indicator[i] >= 0.000000 && smallestIndicator > indicator[i] ) {
            smallestIndicator = indicator[i];
            index = i;
        }
    }
    return index; //return the position of the smallest non-negative number
}

//Update the new tableau
void makeNewTableau(float tableau[MAX_VARIABLES][MAX_CONSTRAINTS], int numVariables, int numConstraints, int pivot_row_index, int pivot_col_index, float pivot_variable) {
   for (int i = 1; i <= numVariables+numConstraints+2; i++) {
        tableau[pivot_row_index][i] = (float)tableau[pivot_row_index][i] / pivot_variable;
   }

    for (int i = 1; i <= numConstraints+1; i++) {
        if (i != pivot_row_index) { //not the pivot row then do this
            
            // Calculate the new value for each element in the row
            // new value = -(value in the column contains pivot, with corresponding row) * (value in the new tablue in pivot row,, with corresponding row)+ current value
            float temp = tableau[i][pivot_col_index] * (-1);
            for (int j = 1; j <= numVariables+numConstraints + 2; j++) {
                tableau[i][j] = tableau[i][j] + temp * tableau[pivot_row_index][j];
            }
            /*use directly tableau[i][j] = tableau[i][j] + -tableau[i][pivot_col_index] * tableau[pivot_row_index][j]; leads to incorrect answer, becauseC may not
            handle the subtraction of -tableau[i][pivot_col_index] as you intend due to potential precision issues with
            floating-point numbers. It can lead to small inaccuracies in the result, especially when dealing with very small or very large numbers */
        }
    }
}

//Check for UNBOUNDED function
bool checkUnbound(float tableau[MAX_VARIABLES][MAX_CONSTRAINTS],int numVariables,int numConstraints, int pivot_col_index) {
    for (int row = 1; row <= numConstraints+1; row ++) {
        if (tableau[row][pivot_col_index] >= 0) {
            return false;
        }
    }
    return true;
}

//Print out the solution
int checkBasic(float arr[MAX_CONSTRAINTS], int numConstraints) {
    int count = 0;
    int index = -1;
    for (int i = 1; i <= numConstraints+1; i++) {
        if (arr[i] != 1 && arr[i] != 0) {
            return -1;   //The column contain number that is not 0 and 1
        }
        else {
            if (arr[i] == 1) {
                count++;
                index = i;
                
            }
        }
    }
    
    if (count == 1) {
        return index;
    }
    else {  //The number 1 appears more than once
        return -1;
    }
}

void showSolution(float tableau[MAX_VARIABLES][MAX_CONSTRAINTS], int numVariables, int numConstraints) {
    
    printf("Optimal solution z max equal to %.1f at: ", tableau[numConstraints+1][numConstraints+numVariables+2]);
    for (int i = 1; i <= numVariables; i++) {
        float x_column[MAX_CONSTRAINTS];  //Transpose array of x_i column
        for (int j = 1; j <= numConstraints+1; j++) {
            x_column[j] = tableau[j][i];
        }
       
        int index = checkBasic(x_column,numConstraints);
       
        printf("x%d = %.1f, ",i,tableau[index][numConstraints+numVariables+2]);

    }
}


int main() {

    float tableau[MAX_VARIABLES][MAX_CONSTRAINTS];
    
    int numVariables, numConstraints;
    puts("Enter number of variables: ");
    scanf("%d",&numVariables);
    puts("Enter number of constraints: ");
    scanf("%d",&numConstraints);
    
    initializeTableau(tableau,numConstraints,numVariables);
    puts("Original tableau");
    showTableau(tableau, numVariables, numConstraints);
    puts("");


    int is_unbound = 0;
    while(checkAllNumberInLastRow(tableau,numVariables, numConstraints) != true) {
        
        //find pivot variable
        int pivot_col_index = smallestNumberInLastRowIndex(tableau[numConstraints+1], numVariables, numConstraints);
        
        
        // check for unbound function
        if (checkUnbound(tableau,numVariables, numConstraints, pivot_col_index)) {
            is_unbound = 1;
            break;
        }
            
        int pivot_row_index = smallestIndicatorIndex(tableau, numVariables, numConstraints, pivot_col_index);
        float pivot_variable = tableau[pivot_row_index][pivot_col_index];
        printf("\nPivot variable: %.1f\n",pivot_variable);

        makeNewTableau(tableau, numVariables, numConstraints, pivot_row_index, pivot_col_index, pivot_variable);
        puts("New tables");
        showTableau(tableau,numVariables,numConstraints);
    

        }
        
    if (is_unbound == 1) {
        puts("UNBOUNDED");
    }
    else {
        puts("END");
        showSolution(tableau,numVariables,numConstraints);
    }
        
        
}
