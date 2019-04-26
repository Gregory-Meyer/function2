FROM gregjm/cpp-build

WORKDIR /usr/src
RUN ["git", "clone", "--single-branch", "--branch", "v2.7.2", "--depth=1", "https://github.com/catchorg/Catch2.git"]
WORKDIR /usr/src/Catch2/build
RUN ["cmake", "..", "-G", "Ninja", "-DCMAKE_BUILD_TYPE=Release"]
RUN ["cmake", "--build", "."]
RUN ["cmake", "--build", ".", "--target", "install"]

WORKDIR /usr/src/app
COPY bin bin
COPY CMakeLists.txt ./
COPY include include
COPY test test
ENTRYPOINT ["./bin/docker-entrypoint.sh"]
