#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define PEASANT 0x08
#define WOLF	0x04
#define GOAT	0x02
#define CABBAGE	0x01

// 인접행렬로 표현된 graph를 화면에 출력
void print_graph( int graph[][16], int num){
		
	for(int i=0;i<16;i++){
		for(int j=0;j<16;j++){
			printf("%d ",graph[i][j]);
		}
		printf("\n");
	}
	
}

// 주어진 상태 state의 마지막 4비트를 파일에 출력 (화면 출력은 stdout)
void bit_print(FILE *fp,int state)
{ 
	int temp=8;
	fprintf(fp,"\"");
	for(int i=0;i<4;i++)
	{
		temp=temp>>i;
		if(state&temp)
			fprintf(fp,"%d",1);
		else
			fprintf(fp,"%d",0);
		temp=8;
	}
	fprintf(fp,"\"\n");
}

//dfs에서 print
void bit_print_dfs(int state)
{ 
	int temp=8;
	for(int i=0;i<4;i++)
	{
		temp=temp>>i;
		if(state&temp)
			printf("%d",1);
		else
			printf("%d",0);
		temp=8;
	}
	printf("\n");
}

// 주어진 상태 state에서 농부, 늑대, 염소, 양배추의 상태를 각각 추출하여 p, w, g, c에 저장
void get_pwgc( int state, int *p, int *w, int *g, int *c)
{
	if(state&8)
		*p=1;
	else *p=0;
	if(state&4)
		*w=1;
	else *p=0;

	if(state&2)
		*g=1;
	else *g=0;
	
	if(state&1)
		*c=1;
	else *c=0;
}

// 주어진 그래프(graph)를 .net 파일로 저장
// pgwc.net 참조
void save_graph( char *filename, int graph[][16], int num){
	
	FILE *pFile=fopen(filename,"w");
	
	fprintf(pFile,"*Vertices 16\n");
	for(int i=0;i<num;i++)
	{
		fprintf(pFile," "); if(i<9) fprintf(pFile," ");

		fprintf(pFile,"%d ",i+1);
		bit_print(pFile,i);
	}
	/////////////////////////////
	fprintf(pFile,"*Edges\n");
	for(int i=0;i<num/2;i++)
	{
		for(int j=0;j<num;j++)
		{
			if(graph[i][j])
				fprintf(pFile,"  %d  %d\n",i+1,j+1);
		}
	}
	
	fclose(pFile);
}

// 허용되지 않는 상태인지 검사
// 예) 농부없이 늑대와 염소가 같이 있는 경우 / 농부없이 염소와 양배추가 같이 있는 경우
// return value: 1 허용되지 않는 상태인 경우, 0 허용되는 상태인 경우
int is_dead_end( int state)
{
	int dead_end=0;
	switch(state){
		case 0:	dead_end=0; break;
		case 1:	dead_end=0; break;
		case 2:	dead_end=0; break;
		case 3: dead_end=1; break;
		case 4: dead_end=0; break;
		case 5: dead_end=0; break;
		case 6: dead_end=1; break;
		case 7: dead_end=1; break;
		////////////대칭//////////
		case 8: dead_end=1; break;
		case 9: dead_end=1; break;
		case 10: dead_end=0; break;
		case 11: dead_end=0; break;
		case 12: dead_end=1; break;
		case 13: dead_end=0; break;
		case 14: dead_end=0; break;
		case 15: dead_end=0; break;
		default: dead_end=1;
	}
	return dead_end;
}

// state1 상태에서 state2 상태로의 전이 가능성 점검
// 농부 또는 농부와 다른 하나의 아이템이 강 반대편으로 이동할 수 있는 상태만 허용
// return value: 1 전이 가능한 경우, 0 전이 불이가능한 경우 
int is_possible_transition( int state1,	int state2)
{
	if(is_dead_end(state1)) return 0; //state1가 불가능한 상태
	if(is_dead_end(state2))	return 0; //state2가 불가능한 상태
	
	if((state1&8)==(state2&8)) return 0; //농부가 이동하지 않는 경우
	else{ //농부 이동
		if(((state1&4)!=(state2&4)) && ((state1&2)==(state2&2)) && ((state1&1)==(state2&1))) //changePW
			return 1;
		else if(((state1&4)==(state2&4)) && ((state1&2)!=(state2&2)) && ((state1&1)==(state2&1))) //changePG
			return 1;
		else if(((state1&4)==(state2&4)) && ((state1&2)==(state2&2)) && ((state1&1)!=(state2&1))) //changePC
			return 1;
		else if(((state1&4)==(state2&4)) && ((state1&2)==(state2&2)) && ((state1&1)==(state2&1))) //changeP
			return 1;
		else 
			return 0;
	}
	return 0;	
}

