FROM clearlinux:base
RUN swupd bundle-add kernel-container user-basic c-basic
RUN clrtrust generate

ENV HOME /root
WORKDIR /root

#RUN mkdir -p /root/.emacs.d
#ADD etc/init.el /root/.emacs.d/init.el
#ADD etc/.tmux.conf /root/.tmux.conf
#ADD etc/.gitconfig /root/.gitconfig