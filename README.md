# `jet-generation`

This is a framework for building jet images used in [`arXiv:1701.05927`](https://arxiv.org/abs/1701.05927). You can either build the framework on your computer normally, or you can run the entire thing in a Docker container (we recommend the latter unless you have a specific reason, *especially* if you're just running this on your computer).

# Getting Started

Make sure to run `git submodule update --init --recursive` to clone the required submodules as well.

# Running with Docker


## Dependencies


You'll need :

* Docker (a new version, untested with anything but `v1.13.0`, but we're not doing anything fancy here so it should work)
* `docker-py` (ideally, `>=2.0.0`)

Just install `docker-py` with `pip install docker`, perhaps with a `sudo` thrown in there if wherever pip installs things isn't writable by you directly.


## Building the Docker Image

If you've decided you want to use Docker (which we highly recommend) you can either build the image locally, or just run the script which will download a pre-built image from Docker Hub (this is much easier 🙂). If you want to build the image, you can just run `docker build -it myimagename .` in the root directory of this repository. Or, if you want to make the named version used for Docker Hub, simply type  `make -f Makefile.docker build`

## Building the framework (with Docker)

No building! Accomplished in the previous step.


# Running directly on hardware


## Dependencies

### Dependencies overview

If you're not running in a docker container, you'll need


* The Python requirements outlined in `requirements.txt`
* `fastjet` version >= 3.1.0
* `Pythia` version >= 8.1
* `ROOT`


### Installing dependencies

Because of how annoying this all is (and how finnicky HEP software tends to be), I recommend running inside Docker.

If you're on Windows, I have no clue how to help you -- if you do manage to get this working, **please** submit a PR!

For `FastJet` and `Pythia`, prefer an already-built version if you're on any CERN computing hardware. If you insist on building it yourself on your own computer (and you *have* root access), just run `sudo scripts/install-fastjet.sh && sudo scripts/install-pythia.sh`. 

For `ROOT`, if you have it installed just leave it and use that one. If you don't and you're on a `*`nix system, type `sudo scripts/install-root.sh` If you're on OSX, make sure Homebrew is installed and type `scripts/install-root-osx.sh`. **DO NOT USE SUDO ON OSX**.

After all of this, **you may need to re-`source` your `.bashrc`** or simply log into a new window, to make sure changes to your path are honored.



## Building the framework (not Docker)

Typing `make -j` should do the trick on most systems if you've either followed the above instructions or if because of previous installations you have `fastjet-config`, `pythia8-config`, and `root-config` in your `$PATH`. This builds the low level script, and for most use cases, should not be needed to be productive.


# Event generation

Run `./generate-events.py` (if you're running directly on hardware) or `./generate-events-container.py` (if you're running on docker). Pass with a `-h` flag to see what options are available.

`./generate-events-container.py` is simply [collecting command-line arguments](https://github.com/lukedeo/jet-generation/blob/master/generate-events-container.py#L31-L42) to then run `./generate-events.py` via this [command in the Dockerfile](https://github.com/lukedeo/jet-generation/blob/master/Dockerfile#L121).

`./generate-events` is itself a wrapper that generates calls to the binary associated with [`./src/src/jet-image-maker.cc`](https://github.com/lukedeo/jet-generation/blob/master/src/src/jet-image-maker.cc).

The file [`./src/src/jet-image-maker.cc`](https://github.com/lukedeo/jet-generation/blob/master/src/src/jet-image-maker.cc) acts as a Pythia8 option config file.

Running `./generate-events-container.py -h` provides the following information:
```
usage: generate-events-container.py [-h] [--image-name IMAGE_NAME]
                                    [--out-file OUT_FILE] [--nevents NEVENTS]
                                    [--ncpu NCPU]
                                    [--process {ZprimeTottbar,WprimeToWZ_lept,WprimeToWZ_had,QCD}]
                                    [--pixels PIXELS] [--range RANGE]
                                    [--pileup PILEUP]
                                    [--pt-hat-min PT_HAT_MIN]
                                    [--pt-hat-max PT_HAT_MAX]
                                    [--boson-mass BOSON_MASS]

optional arguments:
  -h, --help            show this help message and exit
  --image-name IMAGE_NAME, -i IMAGE_NAME
                        Docker image to use when running the sim (default:
                        lukedeo/ji:latest)
  --out-file OUT_FILE, -o OUT_FILE
  --nevents NEVENTS, -n NEVENTS
  --ncpu NCPU
  --process {ZprimeTottbar,WprimeToWZ_lept,WprimeToWZ_had,QCD}, -p {ZprimeTottbar,WprimeToWZ_lept,WprimeToWZ_had,QCD}
                        Can be one of ZprimeTottbar, WprimeToWZ_lept,
                        WprimeToWZ_had, or QCD (default: WprimeToWZ_lept)
  --pixels PIXELS
  --range RANGE
  --pileup PILEUP
  --pt-hat-min PT_HAT_MIN
  --pt-hat-max PT_HAT_MAX
  --boson-mass BOSON_MASS
```
You can test your setup by running: `./generate-events-container.py -o test.root -n 10 -p QCD`, which is supposed to produce an output file called `test.root` with 10 QCD events. It will contain a single tree called `EventTree`. The branches are defined and filled in [`./src/src/JetImageBuffer.cc`](https://github.com/lukedeo/jet-generation/blob/master/src/src/JetImageBuffer.cc).

Warning: pulling the image might take a while!

#### Notes:
`pixels` refers to the number of pixels per side in a square image. The number is then squared in the code to get the total image size, saved in branch called `NFilled`.

# Image pre-processing

Image pre-processing is handled in the `python/` directory. 


```
me@mycomputer$ python jetconverter.py --help
usage: jetconverter.py [-h] [--verbose] [--signal SIGNAL] [--save SAVE]
                       [--plot PLOT]
                       [files [files ...]]

positional arguments:
  files            Files to pass in

optional arguments:
  -h, --help       show this help message and exit
  --verbose        Verbose output
  --signal SIGNAL  String to search for in filenames to indicate a signal file
  --save SAVE      Filename to write out the data.
  --plot PLOT      File prefix that will be part of plotting filenames.
```

An example invocation could look like `./jetconverter.py --signal=Wprime --save=data.hdf5 ./data/*.root`.


