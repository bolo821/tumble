FROM gcr.io/oss-fuzz-base/base-builder
RUN apt-get update && apt-get install -y make cmake zip
COPY . $SRC
WORKDIR $SRC
COPY .clusterfuzzlite/build.sh $SRC/build.sh
