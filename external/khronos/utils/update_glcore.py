#!/usr/bin/env python
import re
import os
import urllib2

# Create directories
if not os.path.exists('../GL'):
    os.makedirs('../GL')
if not os.path.exists('../GL'):
    os.makedirs('../GL')

# Download glcorearb.h
if not os.path.exists('../GL/glcorearb.h'):
    print 'Downloading glcorearb.h to ../GL...'
    web = urllib2.urlopen('http://www.opengl.org/registry/api/glcorearb.h')
    with open('../GL/glcorearb.h', 'wb') as f:
        f.writelines(web.readlines())
else:
    print 'Reusing glcorearb.h from ../GL...'

# Parse function names from glcorearb.h
print 'Parsing glcorearb.h header...'
procs = []
p = re.compile(r'GLAPI\s+(\w+)\s+APIENTRY\s+(\w+)\s*\(\s*(.*)\s*\);')
with open('../GL/glcorearb.h', 'r') as f:
    for line in f:
        m = p.match(line)
        if m:
            procs.append(m.group)

def proc_t(proc):
    return { 'p_r': proc(1), 'p_f': proc(2), 'p_a': re.sub(r'\s*\w+,', ',', proc(3) + ',')[:-1] }

# Generate glcorearb.def
print 'Generating glcorearb.def in ../GL...'
with open('../GL/glcorearb.def', 'wb') as f:
    for proc in procs:
        format = 'GL_PROC(%(p_r)s, %(p_f)s, %(p_a)s)\n' if proc(3) != 'void' else 'GL_PROC(%(p_r)s, %(p_f)s)\n'
        f.write(format % proc_t(proc))
