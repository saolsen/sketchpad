FROM clearlinux:base
RUN swupd bundle-add kernel-container user-basic c-basic
RUN clrtrust generate

ENV HOME /root
WORKDIR /sketchbook