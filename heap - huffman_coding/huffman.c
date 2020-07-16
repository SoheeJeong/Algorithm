#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef struct Node 
{ 
	unsigned char	data; 
	int		freq; 
	struct	Node *left;
	struct	Node *right; 
} tNode;

typedef struct
{
	int	last;
	int	capacity;
	tNode **heapArr;
} HEAP;

void heapPrint( HEAP *heap);
void free_huffman_code( char *codes[]);
int encoding( char *codes[], FILE *infp, FILE *outfp);
void decoding( tNode *root, FILE *infp, FILE *outfp);
int read_chars( FILE *fp, int *ch_freq);
tNode *newNode(char data, int freq);
HEAP *heapCreate( int capacity);
void _reheapUp( HEAP *heap, int index);
int heapInsert( HEAP *heap, tNode* newNode);
void _reheapDown( HEAP *heap, int index);
int heapDelete( HEAP *heap);
void heapDestroy( HEAP *heap);
tNode *make_huffman_tree( int *ch_freq, HEAP* myheap);
void traverse_tree( tNode *root, char *code, int depth, char *codes[]);
void make_huffman_code( tNode *root, char *codes[]);
void destroyTree( tNode *root);
void print_char_freq( int *ch_freq);
tNode *run_huffman( int *ch_freq, char *codes[],HEAP* myheap);
void print_huffman_code( char *codes[]);

////////////////////////////////////////////////////////////////////////////////
// 허프만 코드 메모리 해제
void free_huffman_code( char *codes[]){
	for(int i=0;i<256;i++){
		free(codes[i]);		
	}
	return;
}

// 텍스트 파일을 허프만 코드를 이용하여 바이터리 파일로 인코딩
// return value : 인코딩된 파일의 바이트 수
int encoding( char *codes[], FILE *infp, FILE *outfp){
	int in_char;
	char* out_code;
	int count=0;
	
	in_char=getc(infp);
	
	while(in_char!=EOF){
		out_code=codes[in_char];
		fputs(out_code,outfp);
		count+=strlen(out_code);
		in_char=getc(infp);
	}
	return count/8;
}

// 바이너리 파일을 허프만 트리를 이용하여 텍스트 파일로 디코딩
void decoding( tNode *root, FILE *infp, FILE *outfp){
	tNode *temp=root; //temporary root
	int in_char;
	int out_char;
	in_char=getc(infp);
	while(in_char!=EOF){
		if(root){
			
			if(!(root->left||root->right)){ //leaf node
				out_char=root->data;
				putc(out_char,outfp);
				root=temp;
				continue;
			}//if
			
			else{ //not a leaf node
				if(in_char=='0'){ //move left
					root=root->left;
					in_char=getc(infp);
					continue;
				}
				
				else if(in_char=='1'){ //move right
					root=root->right;
					in_char=getc(infp);
					continue;
				}
			}//else
		}//if(root)	
	}//while
}

// 파일에 속한 각 문자(바이트)의 빈도 저장
// return value : 파일에서 읽은 바이트 수
int read_chars( FILE *fp, int *ch_freq){
	int c;
	int n=0;
	c=getc(fp);
	
	while(c!=EOF){
		ch_freq[c]+=1;
		n++;	
		c=getc(fp);			
	}
	return n;
}
// 새로운 노드를 생성
// 좌/우 subtree가 NULL이고 빈도값(freq)이 저장됨
// return value : 노드의 포인터
tNode *newNode(char data, int freq){
	tNode* newNode;
	newNode=(tNode*)malloc(sizeof(tNode));
	if (newNode==NULL)	return NULL;
	
	newNode->left=NULL;
	newNode->right=NULL;
	newNode->freq=freq;
	newNode->data=data;
	
	return newNode;	
}

// 힙 생성
// 배열을 위한 메모리 할당 (capacity)
// last = -1
HEAP *heapCreate( int capacity){
	HEAP *heap;
	
	heap = (HEAP *)malloc( sizeof(HEAP));
	if (!heap) return NULL;

	heap->last = -1;
	heap->capacity = capacity;
	heap->heapArr = (tNode **)malloc( sizeof(tNode*) * capacity);
	if (heap->heapArr == NULL)
	{
		fprintf( stderr, "Error : not enough memory!\n");
		free( heap);
		return NULL;
	}
	return heap;
}

