# image: cowlog/jos

FROM ubuntu:18.04

MAINTAINER cowlog

RUN apt-get -y update && apt-get -y upgrade
RUN apt-get install -y openssh-server openssh-client
RUN mkdir /var/run/sshd
RUN echo 'root:1' | chpasswd
RUN sed -i 's/PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config
RUN sed 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' -i /etc/pam.d/sshd
ENV NOTVISIBLE "in users profile"
RUN echo "export VISIBLE=now" >> /etc/profile

EXPOSE 22
CMD ["/usr/sbin/sshd", "-D"]

RUN apt-get install -y build-essential libtool libglib2.0-dev libpixman-1-dev zlib1g-dev git libfdt-dev gcc-multilib gdb tcpdump netcat python

RUN git clone https://github.com/mit-pdos/6.828-qemu.git /root/qemu

WORKDIR /root/qemu
RUN /root/qemu/configure --disable-kvm --disable-werror --target-list="i386-softmmu x86_64-softmmu"
RUN make && make install
