#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>

#define MAX_LEN 10000

typedef struct Node 
{
    int i, j;
    float p;
    struct Node *next, *prev;
} Node;

// Function to construct a new blank Node and return it's address
Node* create_node()
{
    Node *node = (Node *) malloc (sizeof(Node));
    node -> i = 0;
    node -> j = 0;
    node -> p = 0;
    node -> next = NULL;
    node -> prev = NULL;
    return node;
}

// Function to fill/initialise a Node with given values
void fill_node(Node *node, int i, int j, float p)
{
    node -> i = i;
    node -> j = j;
    node -> p = p;
}

// Function to traverse the polynomial LL
void traverse_LL(Node *node)
{
    while (node != NULL)
    {
        printf("-->(%d,%d,%.2f)", node->i, node->j, node->p);
        node = node -> next;
    }
    printf("\n");
}

Node *deep_copy_LL(Node *node)
{
    Node *copy = NULL;
    // Create a deep copy of the original list
    while (node != NULL)
    {
        Node *new_node = create_node();
        fill_node(new_node, node->i, node->j, node->p);
        if (copy == NULL)
        {
            copy = new_node;
        }
        else
        {
            new_node -> prev = copy;
            copy -> next = new_node;
            copy = copy -> next;
        }
        node = node -> next;
    }
    // Traverse back to the head node and return the head of the copy list
    while (copy -> prev != NULL)
    {
        copy = copy -> prev;
    }
    return copy;
}

// Delete list and free the memory allocated for it
void free_LL(Node *node)
{
    Node *prev = NULL;
    while (node != NULL)
    {
        prev = node;
        node = node -> next;
        free(prev);
    }
}

// Function to insert a new node into the LL in sorted order and return the head of the modified LL
Node* insert_node(Node *head, Node *new_node)
{
	// case 1: empty LL
	if (head == NULL)
	{
		head = new_node;
	}
	// case 2: insert before head
	else if ((new_node->i + new_node->j > head->i + head->j) || ((new_node->i + new_node->j == head->i + head->j) && (new_node->i > head->i)))
	{
		new_node->next = head;
		head->prev = new_node;
		head = new_node;
	}
	// case 3: insert somewhere in the middle (no need to change head node)
	else 
	{
        // Find the position where new_node should be inserted
		Node *node = head;
		while ((node->next != NULL) && ((new_node->i + new_node->j < node->i + node->j) || ((new_node->i + new_node->j == node->i + node->j) && (new_node->i < node->i))))
		{
			node = node->next;
		}

		// case 3.1: check if new_node has to be merged with node (corner case)
		if (new_node->i == node->i && new_node->j == node->j)
		{
			node->p = node->p + new_node->p;
			return head;
		}
		// case 3.2: check if new_node has to be added at the end of the linked list
		else if ((node->next == NULL) && ((new_node->i + new_node->j < node->i + node->j) || ((new_node->i + new_node->j == node->i + node->j) && (new_node->i < node->i))))
		{
			new_node->prev = node;
			node->next = new_node;
		}
		// case 3.3: insert new_node somewhere in the middle of the linked list (before 'node')
		else
		{
			Node *prev_node = node->prev;
			Node *next_node = node;
			new_node->prev = prev_node;
			new_node->next = next_node;
			prev_node->next = new_node;
			next_node->prev = new_node;
		}
	}
	return head;
}

// Function to take polynomial as input and store it using linked lists
Node* input()
{
    // take polynomial input from a single line as a string
    // and parse the input string to get individual terms of the polynomial
    char *str = (char *) calloc(MAX_LEN, sizeof(char)); 
    Node *head = NULL, *node = NULL;
    memset(str, 0, MAX_LEN);

    scanf("%s", str);

    // Parse input string str and extract the comma-seperated numbers
    int i = 0, j;
    while (str[i] != 0)
    {
        Node *new_node = create_node();

        // Extract polynomial terms (3 comma-seperated values)
        for (int k = 0; k < 3; k++)
        {
            i++;
            int j = 0;
            char num[50] = ""; // temporary string to store numeric data
            while (isdigit(str[i]) || str[i] == '.' || str[i] == '-')
            {
                num[j] = str[i];
                i++;
                j++;
            }
            num[j] = 0; // Append 0 at the end of the string

            if (k == 0)
                new_node -> i = atoi(num);
            else if (k == 1)
                new_node -> j = atoi(num);
            else
                new_node -> p = atof(num);

        }

        // Insert new_node into the linked list in sorted order
        head = insert_node(head, new_node);

        i += 2; // Skip the ')' and ',' characters in input 'str'
    }
    // now return the head of the created polynomial LL
    // And free the heap memory which is no longer required
    free(str);
    return head;
}