//최소힙 유지
void _reheapUp( HEAP *heap, int index){
	tNode **arr = heap->heapArr;
	int parent;
	
	while(1)
	{
		if (index == 0) return; // root node
		
		parent = (index - 1) / 2;
		
		if (arr[index]->freq < arr[parent]->freq) // exchange (for minheap) //부등호방향 바꿈
		{
			tNode* temp = arr[index];
			arr[index] = arr[parent];
			arr[parent] = temp;
			
			index = parent;
		}
		else return;
	}
}

// 힙에 원소 삽입
// _reheapUp 함수 호출
int heapInsert( HEAP *heap, tNode* newNode){
	if (heap->last == heap->capacity - 1)
		return 0;
	
	(heap->last)++;
	(heap->heapArr[heap->last])= newNode;
	
	_reheapUp( heap, heap->last);
	
	return 1;
}

// 최소힙 유지
void _reheapDown( HEAP *heap, int index){
	tNode**arr = heap->heapArr;
	int leftFreq;
	int rightFreq;
	int noright = 0;
	int smallindex; // index of left or right child with large key
	
	while(1)
	{
		if ((index * 2 + 1) > heap->last) return; // leaf node (there is no left subtree)
		
		leftFreq = arr[index * 2 + 1]->freq;
		if (index * 2 + 2 <= heap->last) rightFreq = arr[index * 2 + 2]->freq;
		else noright = 1;
		
		if (noright || leftFreq < rightFreq) smallindex = index * 2 + 1; // left child
		else smallindex = index * 2 + 2; // right child
		
		if (arr[index]->freq > arr[smallindex]->freq) // exchange (for minheap)
		{
			tNode*temp=arr[index];
			arr[index] = arr[smallindex];
			arr[smallindex] = temp;
			
			index = smallindex;
			
			noright = 0;
		}
		else return;
	}
}
// 최소값 제거
// heapDelete 함수 호출
int heapDelete( HEAP *heap){
	if (heap->last == -1) return 0; // empty heap
	
	heap->heapArr[0] = heap->heapArr[heap->last];
	
	(heap->last)--;
	
	_reheapDown( heap, 0);
	
	return 1;
}

// 힙 메모리 해제
void heapDestroy( HEAP *heap){
	free(heap->heapArr);
	free(heap);
}

// 문자별 허프만 트리를 생성
// capacity 256 짜리 빈(empty) 힙 생성
// 개별 알파벳에 대한 노드 생성
// 힙에 삽입 (minheap 구성)
// 2개의 최소값 추출
// 결합 후 새 노드에 추가
// 힙에 삽입
// 힙에 한개의 노드가 남을 때까지 반복
tNode *make_huffman_tree( int *ch_freq, HEAP* myheap){
	int min1,min2=0;
	int freq1,freq2=0;
	
	for(int i=0;i<256;i++){
		tNode* temp=newNode(i,ch_freq[i]);
		heapInsert(myheap,temp);
	}
	while((myheap->last)!=0){
		tNode* parent=newNode(min1+min2,freq1+freq2);
		parent->left=myheap->heapArr[0];
		min1=(myheap->heapArr[0])->data;
		freq1=(myheap->heapArr[0])->freq;
	
	heapDelete(myheap);
		
		parent->right=myheap->heapArr[0];
		min2=(myheap->heapArr[0])->data;
		freq2=(myheap->heapArr[0])->freq;
	
	heapDelete(myheap);

		parent->freq=freq1+freq2;
	
	heapInsert(myheap,parent); //새로운 부모를 heap에 삽입
	}
	
	if(myheap->last==0){
		return myheap->heapArr[0];
	}
}

// 허프만 트리를 순회하며 허프만 코드를 생성하여 codes에 저장
// leaf 노드에서만 코드를 생성
// strdup 함수 사용함
void traverse_tree( tNode *root, char *code, int depth, char *codes[]){
	if(root){		

		if(!(root->left||root->right)){ //leaf node
			code[depth]=0;
			char* temp=strdup(code);
			codes[root->data]=temp;
			depth--; 
			return;
		}
		
		else{
			if(root->left){ //왼쪽 자식이 존재
				code[depth]='0';
				depth++;
				traverse_tree(root->left,code,depth,codes);		
				code[depth]=' ';
				depth--;
			}
			
			if(root->right){ //오른쪽 자식이 존재
				code[depth]='1';
				depth++;
				traverse_tree(root->right,code,depth,codes);	
				code[depth]=' ';
				depth--;
			}
		}
	}
	return;
}


