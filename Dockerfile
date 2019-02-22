#
# DockerFile: create docker image for testing cube code
#   - start with ubuntu 16:04, add g++ and GoogleTest
# Usage:
#   - create:
#       docker build ./ --tag midi_interval_tests
#   - start if not running:
#       docker start cube_ci
#   - run with sh access (where [image] is the image id published by docker):
#       docker run -i -t --name cube_ci [image] /bin/bash
#     example:
#       docker run -i -t --name cube_ci 685b01fd837d /bin/bash
#   - run the test
#       docker run --name cube_ci 685b01fd837d /bin/bash -c 'cd ~/cube; make test'
#   - list images:
#       docker ps -l    # last image run
#       docker ps -a    # all images
#   - connecting to running image:
#       docker attach cube_ci
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
