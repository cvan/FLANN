#!/usr/bin/env python


import tables
import numpy
import sys


def load_data(dataset_name, testset_name, match_name):
    dataset = numpy.loadtxt(dataset_name, dtype=numpy.uint8)
    query = numpy.loadtxt(testset_name, dtype=numpy.uint8)
    match = numpy.loadtxt(match_name, dtype=numpy.int32)

    return dataset, query, match


def create_file(dataset_name, testset_name, match_name, name):
    h5file = tables.openFile(name, mode="w", title="Test data")
    group = h5file.root

    dataset, query, match = load_data(dataset_name, testset_name, match_name)
    print 'Dataset: ', dataset.shape, dataset.dtype
    print 'Query: ', query.shape, query.dtype
    print 'Match: ', match.shape, match.dtype
    h5file.createArray(group, 'dataset', dataset)
    h5file.createArray(group, 'query', query)
    h5file.createArray(group, 'match', match)
    h5file.close()
    

def usage():
    print "Usage %s: dataset query match name"%sys.argv[0]
    sys.exit(1)

def main():
    if len(sys.argv)!=5:
        usage()
    create_file(sys.argv[1],sys.argv[2],sys.argv[3], sys.argv[4])


if __name__=="__main__":
    main()
