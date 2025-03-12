FROM archlinux:latest
LABEL authors="abareux"

RUN pacman -Suy --noconfirm
RUN pacman -S cmake make clang --noconfirm

COPY ./Project /Project
WORKDIR /Project

RUN rm -rf cmake-build-*

RUN cmake ./ -B build/
RUN make -C build

ENTRYPOINT ["/Project/build/DockerTemplate", "80"]