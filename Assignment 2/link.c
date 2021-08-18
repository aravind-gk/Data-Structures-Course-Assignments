#include <stdio.h>
#include <stdlib.h>

#define LL long long
#define LLONG_MIN -9223372036854775807
#define LLONG_MAX 9223372036854775807

typedef struct ListNode
{
    int vertex;
    struct ListNode *next;
} ListNode;

// 'List' holds an adjacency list of one vertex. I'm implementing adjacency list using "linked list of linked list" representation, and NOT "array of linked list" representation
typedef struct List
{
    int vertex; // The vertex number corresponding to the given adjacency list
    int degree; // Holds the degree of the vertex adjacency list
    ListNode *head;
    ListNode *tail; // Tail pointer helps in inserting at end of list in O(1) time
    struct List *nextList;
} List;

typedef struct Graph 
{
    int numberOfVertices;
    int numberOfEdges;
    List *adjList;
} Graph;

typedef struct Link
{
    int u;
    int v;
    double score;
} Link;

ListNode* createNewListNode(int vertex);
List* createNewList(int vertex);
void insertAtEnd(List* list, ListNode *node);
Graph inputGraph(char *fileName);
void printAdjList(Graph G);
void deallocateGraph(Graph G);
Link* createNewHeap(Graph G, int K);
void swapLinks(Link *A, Link *B);
int lessThan(Link A, Link B);
void adjustHeap(Link *heap, int K);
void insertIntoHeap(Link *heap, int K, Link link);
void displayHeap(Link *heap, int K, char *fileName, LL **convergenceIterations);
double jaccardScore(List *listU, List *listV);
void computeJaccard(Graph G, int K, char *fileName);
LL** computeAdjacencyMatrix(Graph G);
LL** multiplyLLMatrices(LL **A, LL **B, Graph G);
double** multiplyDoubleMatrices(double **A, double **B, Graph G);
void deallocateLLMatrix(LL **matrix, int size);
void deallocateDoubleMatrix(double **matrix, int size);
void computeKatz(Graph G, int K, double constantBeta, char *fileName);
void computeCommuteTime1(Graph G, int K, char *fileName);
double absolute(double N);
void computeCommuteTime2(Graph G, int K, char *fileName);


int main()
{
    Graph G = inputGraph("../contact-high-school-proj-graph.txt");
    //printAdjList(G); // Uncomment to print the adjacency list of the Graph G

    int K;
    printf("\nEnter the value of K: ");
    scanf("%d", &K);

    computeJaccard(G, K, "Jaccard.txt");
    computeKatz(G, K, 0.1, "Katz.txt");
    computeCommuteTime1(G, K, "HittingTime.txt");
    computeCommuteTime2(G, K, "HittingTimeAccurate.txt");
    deallocateGraph(G);

    return 0;
}


// Function to create, initialise and return a new ListNode
ListNode* createNewListNode(int vertex)
{
    ListNode *newNode = (ListNode *) calloc(1, sizeof(ListNode));
    newNode -> vertex = vertex;
    return newNode;
}

// Function to create, initialise and return a new Adjacency List
List* createNewList(int vertex)
{
    List *newList = (List *) calloc(1, sizeof(List));
    newList -> vertex = vertex;
    newList -> degree = 0;
    newList -> head = newList -> tail = NULL;
    newList -> nextList = NULL;
    return newList;
}

// Function to insert a node at the end of an adjacency list in O(1) time
void insertAtEnd(List* list, ListNode *node)
{
    if (list -> head == NULL)
    {
        list -> head = node;
        list -> tail = node;
    }
    else
    {
        list -> tail -> next = node;
        list -> tail = node;
    }
    list -> degree += 1;
}

