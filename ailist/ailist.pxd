import numpy as np
cimport numpy as np
cimport cython
from libc.stdint cimport uint32_t, int32_t, int64_t
from libc.stdlib cimport malloc, free


cdef extern from "augmented_interval_list.c":
	# C is include here so that it doesn't need to be compiled externally
	pass

cdef extern from "augmented_interval_list.h":

	# C interval struct
	ctypedef struct interval_t:
		uint32_t start      			    # Region start: 0-based
		uint32_t end    					# Region end: not inclusive
		int32_t index
		double value

	ctypedef struct ailist_t:
		int64_t nr, mr  					# Number of regions
		interval_t *interval_list			# Regions data
		uint32_t first, last				# Record range of intervals


	# Initialize ailist_t
	ailist_t *ailist_init() nogil
	# Add a interval_t interval
	void ailist_add(ailist_t *ail, uint32_t start, uint32_t end, int32_t index, double value) nogil
	# Sort intervals in ailist
	void ailist_sort(ailist_t *ail) nogil
	# Construct ailist: decomposition and augmentation
	void ailist_construct(ailist_t *ail, int cLen) nogil
	# Query ailist intervals
	ailist_t *ailist_query(ailist_t *ail, uint32_t qs, uint32_t qe) nogil
	# Query ailist intervals within lengths
	ailist_t *ailist_query_length(ailist_t *ail, uint32_t qs, uint32_t qe, int min_length, int max_length) nogil
	# Free ailist data
	void ailist_destroy(ailist_t *ail) nogil
	# Calculate coverage
	void ailist_coverage(ailist_t *ail, double coverage[]) nogil
	# Calculate n hits within bins
	void ailist_bin_nhits(ailist_t *ail, double coverage[], int bin_size) nogil
	# Add intervals from arrays
	void ailist_from_array(ailist_t *ail, long starts[], long ends[], long index[], double values[], int length) nogil
	# Merge overlapping intervals
	ailist_t *ailist_merge(ailist_t *ail, uint32_t gap) nogil
	# Calculate Window Protection Score
	void ailist_wps(ailist_t *ail, double wps[], uint32_t protection) nogil
	# Filter ailist by length
	ailist_t *ailist_length_filter(ailist_t *ail, int min_length, int max_length) nogil
	#  Calculate length distribution
	void ailist_length_distribution(ailist_t *ail, int distribution[]) nogil
	# Calculate maximum length
	int ailist_max_length(ailist_t *ail) nogil
	# Calculate number of overlaps from arrays
	void ailist_nhits_from_array(ailist_t *ail, const long starts[], const long ends[], int length, int nhits[]) nogil
	# Calculate number of overlaps from arrays within lengths
	void ailist_nhits_from_array_length(ailist_t *ail, const long starts[], const long ends[], int length, int nhits[], int min_length, int max_length) nogil
	# Print AIList
	void display_list(ailist_t *ail) nogil


cpdef object rebuild(bytes data, bytes b_length, bytes b_first, bytes b_last)


cdef class Interval(object):
	"""
	Wrapper for C interval
	"""
	cdef interval_t i

	cdef void set_i(self, interval_t i)


cdef class AIList(object):
	"""
	Wrapper for C ailist_t
	"""
	cdef ailist_t *interval_list
	cdef public bint is_constructed
	cdef public bint is_sorted

	cdef bytes _get_data(self)
	cdef ailist_t *_set_data(self, bytes data, bytes b_length, bytes b_first, bytes b_last)

	cdef void set_list(AIList self, ailist_t *input_list)
	cdef void _insert(AIList self, int start, int end, double value)
	cdef void _construct(AIList self, int min_length)
	cdef void _sort(AIList self)
	cdef ailist_t *_intersect(AIList self, int start, int end)
	cdef np.ndarray _coverage(AIList self)
	cdef np.ndarray _bin_nhits(AIList self, int bin_size)
	cdef np.ndarray _wps(AIList self, int protection)
	cdef np.ndarray _length_dist(AIList self)
	cdef np.ndarray _nhits_from_array(AIList self, const long[::1] starts, const long[::1] ends)
	cdef np.ndarray _nhits_from_array_length(AIList self, const long[::1] starts, const long[::1] ends, int min_length, int max_length)