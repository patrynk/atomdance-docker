# Some configuration options adapted from https://github.com/jozo/docker-pyqt5/blob/master/default/Dockerfile
FROM ubuntu:22.04

# Add analysis user
RUN adduser --quiet --disabled-password atomuser && usermod -a -G audio atomuser

# Set environment variables
ENV LIBGL_ALWAYS_INDIRECT=1
ENV XDG_RUNTIME_DIR='/tmp/runtime-atomuser'
ENV DEBIAN_FRONTEND=noninteractive

# Install system dependencies
RUN apt-get update && apt-get install -y \
    python3-pyqt5 \
    build-essential \
    g++\
    gfortran \
    m4 \
    wget \
    curl \
    zlib1g \
    zlib1g-dev \
    gdebi

# cpptraj installation workflow, ver. 6.4.4
COPY --chown=atomuser:atomuser ./lib/cpptraj /cpptraj/
WORKDIR /cpptraj/
RUN ./configure gnu --buildlibs
RUN make install

WORKDIR /

# ATOMDANCE installation workflow, ver 1.4.2
COPY --chown=atomuser:atomuser ./lib/ATOMDANCE-comparative-protein-dynamics /atomdance/

# Setup python environment
USER atomuser
RUN python3 -m pip install PyQt5 pandas scikit-learn plotnine progress seaborn pandas_flavor outdated tabulate pingouin
WORKDIR /atomdance/

# Setup start, install-chimerax commands
COPY --chown=atomuser:atomuser ./bin/start /usr/bin/start
RUN chmod +x /usr/bin/start

# Enable running manually with user login to container, i.e. 'docker-compose run -u atomuser -v /PATH/TO/SHARED/DIRECTORY:/data atomdance /bin/bash'
RUN echo 'export CPPTRAJ_HOME=/cpptraj' >> /home/atomuser/.bashrc
RUN echo 'export PATH=$PATH:$CPPTRAJ_HOME/bin:/home/atomuser/.local/bin' >> /home/atomuser/.bashrc