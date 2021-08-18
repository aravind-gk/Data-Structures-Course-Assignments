#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>

#define MAX_LEN 10000

// Used to store sequence using 'chunk' lists
typedef struct Chunk
{
    int arr[8];
    struct Chunk *next;
} Chunk;

// Used to store pattern and text using normal lists 
typedef struct Node
{
    int val;
    struct Node *next;
} Node;

// Create a new node of the type Chunk
Chunk* create_chunk_node()
{
    Chunk *node = (Chunk*) malloc(sizeof(Chunk));
    node -> next = NULL;
    // Initialise all array elements to INT_MIN (to represent minus-infinity)
    for (int i = 0; i < 8; i++)
    {
        node->arr[i] = INT_MIN;
    } 
    return node;
}

// Create a new node for normal linked lists
Node* create_node()
{
    Node *node = (Node*) malloc(sizeof(Node));
    node -> next = NULL;
    node -> val = INT_MIN;
    return node;
}

// function to traverse and print the 'chunk' linked lists
void traverse_chunk_LL(Chunk *node)
{
    while (node != NULL)
    {
        printf("-->(%d", node->arr[0]); // Valid because each chunk contains atleast one integer, otherwise it wouldn't exist
        for (int i = 1; i < 8; i++)
        {
            if (node->arr[i] == INT_MIN)
            {
                break;
            }
            printf(",%d", node->arr[i]);
        }
        printf(")");
        node = node -> next;
    }
    printf("\n");
}

// function to traverse and print normal linked lists
void traverse_LL(Node *node)
{
    while (node != NULL)
    {
        printf("-->(%d)", node->val);
        node = node -> next;
    }
    printf("\n");
}

// function to free (deallocate memory) chunk linked lists
void free_chunk_LL(Chunk *node)
{
    Chunk *prev;
    while (node != NULL)
    {
        prev = node;
        node = node -> next;
        free(prev);
    }
}

// function to free (deallocate memory) normal linked lists
void free_LL(Node *node)
{
    Node *prev;
    while (node != NULL)
    {
        prev = node;
        node = node -> next;
        free(prev);
    }
}

// Function which takes 'sequence' as input from the user
// and stores it in a chunk list and returns the base address of that list
Chunk* input_sequence()
{
    char *str = (char *) calloc(MAX_LEN, sizeof(char));
    scanf("%s", str);

    Chunk *head, *node;
    head = node = NULL;

    // Extract integers one-by-one from the string 'str' and insert them into chunks
    int i = 0, index = 0; // 'index' represents the chunk index where integer has to be inserted
    while (str[i] != '$')
    {
        int j = 0;
        char num[50] = "";
        while (str[i] != ',')
        {
            num[j] = str[i];
            j++;
            i++;
        }
        num[j] = 0; // Append NULL at the end of the string
        i++; // skip the ',' character

        if (index == 0)
        {
            // previous chunk is full, so create a new chunk
            Chunk *new_node = create_chunk_node();
            new_node->arr[index] = atoi(num);

            // Now append this node at the end of the current LL
            if (head == NULL)
            {
                head = node = new_node;
            }
            else 
            {
                node -> next = new_node;
                node = node -> next;
            }
        }
        else
        {
            // Insert the integer into the current chunk
            node->arr[index] = atoi(num);
        }
        index++;
        if (index == 8)
        {
            index = 0; // Insert the next integer in the beginning of a new chunk
        }
    }
    free(str); // free memory which is no longer required
    // return the head node of the input sequence
    return head;
}

// Function which takes 'pattern' and 'text' as inputs from the user
// and stores it in a normal linked list and returns their base addresses 
Node* input_pattern()
{
    char *str = (char *) calloc(MAX_LEN, sizeof(char));
    scanf("%s", str);

    Node *head, *node;
    head = node = NULL;

    // Extract integers one-by-one and insert them into nodes
    int i = 0; // 'index' represents the chunk index where integer has to be inserted
    while (str[i] != '$')
    {
        int j = 0;
        char num[50] = "";
        while (str[i] != ',')
        {
            num[j] = str[i];
            j++;
            i++;
        }
        num[j] = 0; // Append NULL at the end of the string
        i++; // skip the ',' character

        // Create a new node with value 'num' and append it to the list
        Node *new_node = create_node();
        new_node->val = atoi(num);

        if (head == NULL)
        {
            head = node = new_node;
        }
        else
        {
            node -> next = new_node;
            node = node -> next;
        }
    }

    free(str); // free memory which is no longer required
    // return the head node of the input sequence
    return head;
}