// Function to add two polynomials stored as linked lists, and return the resulting polynomial
Node *add_poly(Node *p1, Node *p2)
{
    Node *result = NULL; // Stores the resulting polynomial after adding p1 and p2

    while (p1 != NULL && p2 != NULL)
    {
        Node *new_node = create_node();

        if (p1->i == p2->i && p1->j == p2->j)
        {
            fill_node(new_node, p1->i, p1->j, p1->p + p2->p);
            p1 = p1 -> next;
            p2 = p2 -> next;
        }
        else if ((p1->i + p1->j > p2->i + p2->j) || ((p1->i + p1->j == p2->i + p2->j) && (p1->i > p2->i)))
        {
            fill_node(new_node, p1->i, p1->j, p1->p);
            p1 = p1 -> next;
        }
        else 
        {
            fill_node(new_node, p2->i, p2->j, p2->p);
            p2 = p2 -> next;
        }
        
        // Now append the newly created node at the end of the result list
        if (result == NULL)
        {
            result = new_node;
        }
        else
        {
            new_node -> prev = result;
            result -> next = new_node;
            result = result -> next;
        }
    }

    while (p1 != NULL)
    {
        Node *new_node = create_node();
        fill_node(new_node, p1->i, p1->j, p1->p);
        // Now append the newly created node at the end of the result list
        if (result == NULL)
        {
            result = new_node;
        }
        else
        {
            new_node -> prev = result;
            result -> next = new_node;
            result = result -> next;
        }
        p1 = p1 -> next;
    }

    while (p2 != NULL)
    {
        Node *new_node = create_node();
        fill_node(new_node, p2->i, p2->j, p2->p);
        // Now append the newly created node at the end of the result list
        if (result == NULL)
        {
            result = new_node;
        }
        else
        {
            new_node -> prev = result;
            result -> next = new_node;
            result = result -> next;
        }
        p2 = p2 -> next;
    }

    // Now traverse 'result' back to the head using prev pointers and then return the head
    while (result -> prev != NULL)
    {
        result = result -> prev;
    }

    return result;
}

// Function to multiply two polynomials stored as linked lists, and return the resulting polynomial
Node *multiply_poly(Node *p1, Node *p2)
{
    // The logic is to multiply each element of p2 with all elements of p1 to get partial products
    // And then add up all the intermediate polynomials using the add function

    Node *result = NULL;
    // result holds the final resulting polynomial whereas 
    // 'partial' holds the intermediate partial products which have to be added up

    while (p2 != NULL)
    {
        // Create a deep copy of polynomial p1 called 'partial' (to preserve the original polynomial p1)
        // And multiply each term of partial (p1) with current term of p2 to compute the partial product
        Node *partial = deep_copy_LL(p1);
        Node *node = partial;
        while (node != NULL)
        {
            // Perform multiplication on current pair of terms of polynomials partial(p1) and p2
            node->i = node->i + p2->i;
            node->j = node->j + p2->j;
            node->p = node->p * p2->p;

            node = node -> next;
        }

        // Now 'partial' contains the partial product corresponding to one term of p2
        // Add it to 'result'
        result = add_poly(result, partial);
        p2 = p2 -> next;
        free_LL(partial); // Free the partial LL which is no longer required 
    }

    return result;
}

int main()
{
    Node *poly1, *poly2, *result;
    char flag = 'y', op;
    
    while (flag != 'n')
    {
        printf("Please enter the operator(+/*): ");
        scanf(" %c", &op);

        printf("Enter polynomial A in a single line (without spaces): ");
        poly1 = input();
        printf("Sorted polynomial A: ");
        traverse_LL(poly1);
        printf("Enter polynomial B in a single line (without spaces): ");
        poly2 = input();
        printf("Sorted polynomial B: ");
        traverse_LL(poly2);
        
        if (op == '+')
        {
            result = add_poly(poly1, poly2);
        }
        else 
        {
            result = multiply_poly(poly1, poly2);
        }

        printf("\nA %c B: ", op);
        traverse_LL(result);
        printf("\nDo you want to continue? (y/n): ");
        scanf(" %c", &flag);
        printf("\n");

        // free the memory which is no longer required
        free_LL(poly1);
        free_LL(poly2);
        free_LL(result);
    }
    return 0;
}