// Function to read a graph from input file, and store it in Adjacency list representation (list of lists)
// Time complexity: O(V+E) [This function takes advantage of the sorted order of the input edges in the input file]
Graph inputGraph(char *fileName)
{
    FILE *filePointer = fopen(fileName, "r");

    // check if file is opened correctly
    if (filePointer == NULL)
    {
        printf("File could not be opened.\nTry again.\n");
        exit(0);
    }

    // Count the number of vertices and edges in the input graph while ignoring edge weights
    int u, v; // 'u' and 'v' are used for storing vertices
    int numberOfVertices = 0;
    int numberOfEdges = 0;

    while (fscanf(filePointer, "%d %d %*d", &u, &v) == 2)
    {
        numberOfVertices = (u > numberOfVertices) ? u : numberOfVertices;
        numberOfVertices = (v > numberOfVertices) ? v : numberOfVertices;
        numberOfEdges++;
    }

    printf("Vertices: %d, Edges: %d\n", numberOfVertices, numberOfEdges);

    // Create a new Graph and count the degree of each vertex in given graph, and allocate required memory for adjacency list

    Graph G; 
    // Initialising graph
    G.numberOfVertices = numberOfVertices;
    G.numberOfEdges = numberOfEdges;
    G.adjList = NULL;
    
    // Now compute the degree of each vertex and the adjacency list from the input file
    if (numberOfVertices > 0)
    {
        G.adjList = createNewList(1); // since G has atleast 1 vertex
    }
    else
    {
        fclose(filePointer);
        return G;
    }

    List *listU, *listV;
    listU = listV = G.adjList; // Initialised to the first vertex
    
    rewind(filePointer);
    while (fscanf(filePointer, "%d %d %*d", &u, &v) == 2)
    {
        while (v > listV -> vertex)
        {
            List *newList = createNewList(listV -> vertex + 1);
            listV -> nextList = newList;
            listV = newList;
            listU = G.adjList; // Pointer to adjacency list of vertex 1 (reset listU)
        }

        while (u > listU -> vertex)
        {
            listU = listU -> nextList;
        }

        // insert u in adjacency list of v (listV) and insert v in adjacency list of u (listU)
        insertAtEnd(listU, createNewListNode(v));
        insertAtEnd(listV, createNewListNode(u));
    }

    fclose(filePointer);
    return G;
}

// Function to print the adjacency list in O(V + E) time
void printAdjList(Graph G)
{
    List *adjList = G.adjList;
    while (adjList != NULL)
    {
        printf("Adj[%d]: ", adjList->vertex);
        ListNode *neighbor = adjList -> head;
        while (neighbor != NULL)
        {
            printf("%d, ", neighbor -> vertex);
            neighbor = neighbor -> next;
        }
        printf("\n");
        adjList = adjList -> nextList;
    }
    printf("\n");
}

// Function to deallocate the memory allocated to store the graph
void deallocateGraph(Graph G)
{
    List *adjList = G.adjList;
    List *prevAdjList = NULL;
    while (adjList != NULL)
    {
        ListNode *node = adjList -> head;
        ListNode *prevNode = NULL;
        while (node != NULL)
        {
            prevNode = node;
            node = node -> next;
            free(prevNode);
        }
        prevAdjList = adjList;
        adjList = adjList -> nextList;
        free(prevAdjList);
    }
}

// Function to create a minHeap, which stores the top-K non-existent edges(links) ranked according to scores, 
// then this heap is used to sort the edges(links) using HeapSort in O(KlogK) time
Link* createNewHeap(Graph G, int K)
{
    Link *heap = (Link *) calloc(K, sizeof(Link)); // Create a min-heap heap which stores top K edges
    for (int i = 0; i < K; i++)
    {
        // Initialise every score in heap to LLONG_MIN for stability
        heap[i].score = LLONG_MIN;
    }
    return heap;
}

// Function to swap two links
void swapLinks(Link *A, Link *B)
{
    Link temp = *A;
    *A = *B;
    *B = temp;
}

// Function to compare and rank two links (checks if Link A comes after Link B in terms of the required sorted order)
// (descending order of score, then increasing order of u, then increasing order of v)
int lessThan(Link A, Link B)
{
    if (A.score < B.score) 
    {
        return 1;
    }
    else if (A.score == B.score && A.u > B.u)
    {
        return 1;
    }
    else if (A.score == B.score && A.u == B.u && A.v > B.v)
    {
        return 1;
    }
    return 0;
}

// Function to adjust the heap used for storing links, and restore the heap property after a modification in the heap
void adjustHeap(Link *heap, int K)
{
    int index = 0;
    int leftIndex, rightIndex;

    while (index < K)
    {
        leftIndex = 2 * index + 1;
        rightIndex = 2 * index + 2;

        if (rightIndex < K)
        {
            // current Index has two children: leftIndex and rightIndex both
            if (lessThan(heap[index], heap[leftIndex]) && lessThan(heap[index], heap[rightIndex]))
            {
                // minHeap property satisfied
                return;
            }
            else if (lessThan(heap[leftIndex], heap[rightIndex]))
            {
                // left node will come up
                swapLinks(&heap[leftIndex], &heap[index]);
                index = leftIndex;
            }
            else
            {
                // right node will come up
                swapLinks(&heap[rightIndex], &heap[index]);
                index = rightIndex;
            }
        }
        else if (leftIndex < K)
        {
            // current Index has only one child (leftIndex only)
            if (lessThan(heap[index], heap[leftIndex]))
            {
                // heap property satisfied
                return;
            }
            else
            {
                swapLinks(&heap[leftIndex], &heap[index]);
                // no need to recurse further as leftIndex is last node
            }
        }
        else
        {
            // current Index is a leaf node, so do nothing
            return;
        }
    }
}

