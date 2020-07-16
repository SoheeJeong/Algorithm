#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define INSERT_OP      0x01 //1
#define DELETE_OP      0x02 //2
#define SUBSTITUTE_OP  0x04 //4
#define MATCH_OP       0x08 //8
#define TRANSPOSE_OP   0x10 //16

#define INSERT_COST	1
#define DELETE_COST	1
#define SUBSTITUTE_COST	1 // 2?
#define TRANSPOSE_COST	1 // 4?

void backtrace_main( int *op_matrix, int col_size, char *str1, char *str2, int n, int m, int level, char align_str[][8], int i, int j);
void backtrace( int *op_matrix, int col_size, char *str1, char *str2, int n, int m);
void print_matrix( int *op_matrix, int col_size, int n, int m);
int min_editdistance( char *str1, char *str2);
int __GetMin4( int a, int b, int c, int d);
int __GetMin3( int a, int b, int c);

// op_matrix : 이전 상태에 대한 연산자 정보(backtrace)가 저장된 행렬 (1차원 배열임에 주의!) => I,D,S,M,T bit 쪼개씀
// dist : distance 값 들어있는 1차원 배열
// col_size : op_matrix의 열의 크기
// str1 : 문자열 1
// str2 : 문자열 2
// n : 문자열 1의 길이
// m : 문자열 2의 길이
// level : 재귀호출의 레벨 (0, 1, 2, ...)
// align_str : 정렬된 문자쌍들의 정보가 저장된 문자열 배열 예) "a - a", "a - b", "* - b", "ab - ba"


// 강의 자료의 형식대로 op_matrix를 출력 (좌하단(1,1) -> 우상단(n, m))
// 각 연산자를  괄호 안의 기호로 표시한다. 삽입(I), 삭제(D), 교체(S), 일치(M), 전위(T)
void print_matrix( int *op_matrix, int col_size, int n, int m){
	
	for(int j=m;j>=1;j--){ 
		for(int i=1;i<=n;i++){
			int num=op_matrix[i+j*(n+1)];
			
			if((num & INSERT_OP)==INSERT_OP)
				printf("%c",'I');
			
			if((num & DELETE_OP)==DELETE_OP)
				printf("%c",'D');
			
			if((num & SUBSTITUTE_OP)==SUBSTITUTE_OP)
				printf("%c",'S');
			
			if((num & MATCH_OP)==MATCH_OP)
				printf("%c",'M');
			
			if((num & TRANSPOSE_OP)==TRANSPOSE_OP)
				printf("%c",'T');		
			
			printf("\t");
		}
		printf("\n");
	}	
}

