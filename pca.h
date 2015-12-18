
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*
   用豪斯荷尔德（Householder）变换将n阶实对称矩阵约化为对称三对角阵
*/
void Householder_Tri_Symetry_Diagonal( double a[], int n, double q[],
                                       double b[], double c[] )
{
	int i, j, k, u;
	double h, f, g, h2;

	for ( i = 0; i <= n-1; i++ )
		for ( j = 0; j <= n-1; j++ ) {
			u = i * n + j;
			q[u] = a[u];
		}
	for ( i = n-1; i >= 1; i-- ) {
		h = 0.0;
		if ( i > 1 )
			for ( k = 0; k <= i-1; k++ ) {
				u = i * n + k;
				h = h + q[u] * q[u];
			}
		if ( h + 1.0 == 1.0 ) {
			c[i] = 0.0;
			if ( i == 1 ) c[i] = q[i*n+i-1];
			b[i] = 0.0;
		} else {
			c[i] = sqrt( h );
			u = i * n + i - 1;
			if ( q[u] > 0.0 ) c[i] = -c[i];
			h = h - q[u] * c[i];
			q[u] = q[u] - c[i];
			f = 0.0;
			for ( j = 0; j <= i - 1; j++ ) {
				q[j*n+i] = q[i*n+j] / h;
				g = 0.0;
				for ( k = 0; k <= j; k++ )
					g = g + q[j*n+k] * q[i*n+k];
				if ( j + 1 <= i-1 )
					for ( k = j+1; k <= i-1; k++ )
						g = g + q[k*n+j] * q[i*n+k];
				c[j] = g / h;
				f = f + g * q[j*n+i];
			}
			h2 = f / ( h + h );
			for ( j = 0; j <= i-1; j++ ) {
				f = q[i*n+j];
				g = c[j] - h2 * f;
				c[j] = g;
				for ( k = 0; k <= j; k++ ) {
					u = j * n + k;
					q[u] = q[u] - f * c[k] - g * q[i*n+k];
				}
			}
			b[i] = h;
		}
	}
	for ( i = 0; i <= n-2; i++ ) c[i] = c[i+1];
	c[n-1] = 0.0;
	b[0] = 0.0;
	for ( i = 0; i <= n-1; i++ ) {
		if ( ( b[i] != 0.0 ) && ( i-1 >= 0 ) )
			for ( j = 0; j <= i-1; j++ ) {
				g = 0.0;
				for ( k = 0; k <= i-1; k++ )
					g = g + q[i*n+k] * q[k*n+j];
				for ( k = 0; k <= i-1; k++ ) {
					u = k * n + j;
					q[u] = q[u] - g * q[k*n+i];
				}
			}
		u = i * n + i;
		b[i] = q[u];
		q[u] = 1.0;
		if ( i - 1 >= 0 )
			for ( j = 0; j <= i - 1; j++ ) {
				q[i*n+j] = 0.0;
				q[j*n+i] = 0.0;
			}
	}

	return;
}

