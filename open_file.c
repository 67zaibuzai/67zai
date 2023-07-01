#include <stdio.h>
#include <stdlib.h>

typedef struct tree
{
    int pid;
    struct tree *lchild;
    struct tree *sibling;
    struct tree *father;
} tree;

void quicksort(int *pid, int *ppid, char *name[], int front, int rear);
void build_tree(int *pid, int rn, int *ppid, tree *node);
void printf_tree(tree *head, int count);

// TODO:in accord to the number of row, get the precise number to build;
void chartoint(char **pid, int rn, int *n_pid, int limit) //
{
    // i-for the pid of every thread
    int i = 0;
    // j-for the every char of the thread
    int j = 0;
    int sum = 0;
    for (i; i < rn; i++)
    {
        for (j = 0; j < limit; j++)
        {
            if (pid[i][j] <= '9' && pid[i][j] >= '0')
            {
                int k = limit - j;
                int m = 1;
                for (k; k > 1; k--)
                {
                    m *= 10;
                }
                sum += (pid[i][j] - '0') * m;
            }
        }
        n_pid[i] = sum;
        sum = 0;
    }
}

int main()
{
    FILE *file = fopen("a", "r");
    if (file == NULL)
    {
        printf("Failed to open this file");
        return 0;
    }

    char **buffer = (char **)malloc(sizeof(char *) * 50);
    int i;
    for (i = 0; i < 50; i++)
    {
        buffer[i] = (char *)malloc(sizeof(char) * 2048);
    }
    i = 0;
    int rn = 0; // the aggregate row number
    while (fgets(buffer[i++], 2048, file)) // get content row by row
        rn += 1;
    fclose(file);
    rn = rn - 1;
    // build the vessel to store key information
    char **name;
    name = (char **)malloc(sizeof(char *) * rn);
    for (i = 0; i < rn; i++)
    {
        name[i] = (char *)malloc(sizeof(char) * 9);
    }
    char **pid;
    pid = (char **)malloc(sizeof(char *) * rn);
    for (i = 0; i < rn; i++)
    {
        pid[i] = (char *)malloc(sizeof(char) * 6);
    }
    char **ppid;
    ppid = (char **)malloc(sizeof(char *) * rn);
    for (i = 0; i < rn; i++)
    {
        ppid[i] = (char *)malloc(sizeof(char) * 6);
    }
    // over
    int j;
    // fill in key information
    for (i = 1; i <= rn; i++)
    {
        int k = 0;
        j = 4;
        while (j < 12 && buffer[i][j] != '\0')
        {
            name[i - 1][k] = buffer[i][j++];
            k++;
        }
        name[i - 1][k] = '\0';
        j = 13;
        k = 0;
        while (j < 18)
        {
            pid[i - 1][k] = buffer[i][j++];
            k++;
        }
        pid[i - 1][k] = '\0';
        j = 19;
        k = 0;
        while (j < 24)
        {
            ppid[i - 1][k] = buffer[i][j++];
            k++;
        }
        ppid[i - 1][k] = '\0';
    }
    // over
    // build numerical pid array
    int *n_pid;
    n_pid = (int *)malloc(sizeof(int) * rn);
    int *n_ppid;
    n_ppid = (int *)malloc(sizeof(int) * rn);
    // over
    chartoint(pid, rn, n_pid, 5);
    chartoint(ppid, rn, n_ppid, 5);
    // quicksort
    quicksort(n_pid, n_ppid, name, 0, rn - 1);
    // eliminate the space
    for (i = 0; i < rn - 1; i++)
    {
        free(pid[i]);
        free(ppid[i]);
    }
    // build connection between pid and ppid
    tree *node = (tree *)malloc(sizeof(tree) * rn);
    if (node == NULL)
    {
        printf("memory allocation failure");
        exit(0);
    }
    for (i = 0; i < rn; i++)
    {
        node[i].pid = n_pid[i];
        node[i].lchild = NULL;
        node[i].sibling = NULL;
        if (n_ppid[i] == 0)
        {
            node[i].father = NULL;
        }
        else if (n_ppid[i] == n_pid[i - 1])
        {
            node[i].father = (node + i - 1);
        }
        else if (n_ppid[i] == n_ppid[i - 1])
        {
            node[i].father = node[i - 1].father;
        }
        else
        {
            int n = i;
            int a = n_ppid[i];
            while (n >= 0)
            {
                if (a == n_pid[n--])
                {
                    break;
                }
            }
            if (n_ppid[i] == n_pid[n + 1])
            {
                node[i].father = (node + n + 1);
            }
            else
            {
                printf("unknown node");
                exit(0);
            }
        }
    }
    // over
    build_tree(n_pid, rn - 1, n_ppid, node);
    printf_tree(node, 0);

    return 0;
}
// quicksort
void quicksort(int *pid, int *ppid, char *name[], int front, int rear)
{
    if (front >= rear)
    {
        return;
    }
    int f = front;
    int r = rear;
    int para = ppid[front];
    while (front <= rear)
    {
        // rear
        while (front <= rear && ppid[rear] > para)
        {
            rear--;
        }
        while (front <= rear && ppid[front] <= para)
        {
            front++;
        }
        if (front < rear)
        {
            // for ppid
            int temp = ppid[front];
            ppid[front] = ppid[rear];
            ppid[rear] = temp;
            // for pid
            temp = pid[front];
            pid[front] = pid[rear];
            pid[rear] = temp;
            // for name
            char *tempc;
            tempc = (char *)malloc(8 * sizeof(char));
            tempc = name[front];
            name[front] = name[rear];
            name[rear] = tempc;
            free(tempc);
        }
    }

    int temp = ppid[rear];
    ppid[rear] = ppid[f];
    ppid[f] = temp;
    // for pid
    temp = pid[rear];
    pid[rear] = pid[f];
    pid[f] = temp;
    // for name
    char *tempc;
    tempc = (char *)malloc(8 * sizeof(char));
    tempc = name[f];
    name[f] = name[rear];
    name[rear] = tempc;
    free(tempc);

    quicksort(pid, ppid, name, f, rear - 1);
    quicksort(pid, ppid, name, front, r);
}

void build_tree(int *pid, int rn, int *ppid, tree *node)
{
    int i;
    tree *p = node;
    for (i = 1; i < rn; i++)
    {
        if (pid[i - 1] == ppid[i])
        {
            node[i].sibling = node[i - 1].lchild;
            node[i - 1].lchild = node + i;
        }
        else if (ppid[i] == ppid[i - 1])
        {
            node[i].sibling = node[i - 1].sibling;
            node[i - 1].sibling = node + i;
        }
        else
        {
            tree *p = node[i].father;
            node[i].sibling = p->lchild;
            p->lchild = node + i;
        }
    }
}

void printf_tree(tree *head, int count)
{
    // child node is the most important
    if (head == NULL)
    {
        return;
    }
    int col;
    int i = 0;
    for (i = 0; i < count; i++)
    {
        printf("  ");
    }
    printf("└─%d\n", head->pid);
    if (head->lchild)
        printf_tree(head->lchild, count + 1);
    if (head->sibling)
        printf_tree(head->sibling, count);
}