// 두 문자열 str1과 str2의 최소편집거리를 계산한다.
// return value : 최소편집거리
// 이 함수 내부에서 print_matrix 함수와 backtrace 함수를 호출함
int min_editdistance( char *str1, char *str2){
	char *strtemp1=strdup(str1);	char* strtemp2=strdup(str2);
	int n=strlen(strtemp1); //str1, str2 free해줘야됨
	int m=strlen(strtemp2);
	
	//str 비교할때 index 주의
	
	int is_n_smaller = (n<m)? 1 : 0; //n<m이면 is_n_smaller=1
	int smaller=0;
	int col_size=(n+1)*(m+1);
	int dist[col_size+5];	
	int op_matrix[col_size+5];
	
	//dist 배열 초기화
	dist[0]=0;
	for(int j=1;j<=m;j++){	//열방향 
		dist[j*(n+1)]=dist[(j-1)*(n+1)]+1;
	}
	for(int i=1;i<=n;i++){		//행방향 
		dist[i]=dist[i-1]+1;
	}
		
	//op_matrix 초기화
	op_matrix[0]=0;
	for(int j=1;j<=m;j++){	//열방향 D로 초기화
		op_matrix[j*(n+1)]=DELETE_OP; 
	}
		
	for(int i=1;i<=n;i++){		//행방향 I로 초기화
		op_matrix[i]=INSERT_OP;
	}
	
	int dist_I, dist_D, dist_S, dist_T =0;
	
	//dist 비교하면서 min값을 dist에 넣고 op도 채우기
	if(is_n_smaller==1)//n<m
		smaller=n;
	else //n>=m
		smaller=m;
	
	for(int i=1;i<=smaller;i++){		
	
		//(i,i)~(i,m)의 dist 채우기
			for(int j=i;j<=m;j++){
				int min=0;
				
				dist_I=dist[(i-1)+j*(n+1)]+INSERT_COST;
				dist_D=dist[i+(j-1)*(n+1)]+DELETE_COST;
				dist_S=dist[(i-1)+(j-1)*(n+1)]+SUBSTITUTE_COST; //Match면 쓸모없는 값
				dist_T=dist[(i-2)+(j-2)*(n+1)]+TRANSPOSE_COST;
				
				if(strtemp1[i-1]==strtemp2[j-1]){ //Match
					//dist 추가
					dist[i+j*(n+1)]=dist[(i-1)+(j-1)*(n+1)];
					
					//op_matrix 추가
					op_matrix[i+j*(n+1)]=MATCH_OP;
						
					continue;
				}
				else if( i!=1 && j!=1 && strtemp1[i-1]==strtemp2[j-2] && strtemp1[i-2]==strtemp2[j-1] && strtemp1[i-1]!=strtemp1[i-2] ){ //Match 아니고 T 가능
					//dist 추가
					min=__GetMin4(dist_I, dist_D, dist_S, dist_T); //차례로 I,D,S,T 중 dist+cost 의 최소값
					dist[i+j*(n+1)]=min;
					
					//op추가
					op_matrix[i+j*(n+1)]=0;
					if(min==dist_I)	op_matrix[i+j*(n+1)] +=INSERT_OP;
					if(min==dist_D)	op_matrix[i+j*(n+1)] +=DELETE_OP;
					if(min==dist_S)	op_matrix[i+j*(n+1)] +=SUBSTITUTE_OP;
					if(min==dist_T)	op_matrix[i+j*(n+1)] +=TRANSPOSE_OP;
					
					continue;
				}
				else{ //Match 아니고 T 불가능
					//dist 추가
					min=__GetMin3(dist_I, dist_D, dist_S);	//차례로 I,D,S 중 dist+cost 의 최소값
					dist[i+j*(n+1)]=min;
					
					//op추가
					op_matrix[i+j*(n+1)]=0;
					if(min==dist_I)	op_matrix[i+j*(n+1)] +=INSERT_OP;
					if(min==dist_D)	op_matrix[i+j*(n+1)] +=DELETE_OP;
					if(min==dist_S)	op_matrix[i+j*(n+1)] +=SUBSTITUTE_OP;
										
					continue;
				}
			}
		//(i+1,i)~(n,i)의 dist 채우기
			for(int k=i+1;k<=n;k++){ //여기 for문만 index (i,j) 아니라 (k,i)임
				int min=0;
								
				dist_I=dist[(k-1)+i*(n+1)]+INSERT_COST;
				dist_D=dist[k+(i-1)*(n+1)]+DELETE_COST;
				dist_S=dist[(k-1)+(i-1)*(n+1)]+SUBSTITUTE_COST; //Match면 쓸모없는 값
				dist_T=dist[(k-2)+(i-2)*(n+1)]+TRANSPOSE_COST;
				
				if(strtemp1[k-1]==strtemp2[i-1]){ //Match
					//dist 추가
					dist[k+i*(n+1)]=dist[(k-1)+(i-1)*(n+1)];
					
					//op_matrix 추가
					op_matrix[k+i*(n+1)]=MATCH_OP;
					continue;
				}
				else if( k!=1 && i!=1 && strtemp1[k-1]==strtemp2[i-2] && strtemp1[k-2]==strtemp2[i-1] && strtemp1[k-1]!=strtemp1[k-2] ){ //Match 아니고 T 가능
					//dist 추가
					min=__GetMin4(dist_I, dist_D, dist_S, dist_T); //차례로 I,D,S,T 중 dist+cost 의 최소값
					dist[k+i*(n+1)]=min;
					
					//op추가
					op_matrix[k+i*(n+1)]=0;
					if(min==dist_I)	op_matrix[k+i*(n+1)] +=INSERT_OP;
					if(min==dist_D)	op_matrix[k+i*(n+1)] +=DELETE_OP;
					if(min==dist_S)	op_matrix[k+i*(n+1)] +=SUBSTITUTE_OP;
					if(min==dist_T)	op_matrix[k+i*(n+1)] +=TRANSPOSE_OP;
					
					continue;
				}
				else{ //Match 아니고 T 불가능
					//dist 추가
					min=__GetMin3(dist_I, dist_D, dist_S);	//차례로 I,D,S 중 dist+cost 의 최소값
					dist[k+i*(n+1)]=min;
					
					//op추가
					op_matrix[k+i*(n+1)]=0;
					if(min==dist_I)	op_matrix[k+i*(n+1)] +=INSERT_OP;
					if(min==dist_D)	op_matrix[k+i*(n+1)] +=DELETE_OP;
					if(min==dist_S)	op_matrix[k+i*(n+1)] +=SUBSTITUTE_OP;
										
					continue;
				}
			}		
									
		}
	
	//print하기
	print_matrix(op_matrix,col_size, n, m);

	//backtrace 시작
	backtrace(op_matrix, col_size, str1, str2, n, m);
		
	free(strtemp1); free(strtemp2);
	return dist[n+m*(n+1)]; 
}

////////////////////////////////////////////////////////////////////////////////
// 세 정수 중에서 가장 작은 값을 리턴한다.
int __GetMin3( int a, int b, int c)
{
	int min = a;
	if (b < min)
		min = b;
	if(c < min)
		min = c;
	return min;
}