// Function to insert a new link into the heap used for storing Top K links
void insertIntoHeap(Link *heap, int K, Link link)
{
    // First check whether the current link is one of the Top K links or not, otherwise no need to insert into the heap
    if (lessThan(heap[0], link))
    {
        heap[0] = link;
        adjustHeap(heap, K);
    }
}

// Function to display the minHeap storing top K links
void displayHeap(Link *heap, int K, char *fileName, LL **convergenceIterations)
{
    // Pop out all K links one by one in ascending order and display them in reverse order
    Link *TopKLinks = (Link *) calloc(K, sizeof(Link));
    for (int i = 0; i < K; i++)
    {
        TopKLinks[i] = heap[0];
        heap[0].score = LLONG_MAX;
        adjustHeap(heap, K);
    }

    // display the K links and Restore the original heap for future use (if at all required)

    if (fileName == NULL)
    {
        // write to the console
        if (convergenceIterations == NULL)
        {
            for (int i = K - 1; i >= 0; i--)
            {
                printf("%d %d %.12f\n", TopKLinks[i].u, TopKLinks[i].v, TopKLinks[i].score);
                heap[i] = TopKLinks[i];
            }
        }
        else
        {
            // For the case of HittingTimeAccurate.txt, where we have to print the value of 'x' as well ('x' is the number of iterations)
            for (int i = K - 1; i >= 0; i--)
            {
                printf("%d %d %.12f %lld\n", TopKLinks[i].u, TopKLinks[i].v, TopKLinks[i].score, convergenceIterations[TopKLinks[i].u][TopKLinks[i].v]);
                heap[i] = TopKLinks[i];
            }
        }
    }
    else 
    {
        // open the specified output file and write to the file
        FILE *filePointer = fopen(fileName, "w");
        if (convergenceIterations == NULL)
        {
            for (int i = K - 1; i >= 0; i--)
            {
                fprintf(filePointer, "%d %d %.12f\n", TopKLinks[i].u, TopKLinks[i].v, TopKLinks[i].score);
                heap[i] = TopKLinks[i];
            }
        }
        else
        {
            // For the case of HittingTimeAccurate.txt, where we have to print the value of 'x' as well ('x' is the number of iterations)
            for (int i = K - 1; i >= 0; i--)
            {
                fprintf(filePointer, "%d %d %.12f %lld\n", TopKLinks[i].u, TopKLinks[i].v, TopKLinks[i].score, convergenceIterations[TopKLinks[i].u][TopKLinks[i].v]);
                heap[i] = TopKLinks[i];
            }
        }
        fclose(filePointer);
    }

    free(TopKLinks); 
}

// compute jaccard coefficient of any two vertices given their adjacency lists, by computing the cardinality of their intersection and union
double jaccardScore(List *listU, List *listV)
{
    ListNode *currentNodeU, *currentNodeV;
    currentNodeU = listU -> head;
    currentNodeV = listV -> head;
    int intersectionSetCount = 0;
    
    while (currentNodeU != NULL && currentNodeV != NULL)
    {
        if (currentNodeU -> vertex == currentNodeV -> vertex)
        {
            intersectionSetCount++;
            currentNodeU = currentNodeU -> next;
            currentNodeV = currentNodeV -> next;
        }
        else if (currentNodeU -> vertex > currentNodeV -> vertex)
        {
            currentNodeV = currentNodeV -> next;
        }
        else
        {
            currentNodeU = currentNodeU -> next;
        }
    }

    // compute cardinality of union of adjacency(U) and adjacency(V)
    int unionSetCount = listU -> degree + listV -> degree - intersectionSetCount;
    // compute jaccard coefficient
    double jaccardCoeff = ((double) intersectionSetCount) / unionSetCount;
    return jaccardCoeff;
}

