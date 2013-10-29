parker
=====

An open source FTP Search engine

Thanks to Nanjing University and all the contributors.
Lisence: GPL v2.0
Original project on sourceForge, i copy them to github.

Version 5.0 Last edited: 2013-10-29 by lijingpeng
1.修给Makefile，执行make install可以直接将可执行文件安装到bin和/var/www/cgi-bin，将网页安装到/var/www/parker
2.执行make realclean删除的时候将会直接删除bin文件和网页文件，变得更加彻底
3.添加apache配置文件用户需要将parker_apache拷贝到apache的配置文件下，并重启apache

New installation guide:
1. Download source
    git clone git@github.com:lijingpeng/parker.git
2. make && make install
    cd /src
    make
    sudo make install
3. apache setting
    sudo cp parker_apache /etc/apache2/sites-avaliable
    sudo service apache2 restart

Tips
1.  Browse in Chrome or firefox
    http://localhost/parker
2.  If you want to change install destination, just edit Makefile and change file path.

The following guide was old.
_____________________________________________________________________________________________________________
New installation guide:
Edited date:2013.10.09 by lijingpeng
##############################################################
Problem:
Gibberish exits when browsing windows based FTP site. 
##############################################################
--------------------------------------------------------------
For linux system:

###Basic section
1. Download the source code:

    git clone git@github.com:lijingpeng/parker.git

2. Copy parker to /var

    sudo cp -R parker /var

3. Copy /parker/HTML folder to /var/www/

    sudo cp -R parker/html /var/www

    ##For apache settings, see apendix 1.

4. Change owner of /var/www/parker
    
    sudo chown -R www-data:www-data /var/www/parker

5. Make true that cgi-bin/ folder exists in /var/www/, if not, create that

    sudo mkdir /var/www/cgi-bin
    
    Then you need to make true that it is correctly configured in apache, guides in apendix 1.

###Advanced section
6.  Compile source code
    
    cd /var/parker/etc
    sudo make & make install

###About how to use
7.  After compiled, bin files will be created in src/ folder, you can copy them to /parker/bin

8.  Edit CollectList file in parker/etc/

    cd /var/parker/parker
    sudo vim CollectList
#   add FTP site like:
    ftp://username:password@219.225.230.120:21

    Optional:
    sudo mkdir /var/www/cgi-bin/parker
    sudo mkdir /var/www/cgi-bin/parker/etc
    sudo cp /var/parker/etc/CollectList /var/www/cgi-bin/parker/etc
9.  Collect data
    
    cd /var/parker/bin
    ./collect

10. More functions are listed in this file bellow.

###Apendix
1.  Web site settings
    After you copy parker/html to /var/www/, you need to configure that in apache
    a.  go to apache config folder
        cd /etc/apache2
    b.  add new web site
        cd /sites-avaliable 
        sudo vim parker
    c.  add to parker

        <VirtualHost *:80>
		DocumentRoot /var/www/
		<Directory />
			Options FollowSymLinks
			AllowOverride None
		</Directory>
		<Directory /var/www/parker>
			Options Indexes FollowSymLinks MultiViews
			AllowOverride None
			Order allow,deny
			allow from all
		</Directory>
	
		ScriptAlias /cgi-bin/ /var/www/cgi-bin/
		<Directory "/var/www/cgi-bin">
			AllowOverride None
			Options +ExecCGI -MultiViews +SymLinksIfOwnerMatch
			Order allow,deny
			Allow from all
		</Directory>
	</VirtualHost>

2.  Chrome 404 Error: search is not in this server
    Solution:

    copy /var/parker/bin/search or /var/parker/src/#bin files# to /var/www/cgi-bin/

**************************************************************
*
*                       Parker  4.2
*
*           Copyright (c)2001-2002 by steff( 李翀 )
*           Nanjing University, China, 210093
*                  All rights reserved.
*
**************************************************************

    Parker是一个Linux下ftp搜索引擎服务程序包，原来由Jiri A. Randus所
写，后经国内高手多次改进，这个版本是在西安交大的汉化版上经过大量修改
得到的。由于目前教育网内FTP还非常的实用，而收集和寻找各个FTP是非常需
要的，希望这个软件包能够给需要的朋友带来一些方便。
    在此我要感谢曾经给我帮助的朋友们，认识的、不认识的，是你们的鼓励
