#!/usr/bin/python

# Now go to work
from pyflann import *
from copy import copy
from numpy import *
from numpy.random import *
import unittest


class Test_PyFLANN_nn(unittest.TestCase):

    def setUp(self):
        self.nn = FLANN()


class Test_PyFLANN_nn_index(unittest.TestCase):
    
    def testnn_index(self):

        dim = 10
        N = 100

        x   = rand(N, dim)
        print "Creating object"
        nn = FLANN()
        print "Build index"
        nn.build_index(x)
        
        print "Search with index"
        nnidx, nndist = nn.nn_index(x)
        correct = all(nnidx == arange(N, dtype = index_type))
                
        print "Delete index"
        nn.delete_index()
        self.assert_(correct)




if __name__ == '__main__':
    unittest.main()