// compute the Jaccard score for all non-existent edges (links) of the given graph using only the adjacency list, and write the Top K links output to the given file
void computeJaccard(Graph G, int K, char *fileName)
{
    Link link;
    Link *heap = createNewHeap(G, K);

    List *listU, *listV;
    listV = G.adjList;

    // Efficiently traverse all pairs of vertices and compute Jaccard score for non-adjacent edges
    while (listV != NULL)
    {
        listU = G.adjList;
        ListNode *neighborOfV = listV -> head;
        while (neighborOfV != NULL && listU != NULL && listU -> vertex < listV -> vertex)
        {
            if (listU -> vertex < neighborOfV -> vertex)
            {
                // create a link and insert this link into the heap of top K links
                link.u = listU -> vertex;
                link.v = listV -> vertex;
                link.score = jaccardScore(listU, listV);
                insertIntoHeap(heap, K, link);
            }
            else
            {
                neighborOfV = neighborOfV -> next;
            }
            listU = listU -> nextList;
        }
        
        while (listU != NULL && listU -> vertex <= G.numberOfVertices && listU -> vertex < listV -> vertex)
        {
            // create a link and insert this link into the heap of top K links
            link.u = listU -> vertex;
            link.v = listV -> vertex;
            link.score = jaccardScore(listU, listV);
            insertIntoHeap(heap, K, link);

            listU = listU -> nextList;
        }

        listV = listV -> nextList;
    }
    
    printf("\nTop %d Jaccard Scores written to output file.\n", K);
    displayHeap(heap, K, fileName, NULL);
    free(heap); // free the dynamically allocated memory which is no longer required
}

// Function to compute and return the adjacency matrix of a graph using its adjacency list
LL** computeAdjacencyMatrix(Graph G)
{
    // Dynamically allocate memory for adjacency matrix
    LL **adjMatrix = (LL **) calloc(G.numberOfVertices + 1, sizeof(LL *));
    for (LL vertex = 0; vertex <= G.numberOfVertices; vertex++)
    {
        adjMatrix[vertex] = (LL *) calloc(G.numberOfVertices + 1, sizeof(LL));
    }

    // Fill the adjacency matrix entries whose edges are present in graph
    List *listU = G.adjList;
    while (listU != NULL)
    {
        // Traverse all neighbors of adjacency list of vertex U, and add them to adjMatrix
        // And take advantage of the fact that adjMatrix is a symmetric matrix
        ListNode *neighborOfU = listU -> head;
        while (neighborOfU != NULL && neighborOfU -> vertex < listU -> vertex)
        {
            adjMatrix[listU -> vertex][neighborOfU -> vertex] = 1;
            adjMatrix[neighborOfU -> vertex][listU -> vertex] = 1;
            neighborOfU = neighborOfU -> next;
        }
        listU = listU -> nextList;
    }

    return adjMatrix;
}

// Function to multiply two matrices of the datatype Long Long and return the resulting matrix
LL** multiplyLLMatrices(LL **A, LL **B, Graph G)
{
    // Allocate space to store the resulting matrix
    LL **C = (LL **) calloc(G.numberOfVertices + 1, sizeof(LL*));
    for (int i = 0; i <= G.numberOfVertices + 1; i++)
    {
        C[i] = (LL *) calloc(G.numberOfVertices + 1, sizeof(LL));
    }

    // Multiply matrices A and B, and store the result in matrix C
    for (int row = 0; row <= G.numberOfVertices; row++)
    {
        for (int col = 0; col <= G.numberOfVertices; col++)
        {
            // Multiply row of A and column of B, and store in C[row][col]
            C[row][col] = 0;
            for (int k = 0; k <= G.numberOfVertices; k++)
            {
                C[row][col] += A[row][k] * B[k][col];
            }
        }
    }

    return C;
}

// Function to multiply two matrices of the datatype Long Long and return the resulting matrix
double** multiplyDoubleMatrices(double **A, double **B, Graph G)
{
    // Allocate space to store the resulting matrix
    double **C = (double **) calloc(G.numberOfVertices + 1, sizeof(double*));
    for (int i = 0; i <= G.numberOfVertices + 1; i++)
    {
        C[i] = (double *) calloc(G.numberOfVertices + 1, sizeof(double));
    }

    // Multiply matrices A and B, and store the result in matrix C
    for (int row = 0; row <= G.numberOfVertices; row++)
    {
        for (int col = 0; col <= G.numberOfVertices; col++)
        {
            // Multiply row of A and column of B, and store in C[row][col]
            C[row][col] = 0;
            for (int k = 0; k <= G.numberOfVertices; k++)
            {
                C[row][col] += A[row][k] * B[k][col];
            }
        }
    }

    return C;
}

