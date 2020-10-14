FROM buildpack-deps:focal

ENV LANG=en_US.UTF-8
### base ###
RUN yes | unminimize \
    && apt-get install -yq \
        asciidoctor \
        bash-completion \
        build-essential \
        htop \
        jq \
        less \
        locales \
        man-db \
        nano \
        software-properties-common \
        sudo \
        vim \
        multitail \
        lsof python3 libpython3-dev libblas-dev liblapack-dev libunwind-dev gcc g++ gfortran python3-pip \
    && locale-gen en_US.UTF-8 \
    && mkdir /var/lib/apt/dazzle-marks \
    && apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* \

### Git ###
    && add-apt-repository -y ppa:git-core/ppa \
    && apt-get install -yq git \
    && rm -rf /var/lib/apt/lists/* \

### Gitpod user ###
# '-l': see https://docs.docker.com/develop/develop-images/dockerfile_best-practices/#user
    && useradd -l -u 33333 -G sudo -md /home/gitpod -s /bin/bash -p gitpod gitpod \
    # passwordless sudo for users in the 'sudo' group
    && sed -i.bkp -e 's/%sudo\s\+ALL=(ALL\(:ALL\)\?)\s\+ALL/%sudo ALL=NOPASSWD:ALL/g' /etc/sudoers
ENV HOME=/home/gitpod
WORKDIR $HOME
# custom Bash prompt
RUN { echo && echo "PS1='\[\e]0;\u \w\a\]\[\033[01;32m\]\u\[\033[00m\] \[\033[01;34m\]\w\[\033[00m\] \\\$ '" ; } >> .bashrc

### Gitpod user (2) ###
USER gitpod
# use sudo so that user does not get sudo usage info on (the first) login
RUN sudo echo "Running 'sudo' for Gitpod: success"
# create .bashrc.d folder and source it in the bashrc
#RUN mkdir /home/gitpod/.bashrc.d && \
#    (echo; echo "for i in \$(ls \$HOME/.bashrc.d/*); do source \$i; done"; echo) >> /home/gitpod/.bashrc

### Install C/C++ compiler and associated tools ###
LABEL dazzle/layer=lang-c
LABEL dazzle/test=tests/lang-c.yaml
USER root
RUN curl -fsSL https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - \
    && echo "deb http://apt.llvm.org/focal/ llvm-toolchain-focal main" >> /etc/apt/sources.list.d/llvm.list \
    && apt-get update \
    && apt-get install -yq \
        clang-format \
        clang-tidy \
        # clang-tools \ # breaks the build atm
        clangd \
        gdb \
        lld \
    && cp /var/lib/dpkg/status /var/lib/apt/dazzle-marks/lang-c.status \
    && apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* \

    && python3 -m pip install --upgrade pip && python3 -m pip install pint pydantic matplotlib qiskit scipy numpy pyquil cmake virtualenv pipenv pylint rope flake8 mypy autopep8 pep8 pylama pydocstyle bandit notebook python-language-server[all]==0.25.0 \
    && sudo rm -rf /tmp/* \
    && git clone https://github.com/psi4/psi4 && cd psi4 && mkdir build && cd build \
    && cmake .. -DPYTHON_EXECUTABLE=$(which python3) -DCMAKE_INSTALL_PREFIX=$(python3 -m site --user-site)/psi4 \
    && make -j8 install && cd ../.. && rm -rf psi4
ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update && apt-get install -y texlive-latex-recommended texlive-latex-extra dvipng \
    && apt-get clean && rm -rf /var/lib/apt/lists/* \

USER gitpod

