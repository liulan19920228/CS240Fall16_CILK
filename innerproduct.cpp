#include <cilk/cilk.h>
#include <cilk/reducer_opadd.h>

#include <algorithm>
#include <numeric>
#include <string>
#include <iostream>
#include <cmath>
#include <iterator>
#include <functional>
//#include <stdio.h>
#include "example_util_gettime.h"

#define COARSENESS 100
#define ITERS 10


double rec_cilkified(double *a, double *b, int n)
{
    double sum1,sum2;
    if(n <= COARSENESS)
        return std::inner_product(a,a+n,b,double(0));
    else{
        sum1=cilk_spawn rec_cilkified(a,b,int(n/2));
        sum2=rec_cilkified(a+int(n/2),b+int(n/2),n-int(n/2));
        cilk_sync;
        return sum1+sum2;
   }
   // return 0;
}

double loop_cilkified(double *a, double *b, int n)
{
    int i,j;
    double *sum=new double[n/COARSENESS];
    double result=0;
    
    cilk_for(i = 0; i< n/COARSENESS; i++)
    {
        sum[i]=0;
        for(j=0; j< COARSENESS; j++)
            sum[i]+=a[i*COARSENESS+j]*b[i*COARSENESS+j];
    }
    
    for(i = 0; i< n/COARSENESS; i++)
        result+=sum[i];
    if(n%COARSENESS != 0)
    {
        for(i=0; i<n%COARSENESS; i++)
        {
            result+=a[n-i-1]*b[n-i-1];
        }
    }
    delete [] sum;
    return result;
    return 0;
}

double hyperobject_cilkified(double *a, double *b, int n)
{
    int i;
    cilk::reducer_opadd<double> result;
    cilk_for(i=0; i<n; i++)
    {
        result +=a[i]*b[i];
    }
    
    return result.get_value();
    return 0;
}

int close(double x, double y, int n)
{
        double relative_diff = (x>y? (x-y)/x : (y-x)/x);
       return (relative_diff < sqrt((double) n) * exp2(-42))? 1 : 0;
}

//A simple test harness
int inn_prod_driver(int n)
{
    double * a = new double[n];
    double * b = new double[n];
    for (int i = 0; i < n; ++i)
    {
        a[i] = i;
        b[i] = i;
    }
    std::random_shuffle(a, a + n);
    std::random_shuffle(b, b + n);
    
    double seqresult = std::inner_product(a, a+n, b,(double)0);
   // printf("seqresult is %f",seqresult);
    //double c=0;
   // for (int i=0; i<n; ++i)
   // c+=a[i]*b[i];

   // printf("true result is %f",c); 
    long t1 = example_get_time();
    for(int i=0; i< ITERS; ++i)
    {
        seqresult = std::inner_product(a, a+n, b, double(0));
    }
    long t2 = example_get_time();
    
    double seqtime = (t2-t1)/(ITERS*1000.f);
    std::cout << "Sequential time: " << seqtime << " seconds" << std::endl;

	/***********************************************************/
	/********  START TESTING RECURSIVE CILKFIED VERSION  *******/
	/***********************************************************/


    double parresult = rec_cilkified(a, b, n);
    t1 = example_get_time();
    for(int i=0; i< ITERS; ++i)
    {
        parresult = rec_cilkified(a, b, n);
    }
    t2 = example_get_time();
    
    double partime = (t2-t1)/(ITERS*1000.f);
    std::cout << "Recursive cilkified time:" << partime << " seconds" << std::endl;
    std::cout << "Speedup is: " << seqtime/partime << std::endl;
    std::cout << "Sequential result is: "<<seqresult<<std::endl;
    std::cout << "Recursive cilkified result is: "<<parresult<<std::endl;
    std::cout << "Result is " << (close(seqresult,parresult,n)  ? "correct":"incorrect") << std::endl;
    
        /****************************************************************/
        /********  START TESTING NESTED LOOPED CILKIFIED VERSION  *******/
        /****************************************************************/


    parresult = loop_cilkified(a, b, n);
    
    t1 = example_get_time();
    for(int i=0; i< ITERS; ++i)
    {
        parresult = loop_cilkified(a, b, n);
    }
    t2 = example_get_time();
    
    
    partime = (t2-t1)/(ITERS*1000.f);
    std::cout << "Nested loop cilkified time: " << partime << " seconds" << std::endl;
    std::cout << "Speedup is: " << seqtime/partime << std::endl;
    std::cout << "Sequential result is: "<<seqresult<<std::endl;
    std::cout << "Loop cilkified result is: "<<parresult<<std::endl;
    std::cout << "Result is " << (close(seqresult,parresult,n)  ? "correct":"incorrect") << std::endl;
    
        /**************************************************************/
        /********  START TESTING HYPEROBJECT CILKIFIED VERSION  *******/
        /**************************************************************/

 

   parresult = hyperobject_cilkified(a, b, n);
    
    t1 = example_get_time();
    for(int i=0; i< ITERS; ++i)
    {
        parresult = hyperobject_cilkified(a, b, n);
    }
    t2 = example_get_time();
    
    partime = (t2-t1)/(ITERS*1000.f);
    std::cout << "Hyperobject cilkified time:" << partime << " seconds" << std::endl;
    std::cout << "Speedup is: " << seqtime/partime << std::endl;
    std::cout << "Sequential result is: "<<seqresult<<std::endl;
    std::cout << "Hyperobject result is: "<<parresult<<std::endl;
    std::cout << "Result is " << (close(seqresult,parresult,n)  ? "correct":"incorrect") << std::endl;
    
    
    delete [] a;
    delete [] b;
    return 0;
}

int main(int argc, char* argv[])
{
    int n = 1 * 1000 * 1000;
    if (argc > 1) {
        n = std::atoi(argv[1]);
    }
    
    return inn_prod_driver(n);
}
