#include <stdlib.h> // atoi, rand
#include <stdio.h>
#include <assert.h> // assert
#include <time.h> //time

#define RANGE 100

typedef struct
{
	int x;
	int y;
} point;

typedef struct
{
	point from;
	point to;
} line_segment;

////////////////////////////////////////////////////////////////////////////////
void print_header(char *filename)
{
	printf( "#! /usr/bin/env Rscript\n");
	printf( "png(\"%s\", width=700, height=700)\n", filename);
	
	printf( "plot(1:%d, 1:%d, type=\"n\")\n", RANGE, RANGE);
}
////////////////////////////////////////////////////////////////////////////////
void print_footer(void)
{
	printf( "dev.off()\n");
}

////////////////////////////////////////////////////////////////////////////////
void print_points( point *p, int n)
{
	printf("#points\n");
	
	for(int i=0;i<n;i++)
	{
		printf("points(%d,%d)\n",p[i].x,p[i].y);		
	}
}

// [input] p : set of points
// [input] n : number of points
// [output] l : set of line segments that form the convex hull
// return value : number of line segments
int convex_hull( point *p, int n, line_segment *l)
{
	int line_num=0;
	
	for(int i=0;i<n;i++)
	{
		for(int j=i+1;j<n;j++)
		{
			//x*(p[i].y-p[j].y)/(p[i].x-p[j].x)-y+(p[i].x*p[j].y-p[j].x*p[i].y)/(p[i].x-p[j].x) 직선방정식
			//x*(p[j].y-p[i].y)+y*(p[i].x-p[j].x)-(p[i].x*p[j].y-p[j].x*p[i].y)
			int num=0;
			double var=0; //부호 확인용
			int sign=1;
			int check=0; //전에 0만 나왔나 체크
			
			while(num<n)
			{
				var=p[num].x*(p[j].y-p[i].y)+p[num].y*(p[i].x-p[j].x)-(p[i].x*p[j].y-p[j].x*p[i].y);
				if(var==0)
				{
					num++;
					continue;
				}
				else if(var*sign<0 && check!=0)
					break;
				
				num++;
				
				if(var<0)
					sign=-1;
				else if(var>0)
					sign=1;
				
				check=1;
			}
			
			if(num==n) //모든 점이 같은쪽에 있을 때 -> line_segment에 추가
			{
				l[line_num].from=p[i];
				l[line_num].to=p[j];
				line_num++;
			}	
		}
	}
	
	return line_num;
}

void print_line_segments( line_segment *l, int num_line)
{
	printf("#line segments\n");
	
	for(int i=0;i<num_line;i++)
	{
		printf("segments(%d,%d,%d,%d)\n",(l[i].from).x,(l[i].from).y,(l[i].to).x,(l[i].to).y);
	}
}
////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	int x, y;
	int n; // number of points
	
	if (argc != 2)
	{
		printf( "%s number_of_points\n", argv[0]);
		return 0;
	}

	n = atoi( argv[1]);
	if (n <= 0)
	{
		printf( "The number of points should be a positive integer!\n");
		return 0;
	}

	point p[n+5];
	line_segment l[n+5];

	fprintf( stderr, "Create %d points\n", n);

	// making n points
	srand( time(NULL));
	for (int i = 0; i < n; i++)
	{
		x = rand() % RANGE + 1; // 1 ~ RANGE random number
		y = rand() % RANGE + 1;
		
		p[i].x = x;
		p[i].y = y;
 	}

	print_header( "convex.png");

	print_points( p, n);
	
	int num_line = convex_hull( p, n, l);

	print_line_segments( l, num_line);

	print_footer();
	
	return 0;
}
