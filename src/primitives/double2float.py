#!/usr/bin/python3
"""
Created on Mar 13, 2013

Converts all double-s to float-s in the specified text file.

@author: Markovtsev Vadim <v.markovtsev@samsung.com>
"""

import argparse
import numpy
import re

def double2float(match):
    fn=numpy.double(match.group())  
    return '%.7ef' % (fn)
    

def main():
    parser = argparse.ArgumentParser(description='Converts all double-s to '
                                                 'float-s in the specified '
                                                 'text file.')
    parser.add_argument('--in', dest='input', required=True,
                        help='Input text file.')
    parser.add_argument('--out', dest='output', required=True,
                        help='Output text file.')
    args = parser.parse_args()
    with open(args.input, 'r') as fin:        
        with open(args.output, 'w') as fout:
            for line in fin.readlines():            
                line = re.sub('( -| )\d+\.\d+e-\d\d', double2float, line)   
                fout.write(line)
                
    
if __name__ == '__main__':
    main()