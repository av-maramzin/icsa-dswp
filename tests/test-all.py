#! /usr/bin/python
# Only Linux is supported.
from os import listdir, chdir, remove
from os.path import isfile, join, dirname, abspath, splitext
from subprocess import Popen, PIPE

def issource(filename):
  ext = splitext(filename)[1]
  return ext == '.cpp'

def listsrc():
  srcs = [ f for f in listdir('.') if isfile(f) and issource(f) ]
  assert(1 == len(srcs))
  return srcs[0]

def genbytecode(src):
  cmd = 'clang++ -c -emit-llvm ' + src
  p = Popen(cmd.split(' '), stdout=PIPE, stderr=PIPE)
  output, errors = p.communicate()
  return splitext(src)[0] + '.bc'

def optimize(bc):
  base, ext = splitext(bc)
  obc = base + '-dswp' + ext
  cmd = 'opt -load ../../build/pass/libdswp.so -icsa-dswp ' + bc + ' -o ' + obc
  p = Popen(cmd.split(' '), stdout=PIPE, stderr=PIPE)
  output, errors = p.communicate()
  return obc

def genexec(bc):
  execname = './' + splitext(bc)[0] + '.out'
  cmd = 'clang++ ' + bc + ' -o ' + execname
  p = Popen(cmd.split(' '), stdout=PIPE, stderr=PIPE)
  output, errors = p.communicate()
  return execname

def execcompare(execA, execB):
  # any order:
  # group A
  pA = Popen(execA, stdout=PIPE, stderr=PIPE)
  outA, errorsA = pA.communicate()
  # group B
  pB = Popen(execB, stdout=PIPE, stderr=PIPE)
  outB, errorsB = pB.communicate()
  # end any order
  if outA == outB:
    return True
  else:
    print '[DEBUG] Output of original program:'
    print outA
    print '[DEBUG] Output of optimized program:'
    print outB
    return False

# Run from tests dir.
chdir(dirname( abspath( __file__ ) ))

testdirs = [ d for d in listdir('.') if not isfile(d) ]

# parloop
for d in testdirs:
  chdir(d)

  src = listsrc()
  bc = genbytecode(src)
  # any order:
  # group B
  execorig = genexec(bc)
  # group A
  obc = optimize(bc)
  execopt = genexec(obc)
  # end any order
  success = execcompare(execorig, execopt)

  print 'Test', d, 'result:', "SUCCESS" if success else "FAILURE"

  for f in [src, bc, execorig, obc, execopt]:
    remove(f)

  chdir('..')
