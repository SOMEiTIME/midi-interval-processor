#
#   Developed for use in midi-interval-processor
#   2019
#

#
# Builds the code and runs the tests on Ubuntu 18.04 with GoogleTest:
#   docker build ./ --tag midi_interval_tests
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
