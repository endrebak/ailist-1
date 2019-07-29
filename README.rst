Python implementation of an Augmented Interval List


Installing
==========

To install, it's best to create an environment after installing and downloading the
`Anaconda Python Distribution <https://www.continuum.io/downloads>`__

    conda env create --file environment.yml

PyPI install, presuming you have all its requirements (numpy and scipy) installed::

	pip install AIList

	
Importation
===========
::

	>>> from AIList import AIList
	>>> i = AIList()
	>>> i.add(15, 20)
	>>> i.add(10, 30)
	>>> i.add(17, 19)
	>>> i.add(5, 20)
	>>> i.add(12, 15)
	>>> i.add(30, 40)
	# Print intervals
	>>> i.display()
	# Iterate over intervals
	>>> for x in i:
			print(x)
	# Find overlapping intervals
	>>> o = i.intersect(6, 7)
	# Find array of coverage
	>>> c = i.coverage()
	# Calculate window protection score
	>>> w = i.wps(5)
	# Filter to interval lengths between 3 and 20
	>>> fi = i.filter(3,20)
	# Merge overlapping intervals
	>>> m = i.merge()


Installation
============

If you dont already have numpy and scipy installed, it is best to download
`Anaconda`, a python distribution that has them included.  

    https://continuum.io/downloads

Dependencies can be installed by::

    pip install -r requirements.txt


License
=======

IntervalTree is available under the GPL-3 License
