//=============================================================================
// Quick and efficient storing/querying of intervals 
// by Kyle S. Smith and Jianglin Feng
//
//-----------------------------------------------------------------------------
#include "augmented_interval_list.h"
#include "radix_interval_sort.c"


uint32_t binary_search(interval_t* As, uint32_t idxS, uint32_t idxE, uint32_t qe)
{   /* Find tE: index of the first item satisfying .s<qe from right */
    
    int tL = idxS;
    int tR = idxE - 1;
    int tM;
    int tE = -1;
    
    if(As[tR].start < qe)
    {
        return tR;
    } else if (As[tL].start >= qe) {
        return -1;
    }

    while (tL < tR - 1)
    {
        tM = (tL + tR) / 2; 

        if (As[tM].start >= qe)
        {
            tR = tM - 1;
        } else {
            tL = tM;
        }
    }

    if (As[tR].start < qe)
    {
        tE = tR;
    } else if (As[tL].start < qe) {
        tE = tL;
    }

    return tE; 
}


ailist_t *ailist_init(void)
{   /* Initialize ailist_t object */

    // Initialize variables
    ailist_t *ail = (ailist_t *)malloc(sizeof(ailist_t));
    ail->nr = 0;
    ail->mr = 64;
    ail->first = INT32_MAX;
    ail->last = 0;

    // Initialize arrays
    ail->interval_list = malloc(ail->mr * sizeof(interval_t));

    // Check if memory was allocated
    if (ail == NULL && ail->interval_list == NULL)
    {
        fprintf (stderr, "Out of memory!!! (init)\n");
        exit(1);
    }

	return ail;
}


void ailist_destroy(ailist_t *ail)
{   /* Free ailist_t object */

	if (ail == 0) {return;}

	free(ail->interval_list);
	free(ail->maxE);

	free(ail);
}


void ailist_add(ailist_t *ail, uint32_t s, uint32_t e, int32_t v)
{   /* Add interval to ailist_t object */

	if (s > e) {return;}

    // Update first and last
    ail->first = MIN(ail->first, s);
    ail->last = MAX(ail->last, e);

    // If max region reached, expand array
	if (ail->nr == ail->mr)
		EXPAND(ail->interval_list, ail->mr);

    // Set new interval values
	interval_t *i = &ail->interval_list[ail->nr++];
	i->start = s;
	i->end   = e;
    i->value = v;

	return;
}

//-------------------------------------------------------------------------------

void ailist_construct(ailist_t *ail, int cLen)
{   /* Construct ailist_t object */  

    int cLen1 = cLen / 2;
    int j1, nr;
    int minL = MAX(64, cLen);     
    cLen += cLen1;      
    int lenT, len, iter, j, k, k0, t;  

    //1. Decomposition
    interval_t *L1 = ail->interval_list;					//L1: to be rebuilt
    nr = ail->nr;
    radix_interval_sort(L1, nr);

    if (nr <= minL)
    {        
        ail->nc = 1;
        ail->lenC[0] = nr;
        ail->idxC[0] = 0;                
    } else {         
        interval_t *L0 = malloc(nr * sizeof(interval_t)); 	//L0: serve as input list
        interval_t *L2 = malloc(nr * sizeof(interval_t));   //L2: extracted list 
        memcpy(L0, L1, nr * sizeof(interval_t));			
        iter = 0;
        k = 0;
        k0 = 0;
        lenT = nr;

        while (iter < MAXC && lenT > minL)
        {   
            len = 0;            
            for (t = 0; t < lenT - cLen; t++)
            {
                uint32_t tt = L0[t].end;
                j=1;
                j1=1;

                while (j < cLen && j1 < cLen1)
                {
                    if (L0[j + t].end >= tt) {j1++;}
                    j++;
                }
                
                if (j1 < cLen1)
                {
                    memcpy(&L2[len++], &L0[t], sizeof(interval_t));
                } else {
                    memcpy(&L1[k++], &L0[t], sizeof(interval_t));
                }               
            } 

            memcpy(&L1[k], &L0[lenT - cLen], cLen * sizeof(interval_t));   
            k += cLen;
            lenT = len;               
            ail->idxC[iter] = k0;
            ail->lenC[iter] = k - k0;
            k0 = k;
            iter++;

            if (lenT <= minL || iter == MAXC - 2)
            {	//exit: add L2 to the end
                if (lenT > 0)
                {
                    memcpy(&L1[k], L2, lenT * sizeof(interval_t));
                    ail->idxC[iter] = k;
                    ail->lenC[iter] = lenT;
                    iter++;
                }
                ail->nc = iter;                   
            } else {
                memcpy(L0, L2, lenT * sizeof(interval_t));
            }
        }
        free(L2);
        free(L0);     
    }

    //2. Augmentation
    ail->maxE = malloc(nr * sizeof(uint32_t)); 
    for (j = 0; j < ail->nc; j++)
    { 
        k0 = ail->idxC[j];
        k = k0 + ail->lenC[j];
        uint32_t tt = L1[k0].end;
        ail->maxE[k0] = tt;

        for (t = k0 + 1; t < k; t++)
        {
            if (L1[t].end > tt)
            {
                tt = L1[t].end;
            }

            ail->maxE[t] = tt;  
        }             
    }
    return;
}