/*
   计算实对称三对角阵的全部特征值与对应特征向量
   int n    --- 实对称三对角阵的阶数
   double b --- 长度为n，存放n阶对称三对角阵的主对角线上的各元素；
                返回时存放全部特征值
   double c --- 长度为n，前n－1个元素存放n阶对称三对角阵的次对角
                线上的元素
   double q --- 维数为nxn，若存放单位矩阵，则返回n阶实对称三对角
                阵T的特征向量组；若存放Householder_Tri_Symetry_Diagonal()
				函数所返回的一般实对称矩阵A的豪斯荷尔得变换的乘
				积矩阵Q，则返回实对称矩阵A的特征向量组。其中，q中
				的的j列为与数组b中第j个特征值对应的特征向量
   double eps --- 本函数在迭代过程中的控制精度要求
   int l    --- 为求得一个特征值所允许的最大迭代次数
   返回值：
   若返回标记小于0，则说明迭代了l次还未求得一个特征值，并有fail
   信息输出；若返回标记大于0，则说明程序工作正常，全部特征值由一
   维数组b给出，特征向量组由二维数组q给出
*/
int Tri_Symmetry_Diagonal_Eigenvector( int n, double b[], double c[],
                                       double q[], double eps, int l )
{
	int i, j, k, m, it, u, v;
	double d, f, h, g, p, r, e, s;

	c[n-1] = 0.0;
	d = 0.0;
	f = 0.0;
	for ( j = 0; j <= n-1; j++ ) {
		it = 0;
		h = eps * ( fabs( b[j] ) + fabs( c[j] ) );
		if ( h > d ) d = h;
		m = j;
		while ( ( m <= n-1 ) && ( fabs( c[m] ) > d ) ) m = m+1;
		if ( m != j ) {
			do {
				if ( it == l ) {
#ifdef ALGO_DEBUG
					printf( "fail\n" );
#endif
					return( -1 );
				}
				it = it + 1;
				g = b[j];
				p = ( b[j+1] - g ) / ( 2.0 * c[j] );
				r = sqrt( p * p + 1.0 );
				if ( p >= 0.0 )
					b[j] = c[j] / ( p + r );
				else
					b[j] = c[j] / ( p - r );
				h = g - b[j];
				for ( i = j+1; i <= n-1; i++ )
					b[i] = b[i] - h;
				f = f + h;
				p = b[m];
				e = 1.0;
				s = 0.0;
				for ( i = m-1; i >= j; i-- ) {
					g = e * c[i];
					h = e * p;
					if ( fabs( p ) >= fabs( c[i] ) ) {
						e = c[i] / p;
						r = sqrt( e * e + 1.0 );
						c[i+1] = s * p * r;
						s = e / r;
						e = 1.0 / r;
					} else {
						e = p / c[i];
						r = sqrt( e * e + 1.0 );
						c[i+1] = s * c[i] * r;
						s = 1.0 / r;
						e = e / r;
					}
					p = e * b[i] - s * g;
					b[i+1] = h + s * ( e * g + s * b[i] );
					for ( k = 0; k <= n-1; k++ ) {
						u = k * n + i + 1;
						v = u - 1;
						h = q[u];
						q[u] = s * q[v] + e * h;
						q[v] = e * q[v] - s * h;
					}
				}
				c[j] = s * p;
				b[j] = e * p;
			} while ( fabs( c[j] ) > d );
		}
		b[j] = b[j] + f;
	}
	for ( i = 0; i <= n-1; i++ ) {
		k = i;
		p = b[i];
		if ( i+1 <= n-1 ) {
			j = i+1;
			while ( ( j <= n-1 ) && ( b[j] <= p ) ) {
				k = j;
				p = b[j];
				j = j+1;
			}
		}
		if ( k != i ) {
			b[k] = b[i];
			b[i] = p;
			for ( j = 0; j <= n-1; j++ ) {
				u = j * n + i;
				v = j * n + k;
				p = q[u];
				q[u] = q[v];
				q[v] = p;
			}
		}
	}

	return( 1 );
}

# define EPS    0.000001   /* 计算精度 */
# define Iteration    60   /* 求取特征量的最多迭代次数 */

/*
   计算实对称阵的全部特征值与对应特征向量
   int n              --- 实对称阵的阶数
   double * CovMatrix --- 维数为nxn，存放n阶对称阵的各元素；
   double * Eigen     --- 长度为n，为n个特征值
   double * EigenVector --- 维数为nxn，返回n阶实对称阵的特征向量组其中，
                            EigenVector中的j列为与数组Eigen中第j个特征值
							对应的特征向量
   返回值：
   若返回标记小于0，则说明迭代了Iteration次还未求得一个特征值；
   若返回标记大于0，则说明程序工作正常，全部特征值由一
   维数组Eigen给出，特征向量组由二维数组EigenVector给出
*/
int SymmetricRealMatrix_Eigen( double CovMatrix[],
                               int n,
                               double Eigen[],
                               double EigenVector[] )
{
	int k;
	double * subDiagonal;

	subDiagonal = ( double * )malloc( sizeof( double ) * n );
	Householder_Tri_Symetry_Diagonal( CovMatrix, n, EigenVector, Eigen, subDiagonal );
	k = Tri_Symmetry_Diagonal_Eigenvector( n, Eigen, subDiagonal, EigenVector, EPS, Iteration );
	free( subDiagonal );

	return( k );
}

/*
  PCA1:
  data     --- row*col
			   行数为原始数据维数；每列数据为一个样本
  signals  --- row*col
			   最后求得的降维矩阵
			   但实际有用的部分只是
			   need*col
  PC       --- row*row 特征向量
  V        --- row*col 特征值
			   不要开成 1*row
			   因为那求特征值的算法会神奇的报错......

  need     --- 要求的主成份分析数目
			   不填的话默认为维数
*/

