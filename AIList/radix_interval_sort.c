#include <stdio.h>
#include "augmented_interval_list.h"


int get_max(interval_t *arr, int n) 
{   /* Get maximum start in array */
    int mx = arr[0].start;
    int i;

    // Iterate over array, record max start
    for (i = 1; i < n; i++)
    {
        if ((int)arr[i].start > mx)
        { 
            mx = arr[i].start;
        }
    }
    
    return mx; 
} 
  
 
void count_sort(interval_t *arr, int n, int exp) 
{   /* Counting sort array by exp */
    interval_t *output = (interval_t *)malloc(n * sizeof(interval_t));; // output array 
    int i, count[10] = {0}; 
  
    // Store count of occurrences in count
    for (i = 0; i < n; i++)
    {
        count[ (arr[i].start / exp) % 10 ]++;
    }
  
    // Change count[i] so that count[i] now contains actual 
    // position of this digit in output
    for (i = 1; i < 10; i++)
    {
        count[i] += count[i - 1];
    }
  
    // Build output
    for (i = n - 1; i >= 0; i--) 
    { 
        output[count[ (arr[i].start / exp) % 10 ] - 1] = arr[i]; 
        count[ (arr[i].start / exp) % 10 ]--; 
    } 
  
    // Copy the output array to arr[] 
    for (i = 0; i < n; i++)
    {
        arr[i] = output[i];
    }
} 
  

void radix_interval_sort(interval_t *arr, int n) 
{   /* Radix Sort  */
    // Find the maximum start
    int m = get_max(arr, n);
  
    // Do counting sort for every digit
    int exp;
    for (exp = 1; m/exp > 0; exp *= 10)
    {
        count_sort(arr, n, exp);
    }
} 