ailist_t *ailist_query(ailist_t *ail, uint32_t qs, uint32_t qe)
{   
    uint32_t nr = 0;
    int k;

    ailist_t *overlaps = ailist_init();

    for (k = 0; k < ail->nc; k++)
    {   // Search each component
        int32_t cs = ail->idxC[k];
        int32_t ce = cs + ail->lenC[k];			
        int32_t t;

        if (ail->lenC[k] > 15)
        {
            t = binary_search(ail->interval_list, cs, ce, qe);

            while (t >= cs && ail->maxE[t] > qs)
            {
                if (ail->interval_list[t].end > qs)
                {               	
                    ailist_add(overlaps, ail->interval_list[t].start, ail->interval_list[t].end, nr);
                }

                t--;
            }
        } 
        else {
            for (t = cs; t < ce; t++)
            {
                if (ail->interval_list[t].start < qe && ail->interval_list[t].end > qs)
                {
                    ailist_add(overlaps, ail->interval_list[t].start, ail->interval_list[t].end, nr);
                }
            }                      
        }
    }

    return overlaps;                            
}

//-------------------------------------------------------------------------------

void ailist_coverage(ailist_t *ail, double coverage[])
{
    int length;
    int n;
    int i;
    int position;
    int start = ail->interval_list[0].start;
    for (i = 0; i < ail->nr; i++)
    {
        // Add length to count (excluding last position)
        length = ail->interval_list[i].end - ail->interval_list[i].start - 1;
        for (n = 0; n < length; n++)
        {
            position = (ail->interval_list[i].start - start) + n;
            coverage[position] = coverage[position] + 1;
        }
    }

    return;
}


void ailist_from_array(ailist_t *ail, const long starts[], const long ends[], const long index[], int length)
{
    // Expand interval list to the number of given
    ail->mr = ail->nr + length;
    EXPAND(ail->interval_list, ail->mr);
    
    // Iterate over itervals and add
    int i;
    for (i = 0; i < length; i++)
    {
        ailist_add(ail, starts[i], ends[i], index[i]);
    }

    return;
}


ailist_t *ailist_merge(ailist_t *ail, uint32_t gap)
{   /* Merge intervals in constructed ailist_t object */
    int previous_end = ail->interval_list[0].end;
    int previous_start = ail->interval_list[0].start;
    int k = 0;
    int i;
    ailist_t *merged_list = ailist_init();

    // Iterate over regions
    for (i = 1; i < ail->nr; i++)
    {
        // If previous
        if (previous_end > (int)(ail->interval_list[i].start - gap))
        {
            previous_end = MAX(previous_end, (int)ail->interval_list[i].end);
        }
        else
        {
            ailist_add(merged_list, previous_start, previous_end, k);
            k++;
            previous_start = ail->interval_list[i].start;
            previous_end = ail->interval_list[i].end;
        }
    }

    // Add last interval
    ailist_add(merged_list, previous_start, previous_end, k);

    return merged_list;
}


