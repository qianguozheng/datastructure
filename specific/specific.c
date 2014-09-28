//Get from http://blog.jobbole.com/77321/
// C specific features got from the Internet.

//switch case 
void switchtest()
{
	int a = 1;
	int b = 3;
	switch(a)
	{
		case 1:
			printf("case 1\n");
			if (b==2)
			{
				case 2:
					printf("case 2\n");
			}
			else case 3:
			{
				for (b=0; b<10; b++)
				{
					case 5:
						printf("case 5\n");
				}
			}
			break;
		case 4:
			printf("case 4\n");
		default:
			break;
	}

}

int add(int a, int b)
{

	/// try to find the rule.
	int i = 0;
	i = printf("%*c", a);
	printf("\ni=%d, a=%d\n", i, a);
	i = printf("%*c", b);
	printf ("\ni=%d, b=%d\n", i, b);

	/// end of the find procedure.
	if(a!=0&&b!=0)
		return printf("%*c%*c", a, '\n', b, '\n');
		//return printf("%*c%*c", a, '\r', b, '\r'); \r or \n as end of printf
	else
		return a!=0?a:b;
}
//???位运算真的应该好好学习
int add2(int x, int y)
{
	if (0 == y)
		return x;
	else
		return add(x^y, (x&y)<<1);
}

void format()
{
	int a= 3;
	float b = 6.1234124;
	//a 是b的小数点的位数， 输出6.123
	printf("%.*f\n", a, b);
}

void posix_ext()
{
	printf("%4$d, %3$d, %2$d, %1$d\n", 1, 2, 3, 9); // 9, 3, 2, 1

	//scanf("%*d%d", &a); 输入1，2只会得到2

	int c='B';
	switch(c)
	{
		case 'A' ... 'X': // GCC extension
			printf("a-x\n");
			break;
		case 1 ... 5:
			printf("1.... 5\n");
			break;
	} 

	printf("%d\n", 0b1101); //prints 13
}

void testscanf()
{
	// 通常使用fget 来读取，sscanf来解析字符串， 因为scanf直接读取会有内存溢出的风险。
	char a[100];
	memset(a, 0, 100);
	scanf("%*s[^\n]\n", a); // read till \n, exclude \n
	printf("a=%s\n", a);
	scanf("%*s[^,]", a);    // read till ',' include ','
	printf("a=%s\n", a);
}
int main(int argc, char *argv[])
{
	//switchtest();

	int *p; /* *p是int类型的, 因此p是指向int类型的指针 */
	int a[5]; /* a[0], ..., a[4] 是int类型的, 因此a是int类型的数组 */
	int *ap[5]; /* *ap[0], .., *ap[4] 是int类型的, 因此ap是包含指向int类型指针的指针数组 */
	int (*pa)[5]; /* (*pa)[0], ..., (*pa)[4] 是int类型的, 因此pa是指向一个int类型数组的指针 */

	struct Foo{
		int x;
		int y;
		int z;
	};
	//在C99之前，你只能按顺序初始化一个结构体。在C99中你可以这样做：
	struct Foo foo = {.z = 3, .x=5};
	printf("foo.x=%d, foo.y=%d, foo.z=%d\n", foo.x, foo.y, foo.z);

	int aa[5] = {[1] = 2, [4] = 5};
	int b[] = {[1] = 2, [4] = 5};
	int c[5]={0, 2, 0, 0, 5};

	int i = 0;
	for(i = 0; i< 5; i++)
	{
		printf("%d\t", aa[i]);
	}
	printf("\n");
	for(i = 0; i< 5; i++)
	{
		printf("%d\t", b[i]);
	}
	printf("\n");
	for(i = 0; i< 5; i++)
	{
		printf("%d\t", c[i]);
	}
	printf("\n");

	//i = printf("sum = %d\n", add(10, 5));
	//printf("i =%d ,sum=%d\n", i, add2(3, 5));
	format();
	posix_ext();

	i=11;
	printf("~-i=%d\n", ~-i);
	printf("-~i=%d\n", -~i);

	printf("a=%s\n", a);

	return 0;
}