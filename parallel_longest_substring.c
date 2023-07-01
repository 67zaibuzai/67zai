// A_string:length is m, B_string :length is n:
// assume 1 <= i <= m, 1 <= j <= n:
// if the i == m, j == n:two situations:1.A[i] == B[j], the just find the substring of A[i-1] and B[i-1];
//                                      2.A[i] != B[i], you you can choose the longest substring of A[i] and B[i-1]  or A[i-1] and B[i].
// Considering:
// if the Ai != Bj, just to find the Ai-1 and Bi, or Bi-1 and Ai
// look: when A2 = B2, but A2 != B3, you can easily find the longest substring point is A2 and B2.
// look: when A2 != B9, you may to find the relationship of A2 and B8,
// look: but you can observe that the longest length od substirng is 2, because A2 just has the  length of 2
// so you just to document the value of S[2,2], so you can get the longest length

// Considering more:
//  NO NO NO. the assumation just rely on the S[i][j] == min(i , j),and it has zero opportunity of S[i][j] > min(i ,j)
//  when S[i][j] < min(i , j),you nees to remember the S[i, j] of Ai == Bj(i ！= j)
// now you just resolve the problem of deminishing the data dependency of row
//  wait a minut, think about the column is same to row?
// OK, the answer is yes.

// conclusion:
// when the Ai != Bi, you can just document the value of S[a, b] in which the Aa == Bb
// S[i, j] == S[a, b]

// so let's do it:
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

char *SA, *SB;
int la, lb;
int T;
char **score_table;
pthread_mutex_t lk = PTHREAD_MUTEX_INITIALIZER;
sem_t *t_change;

void *get_substring(void *arg)
{
    while (*(int *)arg <= lb)
    {
        int as = *(int *)arg;
        sem_wait(&t_change[as]);
        pthread_mutex_lock(&lk);
        printf("I'm pthread%ld\n", pthread_self());
        int j = 0;
        int anchor = 0;
        int anchor_i = 0;
        // you write the algorithm first, then consider other problem
        for (j = 0; j <= la; j++)
        {
            if (as == 0 | j == 0)
            {
                score_table[as][j] = 0;
            }
            else if (SB[as - 1] == SA[j - 1])
            {
                anchor = score_table[as - 1][j - 1] + 1;
                score_table[as][j] = anchor;
                // just for one time,set a dirty bit for it
                anchor_i = as;
            }
            else
            {
                if (as == anchor_i)
                {
                    int k = score_table[as - 1][j];
                    if (anchor > k)
                        score_table[as][j] = anchor;
                    else
                    {
                        anchor = k;
                        score_table[as][j] = k;
                    }
                }
                else
                    score_table[as][j] = score_table[as - 1][j];

                // the data dependence form the S[i-1][J-1],S[I][J-1] and S[I-1][J] to the S[i-1][j]
                // others you need anchor and S[i-1][j-1] and S[i-1][j]
            }
        }
        *(int *)arg += T;
        pthread_mutex_unlock(&lk);
        sem_post(&t_change[as + 1]);
    }
}

int main(int argc, char **argv)
{
    SA = argv[1];
    SB = argv[2];
    la = strlen(SA);
    lb = strlen(SB);
    T = atoi(argv[3]);
    int i;
    // allocate space to score_table
    score_table = (char **)malloc(sizeof(char *) * (lb + 1));
    for (i = 0; i < lb + 1; i++)
        score_table[i] = (char *)malloc(sizeof(char) * (la + 1));
    if (score_table == NULL)
    {
        printf("warning for space");
        exit(0);
    }
    //
    // build semaphore
    t_change = (sem_t *)malloc(sizeof(sem_t) * (lb + 1));
    sem_init(&t_change[0], 0, 1);
    for (i = 1; i < (lb + 1); i++)
    {
        sem_init(&t_change[i], 0, 0);
    }
    // build threads
    pthread_t *ids;
    ids = (pthread_t *)malloc(sizeof(pthread_t) * T);
    i = 0;
    // build special arguments
    int *a = (int *)malloc(sizeof(int) * T);
    while (i < T)
    {
        a[i] = i;
        pthread_create(&ids[i], NULL, get_substring, (void *)(&a[i]));
        i++;
    }
    // recycle threads
    for (i = 0; i < T; i++)
    {
        pthread_join(ids[i], NULL);
    }
    for (i = 0; i < lb + 1; i++)
    {
        for (int j = 0; j < la + 1; j++)
        {
            printf("%d  ", score_table[i][j]);
        }
        printf("\n");
    }
    free(score_table);
    return 0;
}
