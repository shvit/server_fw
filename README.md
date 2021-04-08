# TFTP firmware server

## Intro

This TFTP firmware server wonderful used in embedded firmware development.<br>
When you have many firmware shapshots for complex testing (regression, functional, etc).<br>
You don't operate file names or directories of firmware, no, now only its md5 sums.

## Futures

* get file by md5 sum in *.md5 files
* one root server directory for server upload (and download)
* can set many serach directory for server download

## Plans

* using Firebird SQL server as main firmware storage

## Requirements

* Ubuntu 18.04 LTE (newer not tested)
<p>
Other GNU Linux distros not tested, sorry
</p>
* Clang or GCC as last stable version (using C++17)
<p>
For install clang:
</p>
<pre>
sudo apt-get update
sudo apt-get install llvm clang clang-tools
</pre>
* OpenSSL library
<pre>
sudo apt-get install libssl-dev
</pre>



## How build and use
1. Get sources 
<pre>
mkdir server_fw
git clone git@github.com:shvit/server\_fw.git -b master server_fw
</pre>
2. Make binary file
<pre>
cd server_fw
make release
</pre>
3. Disable any listening 69 port other executables
<pre>
sudo netstat -lup|grep "69\|tftp"
</pre>
4. Install firmware tftp server
<p>
<pre>
make install
</pre>
By default, make root server directory  _/mnt/tftp_  and one search directory _/mnt/backup_
<pre>
mkdir /mnt/tftp
mkdir /mnt/backup
</pre>
</p>
5. Copy firmware files to directory  _/mnt/tftp_  or  _/mnt/backup_  and make *.md5 files
<p>
File in search directory  _/mnt/backup_  can place in separate nested directory or with unique file names<br>
For each  _file_  in search directory make *.md5 file:
<pre>
md5sum file > file.md5
</pre>
</p>
<p>
6. Uninstall firmware tftp server
<pre>
make uninstall
</pre>
</p>

Profit!

## License

GNU general public license version 3

## Author
Vitaliy Shirinkin, Russia, 2019-2021<br>
e-mail: vitaliy.shirinkin@gmail.com
