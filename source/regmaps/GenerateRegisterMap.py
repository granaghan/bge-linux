from mako.template import Template
import json
import sys
import os
#import argparse
from optparse import OptionParser

#parser = argparse.ArgumentParser()
#parser.add_argument('-h','--header', help='Generate header file', dest='header', action='store_true')

#args = parser.parse_args()
parser = OptionParser()
parser.add_option("--header", action="store_true", dest="header", default=False)
parser.add_option("--include", action="append", dest="includes", default=list())
parser.add_option("--read_func", action="store", default="read")
parser.add_option("--write_func", action="store", default="write")
(options, args) = parser.parse_args()

data = json.loads(sys.stdin.read())

file_dir = os.path.dirname(os.path.realpath(__file__))

mytemplate = Template(filename=file_dir+"/CRegisterMap.mako")
print(mytemplate.render(regs=data, options=options))

