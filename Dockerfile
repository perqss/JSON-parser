# building the image: docker build -t parser/parser_build:0.1 -f Dockerfile .
# running the container and launching bash inside: 
# $ cd project
# $ docker run -it --rm --name=parser \
#  --mount type=bind,source=${PWD},target=/src \
#  parser/parser_build:0.1 \
#  bash

# in src you'll have your current directory

FROM ubuntu:latest
LABEL Description="Build environment"

ENV HOME /root

SHELL ["/bin/bash", "-c"]

RUN apt-get update && apt-get -y --no-install-recommends install \
    build-essential \
    cmake \
    gdb \
    libcppunit-dev \