// 상태들의 인접 행렬을 구하여 graph에 저장
// 상태간 전이 가능성 점검
// 허용되지 않는 상태인지 점검 
void make_adjacency_matrix( int graph[][16]) /////////////////
{
	//graph 초기화
	for(int state1=0;state1<16;state1++){
		for(int state2=0;state2<16;state2++){
			
			if( !(is_dead_end(state1)) && !(is_dead_end(state2)) && !((state1&8)==(state2&8)) ) //state1가 불가능한 상태 //state2가 불가능한 상태 //농부가 이동하지 않는 경우
			{ 
				if(((state1&4)!=(state2&4)) && ((state1&2)==(state2&2)) && ((state1&1)==(state2&1))) //changePW
					graph[state1][state2]=1; 
				else if(((state1&4)==(state2&4)) && ((state1&2)!=(state2&2)) && ((state1&1)==(state2&1))) //changePG
					graph[state1][state2]=1;
				else if(((state1&4)==(state2&4)) && ((state1&2)==(state2&2)) && ((state1&1)!=(state2&1))) //changePC
					graph[state1][state2]=1;
				else if(((state1&4)==(state2&4)) && ((state1&2)==(state2&2)) && ((state1&1)==(state2&1))) //changeP
					graph[state1][state2]=1;
				else 
					 graph[state1][state2]=0;
			}
		}
	}
}

// 상태 변경: 농부 이동
// return value : 새로운 상태
int changeP( int state)
{
	if(state&8)
		state=state-8;
	else
		state=state+8;
	return state;
}

// 상태 변경: 농부, 늑대 이동
// return value : 새로운 상태
int changePW( int state)
{
	if((state&8)==((state<<1)&8)){
		if((state&12)==12)
			state=state-12;
		else
			state=state+12;
	}
	return state;
}

// 상태 변경: 농부, 염소 이동
// return value : 새로운 상태
int changePG( int state)
{
	if((state&8)==((state<<2)&8)){
		if((state&10)==10){
			state=state-10;
		}
		else
			state=state+10;
	}
	
	return state;
}

// 상태 변경: 농부, 양배추 이동
// return value : 새로운 상태
int changePC( int state)
{
	if((state&8)==((state<<3)&8)){
		if((state&9)==9)
			state=state-9;
		else
			state=state+9;
	}
	return state;	
}

// 방문한 상태인지 검사
// return value : 1 visited, 0 not visited
int is_visited( int *visited, int level, int state)
{
	for(int i=0;i<=level;i++)
	{
		if(visited[i]==state)
			return 1;
	}
	
	return 0;
}

// 방문한 상태들을 차례로 화면에 출력
void print_states( int *visited, int count)
{
	printf("Goal-state found!\n");
	int temp_state=0;
	for(int i=0;i<=count;i++)
	{
		if(i>0)
			if(visited[i]==visited[i-1])
				continue;
		bit_print_dfs(visited[i]);
	}
	printf("\n");
}


void dfs_main( int state, int goal_state, int level, int *visited)
{	
//////0-10-2-14-4-13-5-15//////0-10-2-11-1-13-5-15///////////
	if(state==goal_state) 
	{
		visited[level]=state;
		print_states(visited,level);
		level++;
		return; 
	}

	if(is_possible_transition(state,changeP(state)) && !is_visited(visited,level,changeP(state)) )
	{
		visited[level]=state; 
		level++;
		dfs_main(changeP(state),goal_state,level,visited);
	}
	
	if(is_possible_transition(state,changePW(state)) && !is_visited(visited,level,changePW(state)) )
	{
		visited[level]=state; 
		level++;;
	
		dfs_main(changePW(state),goal_state,level,visited);
	}
	
	if(is_possible_transition(state,changePG(state)) && !is_visited(visited,level,changePG(state)))
	{
		visited[level]=state; 
		level++;

		dfs_main(changePG(state),goal_state,level,visited);
	}
	
	if(is_possible_transition(state,changePC(state)) && !is_visited(visited,level,changePC(state)) )
	{
		visited[level]=state; 
		level++;		
		dfs_main(changePC(state),goal_state,level,visited);
	}
		visited[level]=state;
		return;

}

// 깊이 우선 탐색 (초기 상태 -> 목적 상태)
void depth_first_search( int init_state, int goal_state)
{
	int level=0;
	int visited[16]={0};
	dfs_main(init_state,goal_state,level,visited);
	//free(visited);
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	int graph[16][16] = {0,};
	// 인접 행렬 만들기
	make_adjacency_matrix( graph);

	// 인접 행렬 출력
	//print_graph( graph, 16);
	
	// .net 파일 만들기 pajek
	save_graph( "pwgctest.net", graph, 16);

	//////////////////////////////////////////
	depth_first_search( 0, 15); // initial state, goal state
	
	return 0;
}
