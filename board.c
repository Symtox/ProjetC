#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>


// int randomBoard (){
//     char board[30][30];

//     for (int i = 0; i < 30; i++) {
//         for (int j = 0; j < 30; j++){
//             if(i == 0 || i == 29 || j == 0 || j == 29){
//                 board[i][j] = 1;
//             }else {
//                 board[i][j] = 0;
//             }
//         }
//     }

//     for (int i = 0; i < 30; i++) {
//         for (int j = 0; j < 30; j++) {
//             printf("%d", board[i][j]);
//         }
//         printf("\n");
//     }
//     return 0;
// }




// void readCSV(char* filename){
//     char board[30][30];

//     FILE * file = fopen("board.csv", "r");
//     if (file == NULL){
//         printf("Erreur lors de l'ouverture du fichier");
//         return;
//     }

//     char ligne[1024];
//     fgets(ligne, 1024, file);
//     memmove(ligne, ligne+3, strlen(ligne)-2); // supprime les 3 premiers caractères (BOM) de la première ligne
//     const char * separators = ";"; // séparateur de colonnes
//     char * strtoken = strtok(ligne, separators); // découpe la ligne en colonnes

//     while(strtoken){//boucle tant que le token n'est pas NULL
//         printf("%s", strtoken); 
//         strtoken = strtok(NULL, separators);
//     }

//     while(fgets(ligne, 1024, file)){//boucle sur chaque ligne du fichier (à partir de la deuxième ligne)
//         strtoken = strtok(ligne, separators);// découpe la ligne en "tokens" séparés par le point-virgule et stocke le premier token dans la variable "strtoken"

//         while(strtoken){
//             printf("%s", strtoken);
//             strtoken = strtok(NULL, separators);
//         }
//     }
//     printf("\n");
//     fclose(file);
// }

void readCSV(char* filename){
    srand(time(NULL));
    double random_number = (double)rand() / RAND_MAX;
    char board[30][30];
    // row = 0  ou 40
    // col = 0 ou 33
    // int row[2]={0,40};
    // int col[2]={0,33};


    FILE * file = fopen(filename, "r");
    if (file == NULL){
        printf("Erreur lors de l'ouverture du fichier");
        return;
    }

    char ligne[1024];
    const char * separators = ";"; // séparateur de colonnes
    int row = 1;

    while(row <= 30 && fgets(ligne, 1024, file)){//boucle sur chaque ligne du fichier (à partir de la première ligne)
        if (row == 1) {
            memmove(ligne, ligne+3, strlen(ligne)-2); // supprime les 3 premiers caractères (BOM) de la première ligne
        }
        char * strtoken = strtok(ligne, separators);// découpe la ligne en "tokens" séparés par le point-virgule et stocke le premier token dans la variable "strtoken"
        int col = 1;

        while(strtoken){
            if (row <= 30 && col <= 30) {
                board[row-1][col-1] = *strtoken;
            }
            col++;
            strtoken = strtok(NULL, separators);
        }
        row++;
    }

    for (int i = 0; i < 30; i++) {
        for (int j = 0; j < 30; j++) {
            printf("%c", board[i][j]);
        }
        printf("\n");
    }

    fclose(file);
}


int main ()
{
//    randomBoard();
   readCSV("board.csv");
    return 0;
}