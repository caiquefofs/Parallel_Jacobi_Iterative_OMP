#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>
#define e 0.0000001

//função para criar valores pseudoaleatorios para a diagonal
long double geraDiagonal(int seed){
    return (seed*13+355);
}

//função para criar valores pseudoaleatorios para o restante da matriz
long double geraNumeros(int seed){
    return seed-20; 
}

//função para criar valores pseudoaleatorios para o vetor B
int geraB(int seed){
    return (13*seed + 7) % 1031;
}

int main(int argc, char *argv[]){
    int seed, semente, n, i, j, k = 0, T;
    long double erro = 1, soma, erroParcial;
    double wtime1, wtime2;

    //Inicia a contagem de tempo
    wtime1 = omp_get_wtime();

    n = atoi(argv[1]);
    T = atoi(argv[2]);
    seed = atoi(argv[3]);

    srand(seed);

    //alocação de memória para a Matriz A, Vetor B e o Vetor X;
    long double** A = (long double**)calloc(n, sizeof(long double*));
    for(i=0; i<n; i++){
        A[i] = (long double*)calloc(n, sizeof(long double));
    }

    long double* B = (long double*)calloc(n, sizeof(long double));

    long double* X_atual = (long double*)calloc(n, sizeof(long double));
    long double* X_anterior = (long double*)calloc(n, sizeof(long double));

    //Inicializa as matrizes

    //#pragma omp parallel for private(i, j) shared(A) num_threads(T)
    for(i=0;i<n;i++){
        semente = rand() % 100;
        for(j=0; j<n; j++){
            if(i==j){
                A[i][j] = geraDiagonal(semente);
            }else{
                A[i][j] = geraNumeros(semente);
            }
        }
    }

    //#pragma omp parallel for private(i) shared(B) num_threads(T)
    for(i=0;i<n;i++){
        B[i] = geraB(semente);
        semente = rand() % 100;
    }

    //#pragma omp parallel for private(i) shared(X_atual, X_anterior) num_threads(T)
    for(i=0;i<n;i++){
        X_atual[i] = 0;
        X_anterior[i] = 0;
    }

    /*//Código para imprimir a matriz A e o vetor B
    printf("A = \n");
    for(i=0;i<n;i++){
        for(j=0; j<n; j++){
            printf(" %Lf ", A[i][j]);
        }
        printf("\n");
    }
    printf("B = ");
    for(i=0;i<n;i++){
        printf(" %Lf ", B[i]);
    }
    printf("\n");*/
    

    k = 0;
    while(erro > e){
        erroParcial = 0;
        erro = 0;

        //Número máximo de iterações
        if(k>200){
            break;
        }

        //Começa as iterações
        #pragma omp parallel for private(i, j, soma) shared(X_atual, X_anterior) num_threads(T)
        for(i=0;i<n;i++){
            soma=0;
            for(j=0;j<n;j++){
                if(i!=j){
                    soma += (A[i][j]/A[i][i])*X_anterior[j];
                }   
            }
            soma += (B[i]/A[i][i]);
            X_atual[i] = soma;
            //printf("X%d = %Lf ; ",i,X_atual[i]);
        }
        //printf("\n");

        //Calcula o erro
        for(i=0;i<n;i++){
            erroParcial = fabs(X_atual[i] - X_anterior[i]);
            if(erroParcial>erro){
                erro = erroParcial;
            }
            X_anterior[i] = X_atual[i];
        }

        k++;
    }

    //Término da contagem de tempo
    wtime1 = omp_get_wtime() - wtime1;

    /*
        printf("\nO melhor resultado é: \n");
    for(i=0; i<n; i++){
        printf("x%d = %Lf ", i, X_atual[i]);
    }
    printf("\n");
    */

    /*wtime2 = omp_get_wtime();

    //Comparação de resultado
    int escolha;
    printf("\nQual equação você deseja testar?\n");
    scanf("%d", &escolha);

    printf("\nA equação escolhida foi: \n");
    printf("%d) ", escolha);
    for(j=0; j<n; j++){
        if(j != (n-1)){
            printf("%0.Lf x%d + ", A[escolha][j], j);
        }else{
            printf("%0.Lf x%d = ", A[escolha][j], j);
        }
    }
    printf("%0.Lf \n", B[escolha]);

    long double conta = 0;

    for(j=0; j<n; j++){
            conta += A[escolha][j]*X_atual[j];
    }

    printf("\nResultado ideal da equação escolhida: %Lf\n", B[escolha]);
    printf("Resultado obtido da equação escolhida: %Lf \n\n", conta);*/


    //liberando memória
    for(i=0; i<n; i++){
        free(A[i]);
    }
    free(A);
    free(B);
    free(X_anterior);
    free(X_atual);

    wtime2 = omp_get_wtime() - wtime2;
    printf("O tempo foi de: %lf\n", (wtime1));

    return 0;
}