// Function to detect pattern in sequence in the given position (node,index)
int match(Chunk *node, int index, Node *pattern)
{
    int found = 1;
    while (pattern != NULL)
    {
        if (node == NULL || node->arr[index] == INT_MIN || pattern->val != node->arr[index])
        {
            found = 0;
            break;
        }
        pattern = pattern->next;
        index += 1;
        if (index == 8)
        {
            node = node -> next;
            index = 0;
        }
    }
    return found;
}

// Function to detect all patterns in a given sequence and print the positions where patterns are found
// (I didn't use this function in the final program)
void detect_all_patterns(Chunk *head, Node *pattern)
{
    Chunk *node = head;
    int index = 0, node_count = 1;

    while (node != NULL && node->arr[index] != INT_MIN)
    {
        int found = match(node, index, pattern);
        if (found == 1)
        {
            printf("Pattern found at Chunk %d, index %d.\n", node_count, index);
            Node *temp = pattern;
            while (temp != NULL)
            {
                temp = temp -> next;
                index += 1;
                if (index == 8)
                {
                    node = node -> next;
                    node_count += 1;
                    index = 0;
                }
            }
        }
        else
        {
            index += 1;
            if (index == 8)
            {
                node = node -> next;
                node_count += 1;
                index = 0;
            }
        }
    }
}

// Function to insert replacement text at the beginning of the sequence in case 'pattern' is 'empty'
Chunk* corner_case(Chunk *head1, Node *text)
{
    Chunk *head2 = NULL; // head2 represents the head node of the replacement sequence
    Chunk *node1 = head1; // For traversing original sequence
    Chunk *node2 = head2; // For traversing replacement sequence

    int index1 = 0;
    int index2 = 0;

    // Insert 'text' in the replacement list
    Node *node_t = text;
    while (node_t != NULL)
    {
        if (index2 == 0)
        {
            // Create a new empty chunk
            Chunk *new_node = create_chunk_node();
            new_node -> arr[index2] = node_t -> val;
            if (head2 == NULL)
            {
                head2 = node2 = new_node;
            }
            else
            {
                node2 -> next = new_node;
                node2 = node2 -> next;
            }
        }
        else
        {
            node2 -> arr[index2] = node_t -> val;
        }
        index2 += 1;
        if (index2 == 8)
        {
            index2 = 0;
        }
        node_t = node_t -> next;
    }

    // Now insert the original sequence after the replacement text in the modified list

    while (node1 != NULL && node1 -> arr[index1] != INT_MIN)
    {
        if (index2 == 0)
        {
            // Create a new empty chunk
            Chunk *new_node = create_chunk_node();
            new_node -> arr[index2] = node1 -> arr[index1];
            if (head2 == NULL)
            {
                head2 = node2 = new_node;
            }
            else
            {
                node2 -> next = new_node;
                node2 = node2 -> next;
            }
        }
        else
        {
            node2 -> arr[index2] = node1 -> arr[index1];
        }
        // Increment the second index (which points to the second list)
        index2 += 1;
        if (index2 == 8)
        {
            index2 = 0;
        }
        
        // Increment the first index 
        index1 += 1;
        if (index1 == 8)
        {
            index1 = 0;
            node1 = node1 -> next;
        }
    }

    // Now return the chunk list after performing replacements in the beginning (for empty pattern)
    return head2;
}