////////////////////////////////////////////////////////////////////////////////
// 네 정수 중에서 가장 작은 값을 리턴한다.
int __GetMin4( int a, int b, int c, int d)
{
	int min = __GetMin3( a, b, c);
	return (min > d) ? d : min;
}

////////////////////////////////////////////////////////////////////////////////
int a=1;
void* count=&a; //print할떄 alignment count용

// 정렬된 문자쌍들을 출력 
void print_alignment( char align_str[][8], int level)
{
	int i;
	
	for (i = level-1; i >= 0; i--)
	{
		printf( "%s\n", align_str[i]);
	}
	
	int num=*((int*)count);
	*((int*)count)=num+1;
}

////////////////////////////////////////////////////////////////////////////////
// backtrace_main을 호출하는 wrapper 함수

void backtrace( int *op_matrix, int col_size, char *str1, char *str2, int n, int m)
{
	char align_str[n+m][8]; // n+m strings
	
	backtrace_main( op_matrix, col_size, str1, str2, n, m, 0, align_str, n, m); 
	
}


// 재귀적으로 연산자 행렬을 순회하며, 두 문자열이 최소편집거리를 갖는 모든 가능한 정렬(alignment) 결과를 출력한다.
void backtrace_main( int *op_matrix, int col_size, char *str1, char *str2, int n, int m, int level, char align_str[][8], int i, int j){ 
	
	if(i==0 && j==0){ // 끝 -> print
		printf("\n[%d] ==============================\n",*((int*)count));
		print_alignment(align_str, level);
		return;
	}
	
	if((op_matrix[i+j*(n+1)] & MATCH_OP)==MATCH_OP){ //M 
		align_str[level][0]=str2[j-1];
		align_str[level][1]=' ';
		align_str[level][2]='-';
		align_str[level][3]=' ';
		align_str[level][4]=str1[i-1];
		align_str[level][5]='\0';
		level++;
		backtrace_main( op_matrix, col_size, str1, str2, n, m, level, align_str, i-1, j-1);
		level--;
	}
	
	if((op_matrix[i+j*(n+1)] & DELETE_OP)==DELETE_OP){ //D
		align_str[level][0]=str2[j-1];
		align_str[level][1]=' ';
		align_str[level][2]='-';
		align_str[level][3]=' ';
		align_str[level][4]='*';
		align_str[level][5]='\0';
		level++;
		backtrace_main( op_matrix, col_size, str1, str2, n, m, level, align_str, i, j-1);
		level--;
	}
		
	if((op_matrix[i+j*(n+1)] & INSERT_OP)==INSERT_OP){ //I
		align_str[level][0]='*';
		align_str[level][1]=' ';
		align_str[level][2]='-';
		align_str[level][3]=' ';
		align_str[level][4]=str1[i-1];
		align_str[level][5]='\0';
		level++;
		backtrace_main( op_matrix, col_size, str1, str2, n, m, level, align_str, i-1, j);
		level--;
	}
	
	if((op_matrix[i+j*(n+1)] & SUBSTITUTE_OP)==SUBSTITUTE_OP){  //S
		align_str[level][0]=str2[j-1];
		align_str[level][1]=' ';
		align_str[level][2]='-';
		align_str[level][3]=' ';
		align_str[level][4]=str1[i-1];
		align_str[level][5]='\0';
		level++;
		backtrace_main( op_matrix, col_size, str1, str2, n, m, level, align_str, i-1, j-1);
		level--;
	}

	if((op_matrix[i+j*(n+1)] & TRANSPOSE_OP)==TRANSPOSE_OP){ //T
		align_str[level][0]=str2[j-2];
		align_str[level][1]=str2[j-1];
		align_str[level][2]=' ';
		align_str[level][3]='-';
		align_str[level][4]=' ';
		align_str[level][5]=str1[i-2];
		align_str[level][6]=str1[i-1];
		align_str[level][7]='\0';
		level++;
		backtrace_main( op_matrix, col_size, str1, str2, n, m, level, align_str, i-2, j-2);
		level--;
	}	
}

////////////////////////////////////////////////////////////////////////////////
int main()
{
	char str1[30];
	char str2[30];
	
	int distance;
	
	printf( "INSERT_COST = %d\n", INSERT_COST);
	printf( "DELETE_COST = %d\n", DELETE_COST);
	printf( "SUBSTITUTE_COST = %d\n", SUBSTITUTE_COST);
	printf( "TRANSPOSE_COST = %d\n", TRANSPOSE_COST);
	
	while( fscanf( stdin, "%s\t%s", str2, str1) != EOF)
	{
		printf( "\n==============================\n");
		printf( "%s vs. %s\n", str2, str1);
		printf( "==============================\n");
		
		distance = min_editdistance( str1, str2);
		
		printf( "\nMinEdit(%s, %s) = %d\n", str1, str2, distance);
	}
	return 0;
}
