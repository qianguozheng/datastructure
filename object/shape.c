#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct shape;
struct shape_ops
{
	/*返回几何体的面积*/
	float (*so_area)(struct shape*);
	/*返回几何体的周长*/
	int (*so_perimeter)(struct shape*);
};

struct shape
{
	int *s_type;
	char *s_name;
	struct shape_ops* s_ops; /*虚接口，所有子类必须实现*/
};

typedef struct shape* _shape;

float shape_area(_shape s) /*计算形状面积*/
{
	return s->s_ops->so_area(s);
}

int shape_perimeter(_shape s) /*求周长*/
{
	return s->s_ops->so_perimeter(s);
}

/*三角形*/
struct triangle
{
	struct shape t_base;
	int t_side_a;
	int t_side_b;
	int t_side_c;
};

float triangle_area(_shape s)
{
	struct triangle * t = (struct triangle *) s;
	int a = t->t_side_a;
	int b = t->t_side_b;
	int c = t->t_side_c;
	float p = (a+b+c)/2;
	return sqrt(p*(p-a)*(p-b)*(p-c));
}

float triangle_perimeter(_shape *s)
{
	struct triangle * t = (struct triangle *) s;
	return t->t_side_a + t->t_side_b + t->t_side_c;
}

struct shape_ops triangle_ops = /*实现父类虚接口*/
{
	triangle_area,
	triangle_perimeter,
};

_shape triangle_create(int a, int b, int c)
{
	struct triangle* ret = (struct triangle *) malloc(sizeof (*ret));
	ret->t_base.s_name = "triangle";
	ret->t_base.s_ops = &triangle_ops;
	ret->t_side_a = a;
	ret->t_side_b = b;
	ret->t_side_c = c;
	return ret;
}

/*矩形*/
struct rectangle
{
	struct shape r_base;
	int r_width;
	int r_height;
};

float rectangle_area(_shape s)
{
	struct rectangle *t = (struct rectangle *)s;
	return t->r_width * t->r_height;
}

int rectangle_perimeter(_shape *s)
{
	struct rectangle *r = (struct rectangle *)s;
	return (r->r_width + r->r_height) * 2;
}

struct shape_ops rectangle_ops = 
{
	rectangle_area,
	rectangle_perimeter,
};

struct rectangle * rectangle_create(int width, int height)
{
	struct rectangle * ret = (struct rectangle *)malloc(sizeof (*ret));
	ret->r_base.s_name = "rectangle";
	ret->r_base.s_ops = &rectangle_ops;
	ret->r_height = height;
	ret->r_width = width;
	return ret;
}

#if 0
int main(int argc, char *argv[])
{
	struct shape * s[4];
	s[0] = triangle_create(3,4,5);
	s[1] = triangle_create(7,8,9);
	s[2] = rectangle_create(10,20);
	s[3] = rectangle_create(11,22);
	
	int i = 0;
	for (i = 0; i < 4; i++)
	{
		float area = shape_area(s[i]);
		int perimeter = shape_perimeter(s[i]);
		char *name = s[i]->s_name;

		printf("name:%s, area:%.2f, perimeter:%d\n", name, area, perimeter);
	}
	return 0;
}
#endif
