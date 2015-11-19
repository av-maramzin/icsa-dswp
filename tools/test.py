#! /usr/bin/python
# Only Linux is supported.
from os import listdir, chdir, remove, getcwd
from os.path import isfile, join, dirname, abspath, splitext
from subprocess import Popen, PIPE

def printerror(msg):
  print 'ERROR in testing infrastructure: ' + msg

def issource(filename):
  ext = splitext(filename)[1]
  return ext == '.cpp'

def genbytecode(src):
  cmd = 'clang++ -std=c++11 -c -emit-llvm ' + src
  p = Popen(cmd.split(' '), stdout=PIPE, stderr=PIPE)
  output, errors = p.communicate()
  if errors == '':
    return splitext(src)[0] + '.bc'
  else:
    printerror('bytecode gen failed with errors:\n' + errors)
    return None

def optimize(bc):
  base, ext = splitext(bc)
  obc = base + '-dswp' + ext
  cmd = 'opt -load ../build/pass/libdswp.so -icsa-dswp ' + bc + ' -o ' + obc
  p = Popen(cmd.split(' '), stdout=PIPE, stderr=PIPE)
  output, errors = p.communicate()
  if errors == '':
    return obc
  else:
    printerror('optimization failed with errors:\n' + errors)
    return None

def genexec(bc):
  execname = './' + splitext(bc)[0] + '.out'
  cmd = 'clang++ ' + bc + ' -o ' + execname
  p = Popen(cmd.split(' '), stdout=PIPE, stderr=PIPE)
  output, errors = p.communicate()
  if errors == '':
    return execname
  else:
    printerror('exec gen failed with errors:\n' + errors)
    return None

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

# Run from project's root dir.
chdir('test')

srcs = [ f for f in listdir('.') if isfile(f) and issource(f) ]

print "ICSA-DSWP pass correctness tests"
for src in srcs:
  bc       = None
  execorig = None
  obc      = None
  execopt  = None

  bc = genbytecode(src)
  if not bc is None:
    execorig = genexec(bc)
    obc = optimize(bc)
    if not obc is None:
      execopt = genexec(obc)
      if not execorig is None and not execopt is None:
        success = execcompare(execorig, execopt)
        print splitext(src)[0], '-', "SUCCESS" if success else "FAILURE"

  for f in [bc, execorig, obc, execopt]:
    if not f is None:
      remove(f)
