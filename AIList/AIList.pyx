#cython: embedsignature=True
#cython: profile=False

import os
import numpy as np
cimport numpy as np
cimport cython
import pandas as pd


def get_include():
	"""
	Get file directory if C headers
	"""

	return os.path.split(os.path.realpath(__file__))[0]


cdef class Interval(object):
	"""
	Wrapper of C interval_t
	"""

	# Set the interval
	cdef void set_i(Interval self, interval_t i):
		"""
		Initialize wrapper of C interval
		"""

		self.i = i


	@property
	def start(self):
		return self.i.start

	@property
	def end(self):
		return self.i.end

	@property
	def value(self):
		return self.i.value


	def __str__(self):
		format_string = "Interval(%d-%d, %s)" % (self.start, self.end, self.value)
		return format_string


	def __repr__(self):
		format_string = "Interval(%d-%d, %s)" % (self.start, self.end, self.value)
		return format_string


cdef class AIList(object):
	"""
	Wrapper for C ailist_t
	"""

	def __cinit__(self):
		"""
		Initialize AIList object
		"""

		self.interval_list = ailist_init()
		self.is_constructed = False


	def __dealloc__(self):
		"""
		Free IntervalSkipList
		"""

		if hasattr(self, 'interval_list'):
			ailist_destroy(self.interval_list)


	@property	
	def size(self):
		return self.interval_list.nr
	
	@property
	def first(self):
		return self.interval_list.first

	@property
	def last(self):
		return self.interval_list.last

	@property
	def range(self):
		return self.last - self.first


	def __len__(self):
		return self.size

	
	def __iter__(self):
		cdef Interval interval
		for i in range(self.size):
			interval = Interval()
			interval.set_i(self.interval_list.interval_list[i])
			yield interval


	cdef void set_list(AIList self, ailist_t *input_list):
		"""
		Set wrapper of C ailist
		"""
		# Free old skiplist
		if hasattr(self, 'interval_list'):
			ailist_destroy(self.interval_list)
		# Replace new skiplist
		self.interval_list = input_list


	cdef void _insert(AIList self, int start, int end):
		ailist_add(self.interval_list, start, end, self.interval_list.nr)

	def add(self, int start, int end):
		"""
		"""
		self._insert(start, end)
		self.is_constructed = False


	def from_array(self, const long[::1] starts, const long[::1] ends, const long[::1] index):
		cdef int array_length = len(starts)
		ailist_from_array(self.interval_list, &starts[0], &ends[0], &index[0], array_length)


	cdef void _construct(AIList self, int min_length):
		ailist_construct(self.interval_list, min_length)

	def construct(self, int min_length=20):
		"""
		"""
		self._construct(min_length)
		self.is_constructed = True


	cdef ailist_t *_intersect(AIList self, int start, int end):
		cdef ailist_t *overlaps = ailist_query(self.interval_list, start, end)

		return overlaps

	def intersect(self, int start, int end):
		"""
		"""
		if self.is_constructed == False:
			self.construct()

		cdef ailist_t *i_list = self._intersect(start, end)
		cdef AIList overlaps = AIList()
		overlaps.set_list(i_list)

		return overlaps
		

	cdef np.ndarray _coverage(AIList self):
		# Initialize coverage
		cdef double[::1] coverage = np.zeros(self.range, dtype=np.double)

		ailist_coverage(self.interval_list, &coverage[0])

		return np.asarray(coverage)

	def coverage(self):
		"""
		"""
		if self.is_constructed == False:
			self.construct()
		
		# Initialize coverage
		cdef np.ndarray coverage
		# Calculate coverage
		coverage = self._coverage()
		
		return pd.Series(coverage, index=np.arange(self.first, self.last))


	def display(self):
		"""
		"""
		display_list(self.interval_list)


	def merge(self, int gap=0):
		"""
		"""
		cdef AIList merged_list = AIList()
		cdef ailist_t *merged_clist = ailist_merge(self.interval_list, gap)

		merged_list.set_list(merged_clist)

		return merged_list


	cdef np.ndarray _wps(AIList self, int protection):
		# Initialize wps
		cdef double[::1] wps = np.zeros(self.range, dtype=np.double)

		ailist_wps(self.interval_list, &wps[0], protection)

		return np.asarray(wps)

	def wps(self, int protection=60):
		"""
		"""
		if self.is_constructed == False:
			self.construct()
		
		# Initialize wps
		cdef np.ndarray wps
		# Calculate wps
		wps = self._wps(protection)
		
		return pd.Series(wps, index=np.arange(self.first, self.last))

	
	def filter(self, int min_length=1, int max_length=400):
		"""
		"""
		# Initialize filtered list
		cdef AIList filtered_ail = AIList()

		cdef ailist_t *cfiltered_ail = ailist_length_filter(self.interval_list, min_length, max_length)
		filtered_ail.set_list(cfiltered_ail)

		return filtered_ail


	cdef np.ndarray _length_dist(AIList self):
		# Initialize distribution
		cdef int max_length = ailist_max_length(self.interval_list)
		cdef int[::1] distribution = np.zeros(max_length + 1, dtype=np.intc)

		# Calculate distribution
		ailist_length_distribution(self.interval_list, &distribution[0])

		return np.asarray(distribution, dtype=np.intc)

	def length_dist(self):
		# Initialize distribution
		cdef np.ndarray distribution
		# Calculate distribution
		distribution = self._length_dist()

		return distribution


	cdef np.ndarray _nhits_from_array(AIList self, const long[::1] starts, const long[::1] ends):
		# Initialize hits
		cdef int length = starts.size
		cdef int[::1] nhits = np.zeros(length, dtype=np.intc)

		# Calculate distribution
		ailist_nhits_from_array(self.interval_list, &starts[0], &ends[0], length, &nhits[0])

		return np.asarray(nhits, dtype=np.intc)

	def nhits_from_array(self, const long[::1] starts, const long[::1] ends):
		# Initialize distribution
		cdef np.ndarray nhits
		# Calculate distribution
		nhits = self._nhits_from_array(starts, ends)

		return nhits