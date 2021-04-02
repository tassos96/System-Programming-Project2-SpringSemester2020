#ifndef AVL_TREE_H
#define AVL_TREE_H

#include "record.h"
#include "myDateStructure.h"

typedef struct TreeNode  
{ 
    DateStructure dateKey;
    RecordListNode recordNode;

    struct TreeNode *left; 
    struct TreeNode *right; 
    int height; 
}TreeNode; 

typedef TreeNode* AvlTreeNode;

//Function to insert a node at Avl Tree
AvlTreeNode insertNode(AvlTreeNode treeNode, DateStructure dateKey, RecordListNode recordNode);
//Function to get tree's height
int height(AvlTreeNode treeNode);
//Function for right rotation
AvlTreeNode rotateRight(AvlTreeNode x);
//Function for left rotation
AvlTreeNode rotateLeft(AvlTreeNode x);
// All possible rotation compinations following
AvlTreeNode RR(AvlTreeNode treeNode);
AvlTreeNode LL(AvlTreeNode treeNode);
AvlTreeNode LR(AvlTreeNode treeNode);
AvlTreeNode RL(AvlTreeNode treeNode);
//Function to get balance factor
int balanceFactor(AvlTreeNode treeNode);
//Fuction to print AVL tree in PreOrder traversal 
void printPreOrder(AvlTreeNode treeNode);
//Function to print AVL tree in InOrder traversal
void printInOrder(AvlTreeNode treeNode);
//Function to free all AVL tree memory allocated
void freeAvlTree(AvlTreeNode treeNode);

//----------    Functions for interface commands ----------//
void summaryStatisticsCountNodes(AvlTreeNode treeNode, char* dateName, char* diseaseName, int* counterRange_0_20, int *counterRange_21_40, int* counterRange_41_60, int* counterRange_61_max);

#endif