// Function to detect all patterns in a given sequence and replace it with given 'text'
// Here 'head1' represents the head node of the original sequence
Chunk* replace_all_patterns(Chunk *head1, Node *pattern, Node* text)
{
    // Deal with the corner-case when the pattern itself is empty
    // Make sure that empty-pattern matches only at the beginning of the sequence
    if (pattern == NULL)
    {
        return corner_case(head1, text);
    }

    Chunk *head2 = NULL; // 'head2' represents the head node of the replacement sequence
    Chunk *node1 = head1; // For traversing original sequence
    Chunk *node2 = head2; // For traversing replacement sequence

    int index1 = 0; // Represents the current index in original sequence where we do pattern matching
    int index2 = 0; // Represents the current index in replacement sequence where we insert integers after performing replacement
    int node_count = 0; // Counts the number of sequence nodes traversed so far

    while (node1 != NULL && node1->arr[index1] != INT_MIN)
    {
        int found = match(node1, index1, pattern);
        if (found == 1)
        {
            // Pattern detected in sequence, so insert 'text' in the replacement sequence
            // First traverse the original sequence till the end of the pattern
            
            //printf("Pattern found at Chunk %d, index %d.\n", node_count, index1);
            Node *node_p = pattern; // 'node_p' is a node to traverse the 'pattern'
            while (node_p != NULL)
            {
                node_p = node_p -> next;
                index1 += 1;
                if (index1 == 8)
                {
                    node1 = node1 -> next;
                    node_count += 1;
                    index1 = 0;
                }
            }
            
            // Now insert 'text' into the replacment sequence
            Node *node_t = text; // 'node_t' is a node to traverse 'text'
            while (node_t != NULL)
            {
                // Now insert node_t->val into replacement sequence's 'node2' at position 'index2'
                if (index2 == 0)
                {
                    Chunk *new_node = create_chunk_node();
                    new_node->arr[index2] = node_t -> val;
                    if (head2 == NULL)
                    {
                        head2 = node2 = new_node;
                    }
                    else
                    {
                        node2 -> next = new_node;
                        node2 = node2 -> next;
                    }
                }
                else
                {
                    node2 -> arr[index2] = node_t -> val;
                }
                index2 += 1;
                if (index2 == 8)
                {
                    index2 = 0;
                }
                node_t = node_t -> next;
            }
        }
        else
        {
            // Pattern not found at this position
            // So simply copy the value at current position in original sequence into replacement sequence
            if (index2 == 0)
            {
                Chunk *new_node = create_chunk_node();
                new_node->arr[index2] = node1->arr[index1];
                if (head2 == NULL)
                {
                    head2 = node2 = new_node;
                }
                else
                {
                    node2 -> next = new_node;
                    node2 = node2 -> next;
                }

            }
            else
            {
                node2->arr[index2] = node1->arr[index1];
            }
            index2 += 1;
            if (index2 == 8)
            {
                index2 = 0;
            }
            // Increment index1 as well
            index1 += 1;
            if (index1 == 8)
            {
                node1 = node1 -> next;
                index1 = 0;
                node_count += 1;
            }
        }
    }

    // Return the replacement sequence
    return head2;
}

int main()
{
    // NOTE: The case for empty pattern is still not handled 

    printf("\nEnter sequence S: ");
    Chunk *sequence = input_sequence();

    printf("Original sequence S: ");
    traverse_chunk_LL(sequence);

    // printf("\nEnter pattern: ");
    // Node *pattern = input_pattern();
    // printf("Enter replacement text: ");
    // Node *text = input_pattern();

    // detect_all_patterns(sequence, pattern);
    // free_LL(pattern);
    // free_LL(text);

    char flag = 'y';
    while (flag == 'y')
    {
        printf("\nEnter pattern: ");
        Node *pattern = input_pattern();
        printf("Enter replacement text: ");
        Node *text = input_pattern();

        Chunk *old_sequence = sequence;
        // detect_all_patterns(sequence, pattern);
        sequence = replace_all_patterns(sequence, pattern, text);
        printf("After replacement, S: ");
        traverse_chunk_LL(sequence);

        // Free memory which is no longer required
        free_chunk_LL(old_sequence); 
        free_LL(pattern);
        free_LL(text);

        printf("Do you want to continue? (y/n): ");
        scanf(" %c", &flag);
    }

    printf("\n");
    free_chunk_LL(sequence);
    return 0;
}