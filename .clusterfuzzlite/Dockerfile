FROM gcr.io/oss-fuzz-base/base-builder
RUN apt-get update && apt-get install -y make cmake
COPY . $SRC/tumble
WORKDIR $SRC/tumble
COPY .clusterfuzzlite/build.sh $SRC/build.sh
