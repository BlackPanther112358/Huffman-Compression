#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

typedef struct HuffmanNode{
	char val;
	int freq;
	struct HuffmanNode* left;
	struct HuffmanNode* right;
}Node;

typedef struct MinHeap{
	int size;
	Node** arr;
}heap;

typedef struct Tree_node{
	Node* val;
	struct Tree_node* prev;
	struct Tree_node* next;
}treenode;

typedef struct bool_node{
	bool val;
	struct bool_node* prev;
	struct bool_node* next;
}buffernode;

typedef struct Node_Queue{
	int size;
	treenode* head;
	treenode* tail;
}tree_queue;

typedef struct Bool_Queue{
	int size;
	buffernode* head;
	buffernode* tail;
}buffer;

Node* root;
int freq[128];
char inpt_file[255], outpt_file[255];
char codes[128][16];
buffer* inpt_dump;

void output_tree(Node* node){
	if(node->val == (char)0){
		printf("ROOT ");
		output_tree(node->left);
		output_tree(node->right);
	}else printf("%c ", node->val);
	return;
}

void output_tree_queue(tree_queue* data){
	treenode* itr = (data->head)->next;
	while(itr != NULL){
		if((itr->val)->val == (char)0) printf("ROOT ");
		else printf("%c ", (itr->val)->val);
		itr = itr->next;
	}
	printf("\n");
}

void tree_queue_push(tree_queue* data, Node* val){
	treenode* ele = (treenode*)malloc(sizeof(treenode));
	ele->val = val;
	// printf("%s\n", (ele->val)->val);
	ele->prev = data->tail;
	ele->next = NULL;
	(data->tail)->next = ele;
	data->tail = (data->tail)->next;
	data->size++;
	return;
}

Node* tree_queue_pop(tree_queue* data){
	treenode* ele = (data->head)->next;
	(data->head)->next = ((data->head)->next)->next;
	data->size--;
	if(data->size == 0) data->tail = data->head;
	return (ele->val);
}

void buffer_push(buffer* data, bool val){
	buffernode* ele = (buffernode*)malloc(sizeof(buffernode));
	ele->val = val;
	ele->prev = data->tail;
	ele->next = NULL;
	(data->tail)->next = ele;
	data->tail = (data->tail)->next;
	data->size++;
	return;
}

bool buffer_pop(buffer* data){
	buffernode* ele = (data->head)->next;
	(data->head)->next = ((data->head)->next)->next;
	data->size--;
	if(data->size == 0) data->tail = data->head;
	return ele->val;
}

void push(heap* data, Node* node){
	data->arr[data->size] = node;
	int curr = data->size;
	data->size++;
	while(curr > 0){
		int parent = (curr - 1)/2;
		if((data->arr[parent])->freq < (data->arr[curr])->freq) break;
		Node* extra = data->arr[parent];
		data->arr[parent] = data->arr[curr];
		data->arr[curr] = extra;
		curr = parent;
	}
	return;
}

Node* pop(heap* data){
	Node* del_node = data->arr[0];
	data->size--;
	data->arr[0] = data->arr[data->size];
	int curr = 0, child = 1;
	while(child < data->size){
		if(((child + 1) < data->size) && ((data->arr[child + 1])->freq < (data->arr[child])->freq)) child++;
		if((data->arr[child])->freq < (data->arr[curr])->freq){
			Node* extra = data->arr[child];
			data->arr[child] = data->arr[curr];
			data->arr[curr] = extra;	
		}else break;
		curr = child;
		child = 2*curr + 1;
	}
	return del_node;
}

void insert_heap(heap* data, char val, int freq){
	Node* node = (Node*)malloc(sizeof(Node));
	node->val = val;
	node->freq = freq;
	node->left = NULL;
	node->right = NULL;
	push(data, node);
}

