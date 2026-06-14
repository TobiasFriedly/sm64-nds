FROM devkitpro/devkitarm:20240511 as build

RUN apt update
RUN apt -y install build-essential bsdmainutils sox libaudiofile-dev
RUN mkdir /sm64
WORKDIR /sm64

CMD echo 'usage: docker run --rm --mount type=bind,source="$(pwd)",destination=/sm64 sm64ds make VERSION=us COMPILER=gcc -j4\n' \
         'see https://github.com/Hydr8gon/sm64/blob/nds/README.md for advanced usage'