// 허프만 트리로부터 허프만 코드를 생성
// traverse_tree 함수 호출
void make_huffman_code( tNode *root, char *codes[]){
	char code[256];
	traverse_tree(root,code,0,codes);
}

// 트리 메모리 해제
void destroyTree( tNode *root){
	if(root){
		destroyTree(root->left);
		destroyTree(root->right);
		free(root);
	}
}

////////////////////////////////////////////////////////////////////////////////
// 힙의 내용 출력 (for debugging)
void heapPrint( HEAP *heap){
	int i;
	tNode **p = heap->heapArr;
	int last = heap->last;
	
	for( i = 0; i <= last; i++)
	{
		printf("[%d]%d(%6d)\n", i, p[i]->data, p[i]->freq);
	}
	printf( "\n");
}

////////////////////////////////////////////////////////////////////////////////
// 문자별 빈도 출력 (for debugging)
void print_char_freq( int *ch_freq){
	int i;

	for (i = 0; i < 256; i++)
	{
		printf( "%d\t%d\n", i, ch_freq[i]); // 문자인덱스, 빈도
	}
}

////////////////////////////////////////////////////////////////////////////////
// 문자별 빈도를 이용하여 허프만 트리와 허프만 코드를 생성
// return value : 허프만 트리의 root node
tNode *run_huffman( int *ch_freq, char *codes[],HEAP* myheap){
	tNode *root;
	
	root = make_huffman_tree( ch_freq, myheap);
	
	make_huffman_code( root, codes);
	
	return root;
}

////////////////////////////////////////////////////////////////////////////////
// 허프만 코드를 화면에 출력
void print_huffman_code( char *codes[]){
	int i;
	
	for (i = 0; i < 256; i++)
	{
		printf( "%d\t%s\n", i, codes[i]);
	}
}

////////////////////////////////////////////////////////////////////////////////
// argv[1] : 입력 텍스트 파일
// argv[2] : 바이너리 코드 (텍스트 파일; encoding 결과) 
// argv[3] : 출력 텍스트 파일 (decoding 결과)
int main( int argc, char **argv)
{
	FILE *fp;
	FILE *infp, *outfp;
	int ch_freq[256] = {0,}; // 문자별 빈도
	char *codes[256]; // 문자별 허프만 코드 (ragged 배열)
	tNode *huffman_tree; // 허프만 트리
	HEAP *myheap=heapCreate(256);
	
	if (argc != 4)
	{
		fprintf( stderr, "%s input-file encoded-file decoded-file\n", argv[0]);
		return 1;
	}

	////////////////////////////////////////
	// 입력 텍스트 파일
	fp = fopen( argv[1], "rt");
	if (fp == NULL)
	{
		fprintf( stderr, "Error: cannot open file [%s]\n", argv[1]);
		return 1;
	}

	// 텍스트 파일로부터 문자별 빈도 저장
	// getc 함수 호출
	int num_bytes = read_chars( fp, ch_freq);

	fclose( fp);

	// 문자별 빈도 출력 (for debugging)
	//print_char_freq( ch_freq);
	
	// 허프만 코드/트리 생성
	huffman_tree = run_huffman( ch_freq, codes, myheap);
	
	// 허프만 코드 출력 (stdout)
	print_huffman_code( codes);

	////////////////////////////////////////
	// 입력: 텍스트 파일
	infp = fopen( argv[1], "rt");
	// 출력: 바이너리 코드
	outfp = fopen( argv[2], "wt");
	
	// 허프만코드를 이용하여 인코딩(압축)
	int encoded_bytes = encoding( codes, infp, outfp);

	// 허프만 코드 메모리 해제
	free_huffman_code( codes);
	
	fclose( infp);
	fclose( outfp);

	////////////////////////////////////////
	// 입력: 바이너리 코드
	infp = fopen( argv[2], "rt");
	// 출력: 텍스트 파일
	outfp = fopen( argv[3], "wt");

	// 허프만 트리를 이용하여 디코딩
	decoding( huffman_tree, infp, outfp);

	// 허프만 트리 메모리 해제
	destroyTree( huffman_tree);
	
	//heap 메모리 해제
	heapDestroy(myheap);
	
	fclose( infp);
	fclose( outfp);

	////////////////////////////////////////
	printf( "# of bytes of the original text = %d\n", num_bytes);
	printf( "# of bytes of the compressed text = %d\n", encoded_bytes);
	printf( "compression ratio = %.2f\n", ((float)num_bytes - encoded_bytes) / num_bytes * 100);
	
	return 0;
}