void make_tree(){
	heap* data = (heap*)malloc(sizeof(heap));
	data->size = 0;
	data->arr = (Node**)malloc(sizeof(Node*)*128);
	for(int i = 0; i < 128; i++){
		if(freq[i] == 0) continue;
		insert_heap(data, (char)i, freq[i]);
	}
	if(data->size == 0){
		printf("Empty Input\n");
		exit(0);
	}
	while(data->size > 1){
		Node* node = (Node*)malloc(sizeof(Node));
		node->left = pop(data);
		node->right = pop(data);
		node->freq = (node->left)->freq + (node->right)->freq;
		node->val = (char)0;
		push(data, node);
	}
	root = pop(data);
	return;
}

void get_codes(Node* node, int len, char* code){
	if(node->val == (char)0){
		code[len] = '0';
		get_codes(node->left, len + 1, code);
		code[len] = '1';
		get_codes(node->right, len + 1, code);
	}else{
		for(int i = 0; i < len; i++){
			codes[(int)node->val][i] = code[i];
		}
		codes[(int)node->val][len] = (char)0;
	}
	return;
}

char get_char(Node* node){
	if(node->val != (char)0) return node->val;
	bool dir = buffer_pop(inpt_dump);
	if(dir) return get_char(node->right);
	else return get_char(node->left);
}

void write_metadata(){
	tree_queue* data = (tree_queue*)malloc(sizeof(tree_queue));
	data->size = 0;
	data->head = (treenode*)malloc(sizeof(treenode));
	(data->head)->prev = NULL;
	(data->head)->next = NULL;
	data->tail = data->head;
	tree_queue_push(data, root);
	FILE* outpt = fopen(outpt_file, "a");
	while(data->size > 0){
		Node* node = tree_queue_pop(data);
		fprintf(outpt, "%c", node->val);
		if(node->val == (char)0){
			tree_queue_push(data, node->left);
			tree_queue_push(data, node->right);
		}
	}
	fclose(outpt);
	return;
}

void encode(){
	buffer* data = (buffer*)malloc(sizeof(buffer));
	data->size = 0;
	data->head = (buffernode*)malloc(sizeof(buffernode));
	(data->head)->prev = NULL;
	(data->head)->next = NULL;
	data->tail = data->head;
	FILE* inpt = fopen(inpt_file, "r");
	FILE* outpt = fopen(outpt_file, "a");
	char buff[2];
	fscanf(inpt, "%c", &buff[0]);
	while(feof(inpt) == 0){
		for(int i = 0; i < 16; i++){
			if(codes[buff[0]][i] == (char)0) break;
			bool val = 0;
			if(codes[buff[0]][i] == '1') val = 1;
			buffer_push(data, val);
		}
		while(data->size >= 7){
			int ascii = 0;
			for(int i = 0; i < 7; i++){
				ascii <<= 1;
				ascii += buffer_pop(data);
			}
			fprintf(outpt, "%c", ascii);
		}
		fscanf(inpt, "%c", &buff[0]);
	}
	for(int i = 0; i < 16; i++){
		if(codes[27][i] == (char)0) break;
		bool val = 0;
		if(codes[27][i] == '1') val = 1;
		buffer_push(data, val);
	}
	while(data->size >= 7){
		int ascii = 0;
		for(int i = 0; i < 7; i++){
			ascii <<= 1;
			ascii += buffer_pop(data);
		}
		fprintf(outpt, "%c", ascii);
	}
	if(data->size != 0){
		while(data->size < 7) buffer_push(data, false);
		int ascii = 0;
		for(int i = 0; i < 7; i++){
			ascii <<= 1;
			ascii += buffer_pop(data);
		}
		fprintf(outpt, "%c", ascii);
	}
	fclose(inpt);
	fclose(outpt);
}

void decode(){
	FILE* outpt = fopen(outpt_file, "w");
	while(inpt_dump->size > 0){
		char val = get_char(root);
		if(val == (char)27) break;
		fprintf(outpt, "%c", val);
	}
	fclose(outpt);
}

