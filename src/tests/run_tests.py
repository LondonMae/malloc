#!/usr/bin/env python3.7
# 3.7 is necessary for capture_output flag to subprocess.run

import argparse
import subprocess
import sys
import time

def b2s(b):
    return b.decode('UTF-8')


def run_test(test, writer, l):
    start = time.time()
    print('Running: {name:.<{width}s} '.format(name=test+' ', width=(l+5)),
            end='', flush=True)
    #print('Running: {name}'.format(name=test))
    writer('## {}\n\n'.format(test))
    result = subprocess.run([test], capture_output=True)
    console_output = '✅ passed!'
    output = '✅ __SUCCESS__'
    if result.returncode:
        output = '❌ __FAILED__'
        console_output = '❌ failed.'
    writer('stdout:\n{}\nstderr:\n{}\n{}\n\n'.format(
        b2s(result.stdout), b2s(result.stderr), output))
    end = time.time()
    print('{} ({:.3f}s)'.format(console_output, end-start))
    return result.returncode == 0

def run_tests(tests, writer, short_circuit=False):
    writer('# Lab 5 Allocator Test Output\n\n')
    l = max([len(s) for s in tests])
    failures = 0
    for test in tests:
        success = run_test(test, writer, l)
        if not success:
            failures += 1
            if short_circuit:
                break
    return failures

def main():
    parser = argparse.ArgumentParser(
            prog='Lab 5 Test Harness',
            description='Runs a suite of test binaries against your allocator')
    parser.add_argument('-t', '--tests', dest='tests', required=True,
            help='list of tests to run, separated by commas')
    parser.add_argument('-o', '--output', dest='output',
            help='output file (defaults to standard out)')
    parser.add_argument('-p', '--prefix', default='./')
    parser.add_argument('--fail_fast', action='store_true')
    args = parser.parse_args()
    tests = [args.prefix+t for t in args.tests.split(',')]
    out = sys.stdout
    if args.output:
        out = open(args.output, 'w')
    failures = run_tests(tests, lambda o: print(o, file=out), short_circuit=args.fail_fast)
    if not failures:
        print('all tests passed! 🎉')
    else:
        passed = len(tests)-failures
        print('{}/{} passed {}'.format(passed, len(tests),
                ['😔', '🙁', '😕', '🙂', '😇'][int(5 * passed/len(tests))]))
    if args.output:
        out.close()

if __name__ == '__main__':
    main()