我把这个软件一直做下来。
    本软件包遵循GPL 2.0协议, 任何人可以使用,修改,传播,得到源代码。
为了能够联合大家一起开发Parker, 我在sourceForge上申请了一个Project.
欢迎感兴趣的人一起来把Parker做的更好。

Author: steff
E-mail: mesolinux@263.net
Home  : http://sourceforge.net/projects/parker
CVS   : cvs.parker.sourceforge.net

***************************************************************
*  安装指南：
***************************************************************
1 建议把变动的数据放在/var下。
  以root登陆，
  解开文件包(如果已经解开,可以拷贝过去)

	# cd /var
	# tar vxzf parker-200x-xx-xx.tar.gz
	# ln -s parker-200x-xx-xx parker

  在你的web服务程序(如apache)的cgi-bin目录下建立parker目录

	# mkdir /var/www/cgi-bin/parker

  把parker里的网页部分内容复制到你的web服务程序的相应目录(如apache
  的DocumentRoot所指的位置）

	# cp /var/parker-200x-x-xx/html/parker /var/www/html/parker -r

2 设定各个文件的读写权限

	# chown nobody.nobody /var/parker-200x-xx-xx -R
	# chmod 770 /var/parker-200x-xx-xx -R

   nobody是cgi的执行用户.

3 修改源程序文件路径：
  (如果改变安装目录的话)(默认为/var/parker)

  src/Makefile		PARKER_HOME= /var/parker            (安装目录)
			CGI_HOME= /var/www/cgi-bin/parker (cgi目录)

4 根据喜好修改cgi输出界面
  src/cgi*		根据需要修改，主要修改页面的输出外观
  			主要输出函数是
		370	PutHeaders(void)
		386	Search(void)
		494	void PutEnd(void)
  还有显示定义文件  src/language.h 
5 重新编译代码
	#cd src
	#make
	#make install
  
  然后可以把各个相关目录的权限再检查一下。
  
6 配置crontab，以便定时自动更新数据
  crontab -u parker -e
   1 1 * * * /var/parker/bin/flashdata %(这里是每天更新)

7 编辑站点列表文件 /var/parker/etc/CollectList 

8 执行搜集数据的命令:
  [root@linux /var/parker/bin]$ ./flashdata &

9 测试搜索引擎
  html是我提供的页面例子.
  http://localhost/Parker/index.html
***************************************************************
*  下面是各个文件和目录的作用和安排:
***************************************************************
bin/:
  collect [-l|-r|-d] [ftp site]
		搜集ftp站点数据用的, ftp站点定义在 etc/CollectList
                数据压缩后放在放在LZOData下. 临时工作目录在 tmp/, 站点的统计数据放在 Statist/下
		参数使用如下: 	-l 使用LIST -lR命令列出服务区上的目录；
				-d 通过下载类似于 ls-lR.tgz的文件来获得目录。（有的服务器上会定时
					建立这样的文件。
				-r 自动进入各个子目录得到整个目录的列表。
				默认的调用顺序是 -l,-d,-r 搜索etc/CollectList中的ftp站点

  flashdata	更新数据的调度文件.调用collect和makestat

  convertmask	把从http://www.nic.edu.cn/RS/ipstat/internalip/
		取得的教育网免费IP定义列表更新到$parker/HostData/Edu.mask
		要求输入文件格式如下:
                host	subnet	netmask
	如	202.119.32.0	0.0.31.255	255.255.224.0
		使用方法：设拷贝下来的文件为list.txt, 执行convertmask list.txt

  flashlist	一段时间后,把很大的$parker/etc/onworkhost一些无法联结的
		IP去掉, 备份文件在$parker/etc/backlist

  listfilter	把搜集到的ftp站点列表转成onworkhost

  getdnsname <ip>
		返回ip的DNS名字. 没有的话,返回空行.

  makestat	对ftp站点上的各种文件进行统计排序, 供Statist使用，可以分析不同站点对不同文件的偏好。

  lzo_comp	把目录数据用LZO算法进行压缩的程序. 读取的部分由search直接进行。

src/:
  Makefile	make定义文件
  config.h	目录,文件、目录的定义, make时由conf.head和conf.end生成
  conf.head
  conf.end
  parker.h	头文件,各个程序的定义, 

  collect.c	搜集ftp站点数据的Ｃ代码源程序
  collect.h	头文件
  listfilter.c	能够比较自动的一些ftp站点列表文件中抽取从ip, 例如文件oldlist.txt如下:
        ---------------------------------------
        南大FTP: 202.119.32.32
        交大FTP: xxx.xxx.xxx.xx     "good"
        202.119.46.57
        清华 166.111.19.56
            ..........
            ........
        ---------------------------------------
	使用命令"listfilter oldlist", 可以得到
	-----------------------------------
	202.119.32.101
	xxx.xxx.xxx.xx
	166.111.19.56
	..........
	.....
	-----------------------------------

  getdnsname.c ->bin/getdnsname	取得IP的DNS域名
  convertmask.c	把从http://www.nic.edu.cn/RS/ipstat/internalip/
                取得的免费IP定义列表更新到$parker/HostData/Edu.mask	
  
  common.c	得到ftp的域名等信息等一些共同的函数
  log.c		纪录搜索的历史, 放在 log/, 由search.c 调用
  search.c	搜索用的cgi       -<LZOData/*
  statist.c	显示统计信息的cgi  -<Statist/*
  submit.c	提交新ftp站点的cgi   ->etc/CollectList
  
  minilzo.h	miniLZO压缩函数库的头文件
  minilzo.c	miniLZO压缩函数库的主文件
  lzoconf.h
  
  lzo_comp.c	调用miniLZO压缩函数库的目录数据压缩器
  lzo_fun.c	调用miniLZO压缩函数库的解压缩函数,供search.c调用

etc/:
  CollectList	FTP服务器列表文件

LZOData/:
  只存放搜集到的目录数据, 以ip为文件名, 以压缩格式存放.

HostData/:
  存放站点的信息, ip.info为相应的域名，
  $IP.stat	各个FTP的统计数据,最后集合到stat
  stat		统计的原始数据
  其他		按各个不同要求排序的结果
  Inn.mask	为南京大学内部网络的IP定义,
  Nju.mask	为南京大学正式IP的定义.
  Edu.mask	为教育网的IP定义(免费)

log/:
  search.log	用户搜索的完整纪录
  searchkey.log	搜索的关键字纪录
  submit.log	用户推荐站点的纪录

tmp/:
  工作目录

############################################################################# 
#   修改列表 
############################################################################# 
4.0 (May-8-2001) 
(1)Support multi keywords, exclusion and filename extention. 
(2)FTP site Statistic, include number of files, sepcial archives like .rm,.mp3, 
  Linux files(tar, gz, bz2),and show the last updata time. 
(3)show the updata time at each site when shows the search result. 
(4)sort the FTP sites, make the nearest site to the client as the first site. according to IP. 
(5)remove many useless features, include interface of telnet,gopher and dos utils. 
(6)remove Btree, less memory request, less harddisk space. can contain more site, more data. 
    如果使用Btree，当处理超过100M原始数据使需要超过250M的内存，在达到150个站点后有很多服务器会无法处理数据。
(7)在线推荐FTP站点, detect the site IP, 
  DNS name and scope. and collect site data automaticly.

4.1 (May-20-2001) 
(1)使用LZO压缩算法保存目录数据，节省硬盘空间，大大提高了搜索速度。
   原来100M数据压缩成30M左右，可以自动被放在缓冲中，搜索就不会受到硬盘速度的限制。
(2)优化搜索的关键代码，大大提高了搜索速度。服务器用PIII-450，搜索100个站点数据只用了4秒。

4.2 (FEB-16-2002) 
(1)把用perl语言写的搜集数据部分用C语言重写，使其
   能够运行在新的Linux版本之上。包括Redhat7.x, Mandrake 8.x
(2)在搜索时自动排除太久没有更新的站点；
(3)Multi user language supported:English and Chinese 
(4)修正了许多bug
(5)作了一些清除代码的工作，使其更加的简洁；
(6)搜索列表恢复域名支持, 当然也可以用ip

############################################################################# 
#   运行平台
############################################################################# 
4.0 Redhat Linux 6.2, PIII-800
4.2 Redhat Linux 7.2,Mandrake Linux 8.1, Celeron 900, PIII-900 
经过了对Linux ws-ftpd, Windows NT Server的测试


steff
2002.3.10
