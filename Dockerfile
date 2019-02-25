#
#   Developed for use in midi-interval-processer 
#   Benjamin Hasker
#   2019
#

#
# DockerFile: create docker image for testing code
#   - start with ubuntu 16:04, add g++ and GoogleTest
# Usage:
#   - create:
#       docker build ./ --tag midi_interval_tests
#   - start if not running:
#       docker start midi_to_interval
#   - run with sh access (where [image] is the image id published by docker):
#       docker run -i -t --name midi_to_interval [image] /bin/bash
#     example:
#       docker run -i -t --name midi_to_interval 685b01fd837d /bin/bash
#   - run the test
#       docker run --name midi_to_interval midi_interval_tests /bin/bash -c 'cd ~/midi_to_interval ; make test'
#   - list images:
#       docker ps -l    # last image run
#       docker ps -a    # all images
#   - connecting to running image:
#       docker attach midi_to_interval
#

FROM ubuntu:18.04
# install build tools
RUN apt-get update; apt-get install -y g++ cmake git libgtest-dev
# set up GoogleTest
RUN cd /usr/src/gtest; cmake CMakeLists.txt; make; cp *.a /usr/lib
# copy code under test (current repo) into container
COPY . /midi_to_interval
# build the code and run tests
RUN cd /midi_to_interval; make clean; make test