/*
	数据要一列一列进
	注意行列是反的...因为这样操纵列向量比较方便

	例：
		2.5000    2.4000
		0.5000    0.7000
		2.2000    2.9000
		1.9000    2.2000
		3.1000    3.0000
		2.3000    2.7000
		2.0000    1.6000
		1.0000    1.1000
		1.5000    1.6000
		1.1000    0.9000

	double data[20] = {
		2.5, 0.5, 2.2, 1.9, 3.1, 2.3, 2.0, 1.0, 1.5, 1.1,
		2.4, 0.7, 2.9, 2.2, 3.0, 2.7, 1.6, 1.1, 1.6, 0.9
	};
	int row = 2, col = 10;

*/

void pca(double * data, int row, int col,
         double * signals, double * PC, double * V,
         int need = 0)
{
	if (!need) need = row;

	int x, y, k;
	double * mean;
	double * data1, * tPC, * tV;
	double * covariance, temp;
	int rvalue, * no, tp;

	if (row <= 1 || col <= 1) return;
	/* subtract off the mean for each dimension */
	mean = ( double * )malloc( sizeof( double ) * row );
	data1 = ( double * )malloc( sizeof( double ) * row * col );
	for ( y = 0; y < row; y++ ) { /* calculate mean */
		mean[y] = 0;
		for ( x = 0; x < col; x++ )
			mean[y] += data[y*col+x];
	}
	for ( y = 0; y < row; y++ ) mean[y] = mean[y]/col;
	for ( y = 0; y < row; y++ ) /* subtract mean */
		for ( x = 0; x < col; x++ ) {
			data1[y*col+x] = data[y*col+x] - mean[y];
		}
	free(mean);
	/* calculate the covariance matrix */
	__int64 memory = row;
	memory *= memory;
	memory *= sizeof(double);
	covariance = ( double * )malloc( memory);
	for ( y = 0; y < row; y++ )
		for ( x = 0; x < row; x++ ) {
			temp = 0;
			for ( k = 0; k < col; k++ )
				temp += data1[y*col+k] * data1[x*col+k];
			temp = temp / ( col-1 );
			covariance[x*row+y] = temp;
		}
	/* find the eigenvectors and eigenvalues */
	rvalue = SymmetricRealMatrix_Eigen( covariance, row, V, PC );
	free( covariance );
	/* sort the variances in decreasing order */
	no = ( int * )malloc( sizeof( int ) * row );
	for ( x = 0; x < row; x++ ) no[x] = x;
	for ( x = 0; x < row-1; x++ ) {
		temp = V[x];
		for ( k = x; k < row; k++ )
			if ( temp < V[k] ) {
				tp = no[k];
				no[k] = no[x];
				no[x] = tp;
				temp = V[k];
			}
	}
	/* exchange eigen value and vector in decreasing order */
	tV = ( double * )malloc( sizeof( double ) * row );
	tPC = ( double * )malloc( sizeof( double ) * row * row );
	for ( x = 0; x < row; x++ ) tV[x] = V[x];
	for ( x = 0; x < row; x++ )
		for ( y = 0; y < row; y++ )
			tPC[x*row+y] = PC[x*row+y];
	for ( x = 0; x < row; x++ ) {
		if ( no[x] != x ) {
			for ( y = 0; y < row; y++ )
				PC[y*row+x] = tPC[y*row+no[x]];
			V[x] = tV[no[x]];
		}
	}
	free( no );
	free( tV );
	free( tPC );

	/* project the original data: signals = PC' * data; */
	// PC eig Vector row*row
	// data adjusted data col*row


	for ( y = 0; y < need; y++ )
		for ( x = 0; x < col; x++ ) {
			signals[y*col+x] = 0;
			for ( k = 0; k < row; k++ )
				signals[y*col+x] += PC[k*need+y] * data1[k*col+x];
		}



	free( data1 );
}

// 使用样例
/*
int main()
{
	double data[20] = {
		2, 3, 8, 2, 3,
		7, 9, 29, 3, 5,
		3, 8, 22, 12, 1,
		3, 12, 12, 33, 2};
	int row = 4, col = 5;
	double signals[20], PC[16], V[4];
	int x, y;

	pca( data, row, col, signals, PC, V );

	printf( "Project to Principal Component: \n" );
	for ( y = 0; y < row; y++ )
	{
		for ( x = 0; x < col; x++ )
		{
			printf( "%7.4f ", signals[y*col+x] );
		}
		printf( "\n" );
	}
	printf( "Eigen Values (in deceasing order): \n" );
	for ( y = 0; y < row; y++ )
		printf( "%7.4f ", V[y] );
	printf( "\n" );
	printf( "Eigen Vectors: \n" );
	for ( y = 0; y < row; y++ )
	{
		for ( x = 0; x < row; x++ )
		{
			printf( "%7.4f ", PC[y*row+x] );
		}
		printf( "\n" );
	}

	return( 0 );
}
*/