FROM alpine:3.20
# base dev dependencies, git for fetch content
RUN apk add --no-cache build-base ccache cmake ninja-build ninja-is-really-ninja git
# some coverage and linkers
RUN apk add --no-cache lcov gzip lld binutils-gold
# clang and llvm TODO, this is version dependend, should be specified in the future
RUN apk add --no-cache llvm clang compiler-rt clang17-extra-tools
# dev dependencies, for now (until switch to vcpkg happens)
RUN apk add sqlite-dev

# avoid ccache permission issues
ENV CCACHE_DIR=/tmp/ccache
ARG UNAME=user
ARG UID=1000
ARG GID=1000
RUN adduser -u ${UID} -g ${GID} ${UNAME} -D -h /home/user
