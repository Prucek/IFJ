/**
 * @file symtable-test.c
 * @authors Marek Micek, ...
 * @date 26 OCT 2020
 * @brief Test unit for table of symbols
 */

#include "symtable.h"


TNode *node, *actual_node;
TData data_1, data_2, data_3, data_4, data_5;
bool correct = 1; 

void print_inorder(TNode *node)
{
    if (node)
    {
        print_inorder(node->lptr);
        printf("%d ", node->key);
        print_inorder(node->rptr);
    }
}
 
int main() {

char *identifier_1 = "number";
char *identifier_2 = "get_number";
char *identifier_3 = "pi";
char *identifier_4 = "name";
char *identifier_5 = "undefined";

data_1.id = (char *) malloc(sizeof(strlen(identifier_1) + 1));
data_2.id = (char *) malloc(sizeof(strlen(identifier_2) + 1));
data_3.id = (char *) malloc(sizeof(strlen(identifier_3) + 1));
data_4.id = (char *) malloc(sizeof(strlen(identifier_4) + 1));
data_5.id = (char *) malloc(sizeof(strlen(identifier_5) + 1));

if (data_1.id == NULL || data_2.id == NULL || data_3.id == NULL || data_4.id == NULL)
{
    fprintf(stderr, "Allocation error!\n");
    return 1;
}

/****** first data unit ******/
strcpy(data_1.id, identifier_1);
data_1.type = T_INT;
data_1.defined = false;
data_1.global = false;
data_1.is_function = false;
data_1.param_counter = 0;

/***** second data unit ******/
strcpy(data_2.id, identifier_2);
data_2.type = T_INT;
data_2.defined = false;
data_2.global = false;
data_2.is_function = true;
data_2.param_counter = 1;

/***** third data unit ******/
strcpy(data_3.id, identifier_3);
data_3.type = T_FLOAT64;
data_3.defined = true;
data_3.global = false;
data_3.is_function = false;
data_3.param_counter = 0;

/***** fourth data unit ******/
strcpy(data_4.id, identifier_4);
data_4.type = T_STRING;
data_4.defined = true;
data_4.global = false;
data_4.is_function = false;
data_4.param_counter = 0;

/***** fifth data unit ******/
strcpy(data_5.id, identifier_5);
data_5.type = T_UNDEFINED;
data_5.defined = false;
data_5.global = false;
data_5.is_function = false;
data_5.param_counter = 0;


node = init_symtable(node);
node = insert_symtable(node, data_1, 9);    //< inserting first node == root
actual_node = insert_symtable(node, data_2, 4);
actual_node = insert_symtable(node, data_3, 15);
actual_node = insert_symtable(node, data_4, 6);
actual_node = insert_symtable(node, data_5, 3);


printf("Expected inorder output = 3 4 6 9 15\n");
printf("Our inorder output : ");
print_inorder(node);
printf("\n***********************************************\n");

printf("Search table test 1: searching existing key\n");

actual_node = search_symtable(node, 15);
if (actual_node->key != 15)    //< we should find node
{
    fprintf(stderr, "Keys not equal in node and in passing data!\n");
    correct = false;
}
if (strcmp(actual_node->data.id, identifier_3))
{
    fprintf(stderr, "Identifiers not same in node and in passing data!\n");
    correct = false;
}
if (actual_node->data.type != T_FLOAT64)
{
    fprintf(stderr, "Data type not same in node and in passing data!\n");
    correct = false;
}
if (actual_node->data.defined != true || actual_node->data.global != false || actual_node->data.is_function != false)
{
    fprintf(stderr, "One of boolean variables not same in node and in passing data!\n");
    correct = false;
}
if (actual_node->data.param_counter != 0)
{
    fprintf(stderr, "Param counter not same in node and in passing data!\n");
    correct = false;
}
if (correct)
{
    printf("Searching test 1 passed!!!\n");
}

printf("***********************************************\n");
printf("Search table test 2: searching not existing key\n");

actual_node = search_symtable(node, 99);        //< we should not find any node
if (actual_node != NULL)
{
    fprintf(stderr, "Search table error occured while searching not existing key!\n");
}
else
{
    printf("Searching test 2 passed!!!\n");
}

printf("***********************************************\n");
correct = true; //< in case it was set to false in first test unit

/* we are going to insert new data with same key*/
data_1.type = T_FLOAT64;
data_1.defined = true;
data_1.global = true;
data_1.is_function = true;
data_1.param_counter = 1;

insert_symtable(node, data_1, 3);           //< node 3 should be rewrite with new data
actual_node = search_symtable(node, 3);     //< we check if data were rewrote

if (actual_node->key != 3)                          //< we should find node
{
    fprintf(stderr, "Insert table error occured while inserting existing key!\n");
    correct = false;
}
if (strcmp(actual_node->data.id, identifier_1))
{
    fprintf(stderr, "Identifiers not same in node and in passing data!\n");
    correct = false;
}
if (actual_node->data.type != T_FLOAT64)
{
    fprintf(stderr, "Data type not same in node and in passing data!\n");
    correct = false;
}
if (actual_node->data.defined != true || actual_node->data.global != true || actual_node->data.is_function != true)
{
    fprintf(stderr, "One of boolean variables not same in node and in passing data!\n");
    correct = false;
}
if (actual_node->data.param_counter != 1)
{
    fprintf(stderr, "Param counter not same in node and in passing data!\n");
    correct = false;
}
if (correct)
{
    printf("Insertion of data with existing key passed!!!\n");
}

printf("***********************************************\n");
correct = true; //< in case it was set to false in last test unit

/* we gonna to delete one node with existing key without children */ 
delete_node(node, 15);
actual_node = search_symtable(node, 15);
if (actual_node != NULL)
{
    fprintf(stderr, "Failed to delete node with no children!\n");
}
else
{
    printf("Node with no child was deleted successfully!\n");
}


/* we gonna to delete one node with existing key with both children */
delete_node(node, 4);
actual_node = search_symtable(node, 4); //< node 4 not exists, is overwrote with with node 6 
if (actual_node != NULL)
{
    fprintf(stderr, "Failed to delete node with two children\n");
    correct = false;
}
actual_node =search_symtable(node, 6);
if (actual_node->lptr->key != 3)    //< node 6 now points to node 3
{
    fprintf(stderr, "Failed to delete node with two children\n");
    correct = false;
}
if (correct)
{
    printf("Node with both children deleted successfully!\n");
}

correct = true; //< in case it was set to false in last test unit

/* we gonna to delete one node with existing key with one child */
delete_node(node, 6);
actual_node = search_symtable(node, 6); //< node six is deleted now
if (actual_node != NULL)
{
    fprintf(stderr, "Failed to delete node with one child!\n");
    correct = false;
}
if (node->lptr->key != 3)   //< root must point to node 3 now
{
   fprintf(stderr, "Failed to delete node with one child!\n");
   correct = false; 
}
if (correct)
{
    printf("Node with one child was deleted successfully!\n");
}

printf("***********************************************\n");
printf("End of test unit\n");

delete_symtable(node);


free(data_1.id);
free(data_2.id);
free(data_3.id);
free(data_4.id);
free(data_5.id);

return 0;
}