void ailist_wps(ailist_t *ail, double wps[], uint32_t protection)
{   /* Calculate Window Protection Score */
    uint32_t half_window = protection / 2;
    int head_start;
    int head_end;
    int head_length;
    int tail_start;
    int tail_end;
    int tail_length;
    int first = (int)ail->first;

    // Iterate over regions
    int i;
    int j;
    for (i = 0; i < ail->nr; i++)
    {
        // Find regions around end points
        head_start = MAX(first, (int)(ail->interval_list[i].start - half_window));
        head_end = ail->interval_list[i].start + half_window;
        tail_start = MAX(head_end, (int)(ail->interval_list[i].end - half_window)); // if overlap, set not to overlap
        tail_end = ail->interval_list[i].end + half_window;

        // Decrement region around head
        head_length = head_end - head_start;
        for (j = (head_start - first); j < (head_length + (head_start - first)); j++)
        {
            wps[j] = wps[j] - 1;
        }

        // Decrement region around tail
        tail_length = tail_end - tail_start;
        for (j = (tail_start - first); j < (tail_length + (tail_start - first)); j++)
        {
            wps[j] = wps[j] - 1;
        }

        // If head and tail region don't overlap
        if (head_end != tail_start)
        {
            for (j = (head_end - first); j < (tail_start - first); j++)
            {
                wps[j] = wps[j] + 1;
            }
        }
    }

    return;
}


ailist_t *ailist_length_filter(ailist_t *ail, int min_length, int max_length)
{   /* Filter ailist by length */
    // Initiatize filtered ailist
    ailist_t *filtered_ail = ailist_init();

    // Iterate over intervals and filter
    int length;
    int i;
    for (i = 0; i < ail->nr; i++)
    {
        // Record length (excluding last position)
        length = ail->interval_list[i].end - ail->interval_list[i].start - 1;
        if (length >= min_length && length <= max_length)
        {
            ailist_add(filtered_ail, ail->interval_list[i].start, ail->interval_list[i].end, ail->interval_list[i].value);
        }
    }

    return filtered_ail;
}


int ailist_max_length(ailist_t *ail)
{   /* Calculate maximum length */
	
    // Iterate over intervals and record length
    int length;
    int maximum = 0;
    int i;
    for (i = 0; i < ail->nr; i++)
    {
        // Record length (excluding last position)
        length = ail->interval_list[i].end - ail->interval_list[i].start - 1;
	    maximum = MAX(maximum, length);
    }

    return maximum;
}


void ailist_length_distribution(ailist_t *ail, int distribution[])
{   /* Calculate length distribution */
    // Iterate over intervals and record length
    int length;
    int i;
    for (i = 0; i < ail->nr; i++)
    {
        // Record length (excluding last position)
        length = ail->interval_list[i].end - ail->interval_list[i].start - 1;
        distribution[length] += 1;
    }

    return;
}


void ailist_nhits_from_array(ailist_t *ail, const long starts[], const long ends[], int length, int nhits[])
{
    ailist_t *overlaps;
    int i;
    for (i = 0; i < length; i++)
    {
        overlaps = ailist_query(ail, starts[i], ends[i]);
        nhits[i] = overlaps->nr;
    }

    return;
}


void display_list(ailist_t *ail)
{
    int i;
    for (i = 0; i < ail->nr; i++)
    {
        printf("(%d-%d) ", ail->interval_list[i].start, ail->interval_list[i].end);
    }
    printf("\n");
    return;
}


/* Drier program to test above function*/
int main() 
{ 
    printf("Initializing AIList...\n");
    ailist_t *ail = ailist_init();

    printf("Adding intervals...\n");
    ailist_add(ail, 15, 20, 1); 
    ailist_add(ail, 10, 30, 2); 
    ailist_add(ail, 17, 19, 3); 
    ailist_add(ail, 5, 20, 4); 
    ailist_add(ail, 12, 15, 5); 
    ailist_add(ail, 30, 40, 6); 

    //int i;
    /* for (i = 1000; i < 1000000000; i+=100) 
    {
        ailist_add(ail, i, i+2000, 0);
    } */
    display_list(ail);
    
    printf("Constructing AIList...\n");
    ailist_construct(ail, 20);
    display_list(ail);

    // Merge intervals
    printf("Merging AIList...\n");
    ailist_t *merged_ail = ailist_merge(ail, 1);
    display_list(merged_ail);

    printf("Finding overlaps...for (10-15)\n");
    ailist_t *overlaps;

    overlaps = ailist_query(ail, 10, 15);
    display_list(overlaps);

    return 0;
}