// Function to deallocate a dynamically allocated matrix of datatype Long Long
void deallocateLLMatrix(LL **matrix, int size)
{
    for (int i = 0; i <= size; i++)
    {
        free(matrix[i]);
    }
    free(matrix);
}

// Function to deallocate a dynamically allocated matrix of datatype Datatype
void deallocateDoubleMatrix(double **matrix, int size)
{
    for (int i = 0; i <= size; i++)
    {
        free(matrix[i]);
    }
    free(matrix);
}

// compute the Katz score for all non-existent edges (links) of the given graph, and write the Top K links output to the given file
void computeKatz(Graph G, int K, double constantBeta, char *fileName)
{
    // First compute the adjacency matrix of G using the adjacency list, which will help us to count the number of paths of any length between vertices
    // Also, store the adjacency matrix as LL (long long) instead of int to prevent integer overflow in the future
    LL **adjMatrix = computeAdjacencyMatrix(G);
    
    // Compute "paths" matrix, which will store the number of paths between any two vertices of a given "pathLength"
    // Also initialise "paths" matrix to the adjacency matrix and initially set corresponding "pathLength" as 1
    LL **paths = computeAdjacencyMatrix(G);
    int pathLength = 1;

    // Also create a matrix called "katzScores" to score the katzScores of non-existent edges in the graph
    double **katzScores = (double **) calloc(G.numberOfVertices + 1, sizeof(double *));
    for (int vertex = 0; vertex <= G.numberOfVertices; vertex++)
    {
        katzScores[vertex] = (double *) calloc(G.numberOfVertices + 1, sizeof(double));
    }

    Link *heap = createNewHeap(G, K);

    // compute Katz score for non-existent edges, by summing up scores over path lengths 2 to 6
    double beta = constantBeta;
    for (pathLength = 2; pathLength <= 6; pathLength++)
    {
        // compute number of walks of pathLength 2 between all vertices
        LL** previousPaths = paths; // 'previousPaths' is a temporary variable used to free this memory later
        paths = multiplyLLMatrices(paths, adjMatrix, G);
        deallocateLLMatrix(previousPaths, G.numberOfVertices);
        beta *= constantBeta;

        // for every non-existent edge (u, v) compute katz score
        for (int u = 1; u <= G.numberOfVertices; u++)
        {
            for (int v = u + 1; v <= G.numberOfVertices; v++)
            {
                // check if non-existent edge
                if (adjMatrix[u][v] == 1)
                {
                    continue;
                }
                katzScores[u][v] += beta * paths[u][v];
                katzScores[v][u] += beta * paths[v][u];
                
                // insert link into heap along with it's katz score
                if (pathLength == 6)
                {
                    Link link;
                    link.u = u;
                    link.v = v;
                    link.score = katzScores[u][v];
                    insertIntoHeap(heap, K, link);
                }
            }
        }
    }

    printf("\nTop %d Katz Scores written to output file.\n", K);
    displayHeap(heap, K, fileName, NULL); // Display the top K links
    deallocateLLMatrix(paths, G.numberOfVertices);
    deallocateLLMatrix(adjMatrix, G.numberOfVertices);
    deallocateDoubleMatrix(katzScores, G.numberOfVertices);
    free(heap);
}

