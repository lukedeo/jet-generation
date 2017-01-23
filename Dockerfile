
sudo apt-get update && sudo apt-get -y --force-yes install   \
        bc                           \
        curl                         \
        git                          \
        wget                         \
        python-dev                   \
        python-pip                   \
        python-numpy                 \
        python-scipy                 \
        libx11-dev                   \
        libxpm-dev                   \
        libxft-dev                   \
        libxext-dev                  \
        libpng3                      \
        libjpeg8                     \
        gfortran                     \
        libssl-dev                   \
        libpcre3-dev                 \
        libgl1-mesa-dev              \
        libglew1.5-dev               \
        libftgl-dev                  \
        libmysqlclient-dev           \
        libfftw3-dev                 \
        libcfitsio3-dev              \
        graphviz-dev                 \
        libavahi-compat-libdnssd-dev \
        libldap2-dev                 \
        libxml2-dev  &&              \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

sudo pip install --upgrade pip
sudo pip install --no-cache-dir cython scikit-image==0.10.0


# all SW is installed under /opt
WORKDIR /opt

# unpack ROOT to /opt/troot
RUN wget -O root.tgz \
    https://root.cern.ch/download/root_v5.34.36.Linux-ubuntu14-x86_64-gcc4.8.tar.gz && \
    tar -xzf root.tgz 


# ROOT specific python packages
RUN pip install --no-cache-dir \
    rootpy \
    root_numpy

# Install Pythia8
RUN wget http://home.thep.lu.se/~torbjorn/pythia8/pythia8219.tgz && \
    tar -xzf pythia8219.tgz
RUN cd pythia8219 && \
    ./configure   && \
    make          && \
    make install  && \
    cd /opt

# Install FastJet
RUN wget http://fastjet.fr/repo/fastjet-3.2.1.tar.gz && \
    tar -xzf fastjet-3.2.1.tar.gz
RUN cd fastjet-3.2.1 && \
    ./configure      && \
    make             && \
    make install     && \
    cd /opt

# Setup all system level

RUN pip install joblib

ADD generation /root/generation

WORKDIR /root/generation

RUN make



# VOLUME /data

ENTRYPOINT ["python", "generateEvents.py"]
# ENTRYPOINT ["jet-image-maker"]
