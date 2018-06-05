FROM gcc:latest

RUN apt-get -yq update
RUN apt-get -yqq install openssl zlib1g-dev libgoogle-glog-dev libevent-dev git cmake \
  libboost-dev libgtest-dev libevent-dev libgflags-dev libboost-system-dev libboost-thread-dev \
  rpm

# uWebSockets
RUN git clone https://github.com/uNetworking/uWebSockets.git /usr/local/src/uwebsockets
WORKDIR /usr/local/src/uwebsockets
RUN make && make install

# evpp
RUN git clone https://github.com/Qihoo360/evpp /usr/local/src/evpp
WORKDIR /usr/local/src/evpp
RUN git submodule update --init --recursive
WORKDIR /usr/local/src/evpp/tools
RUN ./release-build.sh

# helixproxy
RUN git clone https://github.com/Hoffs/HelixProxy.git /usr/local/src/HelixProxy
WORKDIR /usr/local/src/HelixProxy
RUN g++ \
  -I/usr/local/src/evpp/ \
  HProxy/main.cpp HProxy/helix_proxy_server.cpp HProxy/helix_user.cpp \
  /usr/local/src/evpp/build-release/lib/libevpp_static.a -lssl -lcrypto -lz -luWS -lpthread -levent -lglog \
  -o helixproxy
RUN chmod a+x helixproxy

# Runnable image
FROM debian:stretch-slim
RUN apt-get -yq update
RUN apt-get -yqq install openssl libevent-2.0-5 libgoogle-glog0v5 zlib1g libuv1
WORKDIR /root/
COPY --from=0 /usr/local/src/HelixProxy/helixproxy .
COPY --from=0 /usr/local/src/uwebsockets/libuWS.so /usr/lib
ENV IN_PORT=4300
ENV OUT_PORT=4300
CMD ./helixproxy ${IN_PORT} ${OUT_PORT}