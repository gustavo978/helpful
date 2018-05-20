#Helpful

Flexible Architecture to control Heterogeneous Low power wide area networks

## Getting Started

These instructions will get you a copy of the project up and running on your
local machine for development and testing purposes. See deployment for notes on
how to deploy the project on a live system.

### Prerequisites
```
apt-get update
apt-get -y install gcc g++ python python-dev
apt-get -y install mercurial python-setuptools git
apt-get -y install qt5-default
apt-get -y install python-pygraphviz python-kiwi python-pygoocanvas
libgoocanvas-dev ipython
apt-get -y install openmpi-bin openmpi-common openmpi-doc libopenmpi-dev
apt-get -y install autoconf cvs bzr unrar
apt-get -y install gdb valgrind 
apt-get -y install uncrustify
apt-get -y install doxygen graphviz imagemagick
apt-get -y install texlive texlive-extra-utils texlive-latex-extra
texlive-font-utils texlive-lang-portuguese dvipng
apt-get -y install python-sphinx dia
apt-get -y install gsl-bin libgsl2 libgsl-dev
apt-get -y install flex bison libfl-dev
apt-get -y install tcpdump
apt-get -y install sqlite sqlite3 libsqlite3-dev
apt-get -y install libxml2 libxml2-dev
apt-get -y install cmake libc6-dev libc6-dev-i386 libclang-dev
apt-get -y install libgtk2.0-0 libgtk2.0-dev
apt-get -y install vtun lxc
apt-get -y install libboost-signals-dev libboost-filesystem-dev
```

### Installing

Ansible install
```
git clone git@github.com:gustavo978/helpful.git
cd helpful
./configure.sh
./install.sh master
```

When some ns3 modification is needed you have to build it:

```
./waf configure
./waf --apiscan=netanim
./waf --apiscan=wifi
./waf --apiscan=lte
./waf --apiscan=lorawan
./waf build
./waf install
```


## Exemple

In the director '~/helpful/mininet/exemples/exemplo_1' you will find a little
examples about how to use ns3 with a p4 switch on mininet. Just run the script
exemplo_1.py and the magic will be done.