// compute the Commute Time score for all non-existent edges (links) of the given graph by taking summation upto PathLength 6, and write the Top K links output to the given file
void computeCommuteTime1(Graph G, int K, char *fileName)
{
    LL **adjMatrix = computeAdjacencyMatrix(G);
    // 'transitionMatrix' is the probability transition matrix between vertices of the graph, whereas
    // 'pathMatrix' stores powers of the transition matrix, i.e, for every u,v pathMatrix[u][v] stores the probability of starting from vertex u and ending up in vertex v, after traversing a walk of length 'pathLength'
    double **transitionMatrix = (double **) calloc(G.numberOfVertices + 1, sizeof(double *));
    double **pathMatrix = (double **) calloc(G.numberOfVertices + 1, sizeof(double *));
    for (int vertex = 0; vertex <= G.numberOfVertices; vertex++)
    {
        transitionMatrix[vertex] = (double *) calloc(G.numberOfVertices + 1, sizeof(double));
        pathMatrix[vertex] = (double *) calloc(G.numberOfVertices + 1, sizeof(double));
    }

    // Compute the probability transition matrix using the adjacency matrix, and initialise pathMatrix = transitionMatrix for pathLength = 1
    int pathLength = 1;
    LL *adjacencySum = (LL *) calloc(G.numberOfVertices + 1, sizeof(LL));
    for (int vertex = 0; vertex <= G.numberOfVertices; vertex++)
    {
        for (int v = 1; v <= G.numberOfVertices; v++)
        {
            adjacencySum[vertex] += adjMatrix[vertex][v];
        }
    }

    for (int u = 1; u <= G.numberOfVertices; u++)
    {
        for (int v = 1; v <= G.numberOfVertices; v++)
        {
            transitionMatrix[u][v] = ((double) adjMatrix[u][v]) / adjacencySum[u];
            pathMatrix[u][v] = transitionMatrix[u][v];
        }
    }
    free(adjacencySum);

    // create a matrix to store the commute time between every pair of vertices (for non-existent edges only)
    double **commuteTime = (double **) calloc(G.numberOfVertices + 1, sizeof(double *));
    for (int vertex = 0; vertex <= G.numberOfVertices; vertex++)
    {
        commuteTime[vertex] = (double *) calloc(G.numberOfVertices + 1, sizeof(double));
    }
    Link *heap = createNewHeap(G, K); // create new heap for storing the Top K links

    // compute commute time by taking summation from pathLength = 2 to 6 as mentioned in the question
    for (pathLength = 2; pathLength <= 6; pathLength++)
    {
        double **previousPathMatrix = pathMatrix; // 'previousPathMatrix' is a temporary variable which helps to deallocate this memory later
        pathMatrix = multiplyDoubleMatrices(pathMatrix, transitionMatrix, G);
        deallocateDoubleMatrix(previousPathMatrix, G.numberOfVertices);

        // for every u,v of non-existent edge, compute the commute time
        for (int u = 1; u <= G.numberOfVertices; u++)
        {
            for (int v = u + 1; v <= G.numberOfVertices; v++)
            {
                // check if non-existent edge
                if (adjMatrix[u][v] == 1)
                {
                    continue;
                }

                commuteTime[u][v] -= pathLength * (pathMatrix[u][v] + pathMatrix[v][u]);
                commuteTime[v][u] = commuteTime[u][v];

                // insert link into heap along with it's commute time score
                if (pathLength == 6)
                {
                    Link link;
                    link.u = u;
                    link.v = v;
                    link.score = commuteTime[u][v];
                    insertIntoHeap(heap, K, link);
                }
            }
        }
    }

    printf("\nTop %d Commute Time Scores written to output file.\n", K);
    displayHeap(heap, K, fileName, NULL); // Display the top K links
    deallocateLLMatrix(adjMatrix, G.numberOfVertices);
    deallocateDoubleMatrix(commuteTime, G.numberOfVertices);
    deallocateDoubleMatrix(pathMatrix, G.numberOfVertices);
    deallocateDoubleMatrix(transitionMatrix, G.numberOfVertices);
    free(heap);
}

// returns the absolute value of a decimal number (double)
double absolute(double N)
{
    if (N > 0)
    {
        return N;
    }
    return -N;
}