void compress_input(){
	for(int i = 0; i < 128; i++) freq[i] = 0;
	FILE* inpt = fopen(inpt_file, "r");
	char buff[2];
	fscanf(inpt, "%c", &buff[0]);
	while(feof(inpt) == 0){
		freq[(int)buff[0]]++;
		fscanf(inpt, "%c", &buff[0]);
	}
	freq[27]++;
	fclose(inpt);
	return;
}

void compress_output(){
	FILE* outpt = fopen(outpt_file, "w");
	fprintf(outpt, "");
	fclose(outpt);
	write_metadata();
	encode();
	return;
}

void decompress_input(){
	FILE* inpt = fopen(inpt_file, "r");
	tree_queue* data = (tree_queue*)malloc(sizeof(tree_queue));
	data->size = 0;
	data->head = (treenode*)malloc(sizeof(treenode));
	(data->head)->prev = NULL;
	(data->head)->next = NULL;
	data->tail = data->head;
	char buff[2];
	fscanf(inpt, "%c", &buff[0]);
	root = (Node*)malloc(sizeof(Node));
	root->val = buff[0];
	root->right = NULL;
	root->left = NULL;
	tree_queue_push(data, root);
	while(data->size > 0){
		tree_queue* temp = (tree_queue*)malloc(sizeof(tree_queue));
		temp->size = 0;
		temp->head = (treenode*)malloc(sizeof(treenode));
		(temp->head)->prev = NULL;
		(temp->head)->next = NULL;
		temp->tail = temp->head;
		while(data->size > 0){
			Node* node = tree_queue_pop(data);
			if(node->val == (char)0){
				fscanf(inpt, "%c", &buff[0]);
				Node* node1 = (Node*)malloc(sizeof(Node));
				node1->val = buff[0];
				node1->right = NULL;
				node1->left = NULL;
				tree_queue_push(temp, node1);
				fscanf(inpt, "%c", &buff[0]);
				Node* node2 = (Node*)malloc(sizeof(Node));
				node2->val = buff[0];
				node2->right = NULL;
				node2->left = NULL;
				tree_queue_push(temp, node2);
				node->left = node1;
				node->right = node2;
			}
		}
		data = temp;
	}
	inpt_dump = (buffer*)malloc(sizeof(buffer));
	inpt_dump->size = 0;
	inpt_dump->head = (buffernode*)malloc(sizeof(buffernode));
	(inpt_dump->head)->prev = NULL;
	(inpt_dump->head)->next = NULL;
	inpt_dump->tail = inpt_dump->head;
	fscanf(inpt, "%c", &buff[0]);
	while(feof(inpt) == 0){
		int num_val = (int)buff[0], ext = 64;
		while(ext > 0){
			if(num_val&ext) buffer_push(inpt_dump, true);
			else buffer_push(inpt_dump, false);
			ext >>= 1;
		}
		fscanf(inpt, "%c", &buff[0]);
	}
	fclose(inpt);
}

void compress(){
	compress_input();	
	make_tree();
	char code[16];
	get_codes(root, 0, code);
	for(int i = 0; i < 128; i++){
		if(i == 27) continue;
		if(strlen(codes[i]) == 0) continue;
		printf("'%c' -> %s\n", (char)i, codes[i]);
	}
	printf("\nGenerating %s\n", outpt_file);
	compress_output();
	return;
}

void decompress(){
	decompress_input();
	printf("\nGenerating %s\n", outpt_file);
	decode();
	return;
}

int main(){
	int mode;
	printf("Enter the input file: ");
	scanf("%s", &inpt_file[0]);
	printf("\nEnter the output file: ");
	scanf("%s", &outpt_file[0]);
	printf("\nEnter the mode of input: ");
	scanf("%d", &mode);
	printf("\n");
	if(mode == 0) compress();
	else decompress();
	return 0;
}