# Augmented Interval List

[![Build Status](https://travis-ci.org/kylessmith/AIList.svg?branch=master)](https://travis-ci.org/kylessmith/AIList) [![PyPI version](https://badge.fury.io/py/AIList.svg)](https://badge.fury.io/py/AIList)

Augmented interval list (AIList) is a data structure for enumerating intersections 
between a query interval and an interval set. AILists have previously been showed 
to be is several times faster than [interval tree][ITree], [NCList][NCList] 
and BEDTools, see [ailist_paper][ailist_paper].

This implementation is a Python wrapper of the one used in the original AIList library.
Original: https://github.com/databio/AIList

Additonal wrapper functions have been created which allow easy user interface.

Paper: https://academic.oup.com/bioinformatics/advance-article/doi/10.1093/bioinformatics/btz407/5509521

## Install

If you dont already have numpy and scipy installed, it is best to download
`Anaconda`, a python distribution that has them included.  
```
    https://continuum.io/downloads
```

Dependencies can be installed by:

```
    pip install -r requirements.txt
```

PyPI install, presuming you have all its requirements installed:
```
    pip install AIList
```

## Usage

```
	>>> from AIList import AIList
	>>> import numpy as np
	>>>
	>>> i = AIList()
	>>> i.add(15, 20)
	>>> i.add(10, 30)
	>>> i.add(17, 19)
	>>> i.add(5, 20)
	>>> i.add(12, 15)
	>>> i.add(30, 40)
	>>>
	# Print intervals
	>>> i.display()
	(15-20) (10-30) (17-19) (5-20) (12-15) (30-40)
	>>>
	# Find overlapping intervals
	>>> o = i.intersect(6, 7)
	>>> o.display()
	(5-20)
	>>>
	# Now i has been constructed/sorted
	>>> i.display()
	(5-20) (10-30) (12-15) (15-20) (17-19) (30-40)
	>>>
	# Can be done manually as well at any time
	>>> i.construct()
	>>>
	# Iterate over intervals
	>>> for x in i:
			print(x)
	Interval(5-20, 3)
	Interval(10-30, 1)
	Interval(12-15, 4)
	Interval(15-20, 0)
	Interval(17-19, 2)
	Interval(30-40, 5)
	>>>
	# AIList can also add to from arrays
	>>> starts = np.arange(10,1000,100)
	>>> ends = starts + 50
	>>> ids = starts
	>>> i.from_array(starts, ends, ids)
	>>> i.display()
	(5-20) (10-30) (12-15) (15-20) (17-19) (30-40) 
	(10-60) (110-160) (210-260) (310-360) (410-460) 
	(510-560) (610-660) (710-760) (810-860) (910-960)
	>>>
	# Merge overlapping intervals
	>>> m = i.merge(gap=10)
	>>> m.display()
	(5-60) (110-160) (210-260) (310-360) (410-460) 
	(510-560) (610-660) (710-760) (810-860) (910-960)
	>>>
	# Find array of coverage
	>>> c = i.coverage()
	>>>
	# Calculate window protection score
	>>> w = i.wps(5)
	>>>
	# Filter to interval lengths between 3 and 20
	>>> fi = i.filter(3,20)
	>>>

```


## Original paper

> Jianglin Feng,  Aakrosh Ratan,  Nathan C Sheffield; Augmented Interval List: a novel data structure for efficient genomic interval search, Bioinformatics, btz407, https://doi.org/10.1093/bioinformatics/btz407