// compute the Commute Time score for all non-existent edges (links) of the given graph by taking summation till atleast K edges converge and reach the stopping condition given in question, and write the Top K links output to the given file (I did not wait for all edges(links) to converge since some links scores were actually diverging instead of converging)
void computeCommuteTime2(Graph G, int K, char *fileName)
{
    LL **adjMatrix = computeAdjacencyMatrix(G);
    // Definition and use-case of transitionMatrix and pathMatrix same as in 'computeCommuteTime1' function, so not writing the same comments here
    double **transitionMatrix = (double **) calloc(G.numberOfVertices + 1, sizeof(double *));
    double **pathMatrix = (double **) calloc(G.numberOfVertices + 1, sizeof(double *));
    for (int vertex = 0; vertex <= G.numberOfVertices; vertex++)
    {
        transitionMatrix[vertex] = (double *) calloc(G.numberOfVertices + 1, sizeof(double));
        pathMatrix[vertex] = (double *) calloc(G.numberOfVertices + 1, sizeof(double));
    }

    // Initialise the probability transition matrix for path length = 1
    int pathLength = 1;
    LL *adjacencySum = (LL *) calloc(G.numberOfVertices + 1, sizeof(LL));
    for (int vertex = 0; vertex <= G.numberOfVertices; vertex++)
    {
        for (int v = 1; v <= G.numberOfVertices; v++)
        {
            adjacencySum[vertex] += adjMatrix[vertex][v];
        }
    }

    for (int u = 1; u <= G.numberOfVertices; u++)
    {
        for (int v = 1; v <= G.numberOfVertices; v++)
        {
            transitionMatrix[u][v] = ((double) adjMatrix[u][v]) / adjacencySum[u];
            pathMatrix[u][v] = transitionMatrix[u][v];
        }
    }
    free(adjacencySum);

    double **commuteTime = (double **) calloc(G.numberOfVertices + 1, sizeof(double *));
    for (int vertex = 0; vertex <= G.numberOfVertices; vertex++)
    {
        commuteTime[vertex] = (double *) calloc(G.numberOfVertices + 1, sizeof(double));
    }
    Link *heap = createNewHeap(G, K);

    int convergedEdges = 0; // Keeps track of the number of converged edges
    // At the end of summing up commute times for each pathLength, we check whether atleast K edges have converged or not. If yes, we stop the loop.

    // used for storing 'x' (number of iterations needed for convergence) for HittingTimeAccurate
    LL **convergence = (LL **) calloc(G.numberOfVertices + 1, sizeof(LL *));
    for (int vertex = 0; vertex <= G.numberOfVertices; vertex++)
    {
        convergence[vertex] = (LL *) calloc(G.numberOfVertices + 1, sizeof(LL));
    }

    // I'm not considering summation of pathLengths more than 50, and I am stopping at this point since I don't want the program to fall into an infinite loop for wierd test cases
    while (convergedEdges < K && pathLength <= 20) 
    {
        pathLength++;
        double **previousPathMatrix = pathMatrix;
        pathMatrix = multiplyDoubleMatrices(pathMatrix, transitionMatrix, G);
        deallocateDoubleMatrix(previousPathMatrix, G.numberOfVertices);

        for (int u = 1; u <= G.numberOfVertices; u++)
        {
            for (int v = u + 1; v <= G.numberOfVertices; v++)
            {
                // check if non-existent edge
                if (adjMatrix[u][v] == 1)
                {
                    continue;
                }

                double commuteTimeUV = commuteTime[u][v] - pathLength * (pathMatrix[u][v] + pathMatrix[v][u]);
                commuteTime[u][v] = commuteTimeUV;
                commuteTime[v][u] = commuteTimeUV;

                if (absolute(commuteTimeUV - commuteTime[u][v]) < 0.01 && absolute(commuteTimeUV) > 0.00001)
                {
                    Link link;
                    link.u = u;
                    link.v = v;
                    link.score = commuteTime[u][v];
                    convergence[u][v] = pathLength;
                    convergence[v][u] = pathLength;
                    insertIntoHeap(heap, K, link);
                    convergedEdges++;
                }
            }
        }
    }

    printf("\nNumber of links whose Commute Time Accurate scores actually converged according to the given stopping condition: %d\n", convergedEdges);

    // cover the corner case of infinite loop
    if (convergedEdges < K)
    {
        for (int u = 1; u <= G.numberOfVertices; u++)
        {
            for (int v = u + 1; v <= G.numberOfVertices; v++)
            {
                if (adjMatrix[u][v] == 1)
                {
                    continue;
                }
                Link link;
                link.u = u;
                link.v = v;
                link.score = commuteTime[u][v];
                convergence[u][v] = -1; // -1 indicates this link hasn't actually converged yet
                convergence[v][u] = -1;
                insertIntoHeap(heap, K, link);
            }
        }
    }

    printf("Top %d Commute Time Accurate Scores written to output file.\n\n", K);
    displayHeap(heap, K, fileName, convergence); // Display the top K links
    deallocateLLMatrix(adjMatrix, G.numberOfVertices);
    deallocateLLMatrix(convergence, G.numberOfVertices);
    deallocateDoubleMatrix(commuteTime, G.numberOfVertices);
    deallocateDoubleMatrix(pathMatrix, G.numberOfVertices);
    deallocateDoubleMatrix(transitionMatrix, G.numberOfVertices);
    free(heap);
}