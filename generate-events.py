#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
file: generatee-events.py
description: build some pythia events
author: Luke de Oliveira (lukedeo@vaitech.io)
"""
from multiprocessing import Pool, cpu_count
from joblib import Parallel, delayed
from numpy import floor
import os
import sys
import argparse
from subprocess import Popen, PIPE, STDOUT

DEVNULL = open(os.devnull, 'wb')


def determine_allocation(n_samples, n_cpus=-1):
    if n_cpus < 0:
        n_cpus = max(cpu_count() - 1, 1)
    if n_cpus > (cpu_count() - 1):
        n_cpus = max(cpu_count() - 1, 1)

    allocation = n_cpus * [0]

    if n_samples <= n_cpus:
        for i in xrange(0, n_samples):
            allocation[i] += 1
        return allocation

    per_cpu = int(floor(float(n_samples) / n_cpus))

    added = 0
    to_add = n_samples

    for i in xrange(n_cpus):
        if i == (n_cpus - 1):
            if added + to_add < n_samples:
                to_add = n_samples - added
                leftover_allocation = determine_allocation(
                    to_add - per_cpu, n_cpus)
                for i in xrange(n_cpus):
                    allocation[i] += leftover_allocation[i]
                to_add = per_cpu
        else:
            to_add = min(per_cpu, n_samples - added)
        allocation[i] += to_add
        added += to_add
    return allocation


def generate_calls(n_events, n_cpus=-1, executable='bin/jet-image-maker',
                   outfile='gen.root', pixels=25, process='WprimeToWZ_lept',
                   imrange=1, pileup=0, pt_hat_min=100, pt_hat_max=500,
                   bosonmass=800):
    if n_cpus < 0:
        n_cpus = max(cpu_count() - 1, 1)
    if n_cpus > (cpu_count() - 1):
        n_cpus = max(cpu_count() - 1, 1)
    print 'Splitting event generation over {} CPUs'.format(n_cpus)
    events_per_core = zip(determine_allocation(n_events, n_cpus), range(n_cpus))

    def _filename_prepare(f):
        if f.find('.root') < 0:
            return f + '.root'
        return f

    lookup = {'ZprimeTottbar': "1",
              'WprimeToWZ_lept': "2",
              'WprimeToWZ_had': "3",
              'QCD': "4"}

    if n_cpus == 1:
        _call = [executable,
                 '--out-file', _filename_prepare(outfile),
                 '--nb-events', str(n_events),
                 '--process', lookup[process],
                 '--nb-pixels', str(pixels),
                 '--image-range', str(imrange),
                 '--pileup', str(pileup),
                 '--pt-hat-min', str(pt_hat_min),
                 '--pt-hat-max', str(pt_hat_max),
                 '--boson-mass', str(bosonmass)]
        return [_call]

    def _generate_call(par):

        try:
            _ = lookup[process]
        except:
            raise ValueError('process can be one of ' +
                             ', '.join(a for a in lookup.keys()))

        _call = [executable,
                 '--out-file', _filename_prepare(outfile).replace(
                     '.root', '_cpu{}.root'.format(par[1])),
                 '--nb-events', str(par[0]),
                 '--process', lookup[process],
                 '--nb-pixels', str(pixels),
                 '--image-range', str(imrange),
                 '--pileup', str(pileup),
                 '--pt-hat-min', str(pt_hat_min),
                 '--pt-hat-max', str(pt_hat_max),
                 '--boson-mass', str(bosonmass)]
        return _call

    return [_generate_call(c) for c in events_per_core]


def excecute_call(f):
    return os.system(' '.join(f))


# 'Can be one of ZprimeTottbar, WprimeToWZ_lept, WprimeToWZ_had, or QCD'
if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('--out-file', type=str, default='events.root')
    parser.add_argument('--nevents', type=int, default=1000)
    parser.add_argument('--ncpu', type=int, default=-1)
    parser.add_argument('--process', type=str, default='WprimeToWZ_lept',
                        help='Can be one of ZprimeTottbar, WprimeToWZ_lept, '
                        'WprimeToWZ_had, or QCD', choices=['ZprimeTottbar',
                                                           'WprimeToWZ_lept',
                                                           'WprimeToWZ_had',
                                                           'QCD']
                        )
    parser.add_argument('--pixels', type=int, default=25)
    parser.add_argument('--range', type=float, default=1.25)
    parser.add_argument('--pileup', type=int, default=0)
    parser.add_argument('--pt-hat-min', type=float, default=100)
    parser.add_argument('--pt-hat-max', type=float, default=500)
    parser.add_argument('--boson-mass', type=float, default=800)
    parser.add_argument('--executable', '-e', action="store",
                        default='bin/jet-image-maker',
                        help='alt. path to the jet-image-maker executable')

    args = parser.parse_args()

    calls = generate_calls(n_events=args.nevents,
                           n_cpus=args.ncpu,
                           executable=args.executable,
                           outfile=args.out_file,
                           pixels=args.pixels,
                           process=args.process,
                           imrange=args.range,
                           pileup=args.pileup,
                           pt_hat_min=args.pt_hat_min,
                           bosonmass=args.boson_mass,
                           pt_hat_max=args.pt_hat_max)

    _ = Parallel(n_jobs=args.ncpu, verbose=True)(
        delayed(excecute_call)(c) for c in calls
    )
