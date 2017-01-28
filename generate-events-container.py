#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
file: generate-events-container.py
description: run the event simulation inside a docker container
author: Luke de Oliveira (lukedeo@vaitech.io)
"""

import docker
import os
import sys
import argparse

DEVNULL = open(os.devnull, 'wb')


client = docker.from_env()


def _run_in_container(image_name, out_file, nevents, ncpu, process, pixels,
                      range, pileup, pt_hat_min, pt_hat_max, boson_mass):

    # wherever the invoker wants to save this to, we just map
    host_root = os.path.dirname(os.path.realpath(out_file))
    rel_file = os.path.split(out_file)[-1]

    COMMAND = ' '.join([
        '--out-file=/sim/{rel_file}',
        '--nevents={nevents}',
        '--ncpu={ncpu}',
        '--process={process}',
        '--pixels={pixels}',
        '--range={range}',
        '--pileup={pileup}',
        '--pt-hat-min={pt_hat_min}',
        '--pt-hat-max={pt_hat_max}',
        '--boson-mass={boson_mass}'
    ])

    command = COMMAND.format(rel_file=rel_file, nevents=nevents, ncpu=ncpu,
                             process=process, pixels=pixels, range=range,
                             pileup=pileup, pt_hat_min=pt_hat_min,
                             pt_hat_max=pt_hat_max, boson_mass=boson_mass)

    container = client.containers.run(
        image=image_name,
        command=command,
        volumes={
            host_root: {'bind': '/sim', 'mode': 'rw'}
        },
        detach=True
    )

    for line in container.logs(stream=True):
        sys.stdout.write(line)
        sys.stdout.flush()

    return


# container = client.containers.run(image='ji:0.1',
# c'--out-file=/sim/{}'.format(rel_file), volumes={host_root: {'bind':
# '/sim', 'mode': 'rw'}}, detach=True)


# 'Can be one of ZprimeTottbar, WprimeToWZ_lept, WprimeToWZ_had, or QCD'
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--image-name', '-c', action="store",
                        default='lukedeo/ji:latest',
                        help='Docker image to use when running the sim')

    parser.add_argument('--out-file', type=str, default='events.root')
    parser.add_argument('--nevents', type=int, default=1000)
    parser.add_argument('--ncpu', type=int, default=-1)
    parser.add_argument('--process', type=str, default='WprimeToWZ_lept',
                        help='Can be one of ZprimeTottbar, WprimeToWZ_lept, '
                        'WprimeToWZ_had, or QCD', choices=['ZprimeTottbar',
                                                           'WprimeToWZ_lept',
                                                           'WprimeToWZ_had',
                                                           'QCD'])
    parser.add_argument('--pixels', type=int, default=25)
    parser.add_argument('--range', type=float, default=1.25)
    parser.add_argument('--pileup', type=int, default=0)
    parser.add_argument('--pt-hat-min', type=float, default=100)
    parser.add_argument('--pt-hat-max', type=float, default=500)
    parser.add_argument('--boson-mass', type=float, default=800)

    args = parser.parse_args()

    _run_in_container(nevents=args.nevents,
                      ncpu=args.ncpu,
                      image_name=args.image_name,
                      out_file=args.out_file,
                      pixels=args.pixels,
                      process=args.process,
                      range=args.range,
                      pileup=args.pileup,
                      pt_hat_min=args.pt_hat_min,
                      boson_mass=args.boson_mass,
                      pt_hat_max=args.pt_hat_max)
