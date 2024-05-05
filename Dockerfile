# @see https://hub.docker.com/_/ubuntu
FROM ubuntu:latest as Runner

# Define the default shell.
SHELL [ "/bin/bash", "-c" ]

# Create 'ubuntu' password-less administrator user and go home.
RUN apt-get update --yes
RUN apt-get install sudo --yes
#RUN cat /etc/passwd
RUN userdel -r ubuntu
RUN groupadd ubuntu
RUN sudo useradd --create-home --home-dir /home/ubuntu --gid sudo --groups ubuntu,sudo --shell /bin/bash ubuntu
#RUN sudo usermod -aG sudo ubuntu
RUN groups ubuntu
RUN passwd --delete ubuntu
#RUN usermod -a -G sudo ubuntu
RUN groups ubuntu
RUN getent group sudo
USER ubuntu
WORKDIR /home/ubuntu
RUN sudo chown ubuntu:ubuntu /home/ubuntu

# Copy only the Ubuntu scripts inside image.
COPY script/ubuntu/ script/ubuntu/
RUN sudo chown ubuntu:ubuntu --recursive /home/ubuntu

# Install dependencies.
RUN chmod u+x --recursive script/ubuntu
RUN script/ubuntu/install.sh

# Cleaning image.
RUN rm --force --recursive script
