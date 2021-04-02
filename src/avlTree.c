#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/avlTree.h"

//Function to insert a node at Avl Tree
//treeNode: A tree node pointer
//dateKey: The tree's node key for insertion (Date struct)
//recordNode: Pointer to the recordsList node that tree node have to point
AvlTreeNode insertNode(AvlTreeNode treeNode, DateStructure dateKey, RecordListNode recordNode)
{
	if(treeNode==NULL)	//if its the first node (tree's root)
	{
		treeNode = malloc(sizeof(TreeNode));

		treeNode->dateKey = dateKey;
		treeNode->recordNode = recordNode;

		treeNode->left=NULL;
		treeNode->right=NULL;
	}
	else
	{
		if(compareDateKey(dateKey, treeNode->dateKey) == 1)		// insert in right subtree, if datekey > treeNode's dateKey
		{
			treeNode->right = insertNode(treeNode->right, dateKey, recordNode);
			if(balanceFactor(treeNode) == -2)	//Re-balance tree
			{
				if(compareDateKey(dateKey, treeNode->right->dateKey) == 1)
				{
					treeNode = RR(treeNode);
				}
				else
				{
					treeNode = RL(treeNode);
				}
			}
		}
		else
		{
			if(compareDateKey(dateKey, treeNode->dateKey) == -1 ||compareDateKey(dateKey, treeNode->dateKey) ==  0) //insert in left subtree, if dateKey <= treeNode's dateKey
			{
				treeNode->left = insertNode(treeNode->left, dateKey, recordNode);
				if(balanceFactor(treeNode) == 2) //Re-Balance tree
				{
					if(compareDateKey(dateKey, treeNode->left->dateKey) == -1 ||compareDateKey(dateKey, treeNode->left->dateKey) == 0)
					{
						treeNode = LL(treeNode);
					}
					else
					{
						treeNode = LR(treeNode);
					}
				}
			}
		}
	}	
	
	treeNode->height = height(treeNode);	
	return(treeNode);
}
 
//Function to get tree's height
//treeNode: A tree node pointer 
int height(AvlTreeNode treeNode)
{
	int leftHeight;
	int rightHeight;

	if(treeNode == NULL)
		return(0);
	
	if(treeNode->left == NULL)
		leftHeight = 0;
	else
		leftHeight = 1 + treeNode->left->height;
		
	if(treeNode->right == NULL)
		rightHeight = 0;
	else
		rightHeight = 1 + treeNode->right->height;
	
	if(leftHeight > rightHeight)
		return(leftHeight);
	
	return(rightHeight);
}

//Function for right rotation 
AvlTreeNode rotateRight(AvlTreeNode x)
{
	AvlTreeNode y;

	y = x->left;
	x->left = y->right;
	y->right = x;
	
	x->height = height(x);
	y->height = height(y);
	
	return(y);
}

//Function for left rotation 
AvlTreeNode rotateLeft(AvlTreeNode x)
{
	AvlTreeNode y;
	
	y = x->right;
	x->right = y->left;
	y->left = x;
	
	x->height = height(x);
	y->height = height(y);

	return(y);
}

 
// All possible rotation compinations following
AvlTreeNode RR(AvlTreeNode treeNode)
{
	treeNode = rotateLeft(treeNode);
	return(treeNode);
}
 
AvlTreeNode LL(AvlTreeNode treeNode)
{
	treeNode = rotateRight(treeNode);
	return(treeNode);
}
 
AvlTreeNode LR(AvlTreeNode treeNode)
{
	treeNode->left = rotateLeft(treeNode->left);
	treeNode = rotateRight(treeNode);
	
	return(treeNode);
}
 
AvlTreeNode RL(AvlTreeNode treeNode)
{
	treeNode->right = rotateRight(treeNode->right);
	treeNode = rotateLeft(treeNode);
	
	return(treeNode);
}

//Function to get balance factor 
int balanceFactor(AvlTreeNode treeNode)
{
	int leftHeight;
	int rightHeight;

	if(treeNode == NULL)
		return(0);
 
	if(treeNode->left == NULL)
		leftHeight = 0;
	else
		leftHeight = 1 + treeNode->left->height;
 
	if(treeNode->right == NULL)
		rightHeight = 0;
	else
		rightHeight = 1 + treeNode->right->height;
 
	return(leftHeight - rightHeight);
}

//Fuction to print AVL tree in PreOrder traversal 
void printPreOrder(AvlTreeNode treeNode)
{
	if(treeNode != NULL)
	{
		printf("\t\t\t  BF=%d: %d-%d-%d\n",balanceFactor(treeNode),treeNode->dateKey->day, treeNode->dateKey->month, treeNode->dateKey->year);
		printPreOrder(treeNode->left);
		printPreOrder(treeNode->right);
	}
}

//Function to print AVL tree in InOrder traversal 
void printInOrder(AvlTreeNode treeNode)
{
	if(treeNode != NULL)
	{
		printInOrder(treeNode->left);
		printf("\t\t\t  BF=%d: %d-%d-%d\n",balanceFactor(treeNode),treeNode->dateKey->day, treeNode->dateKey->month, treeNode->dateKey->year);
		printInOrder(treeNode->right);
	}
}

//Function to free all AVL tree memory allocated
void freeAvlTree(AvlTreeNode treeNode)
{
	AvlTreeNode temporary = treeNode;
	if(temporary != NULL)
	{
		freeAvlTree(temporary->left);
		freeAvlTree(temporary->right);
		free(temporary);
	}
}

//--------------------    Functions for interface commands    --------------------//

void summaryStatisticsCountNodes(AvlTreeNode treeNode, char* dateName, char* diseaseName, int* counterRange_0_20, int *counterRange_21_40, int* counterRange_41_60, int* counterRange_61_max)
{
	if(treeNode != NULL)
	{
		summaryStatisticsCountNodes(treeNode->left, dateName, diseaseName, counterRange_0_20, counterRange_21_40, counterRange_41_60, counterRange_61_max);
		char dateKeyString[11];
		sprintf(dateKeyString, "%02d-%02d-%02d", treeNode->dateKey->day, treeNode->dateKey->month, treeNode->dateKey->year);
		//printf("%s\n", dateKeyString);
		////printf("%s\n", dateName);
		if((strcmp(dateKeyString, dateName) == 0) && (strcmp(treeNode->recordNode->diseaseID, diseaseName) == 0))
		{
			if(treeNode->recordNode->patientAge >= 0 && treeNode->recordNode->patientAge <= 20)
			{
				(*counterRange_0_20)++;
				//printf("TEST_1\n");
			}
			else if(treeNode->recordNode->patientAge >= 21 && treeNode->recordNode->patientAge <= 40)
			{
				(*counterRange_21_40)++;
				//printf("TEST_2\n");
			}
			else if(treeNode->recordNode->patientAge >= 41 && treeNode->recordNode->patientAge <= 60)
			{
				(*counterRange_41_60)++;
				//printf("TEST_3\n");
			}
			else if(treeNode->recordNode->patientAge >= 61)
			{
				(*counterRange_61_max)++;
				//printf("TEST_4\n");
			}
		}
		summaryStatisticsCountNodes(treeNode->right, dateName, diseaseName, counterRange_0_20, counterRange_21_40, counterRange_41_60, counterRange_61_max);		
	}
}