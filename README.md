parker
=====

An open source FTP Search engine

Thanks to Nanjing University and all the contributors.
Lisence: GPL v2.0
Original project on sourceForge, i copy them to github.

Version 5.0 Last edited: 2013-10-29 by lijingpeng
1.�޸�Makefile��ִ��make install����ֱ�ӽ���ִ���ļ���װ��bin��/var/www/cgi-bin������ҳ��װ��/var/www/parker
2.ִ��make realcleanɾ����ʱ�򽫻�ֱ��ɾ��bin�ļ�����ҳ�ļ�����ø��ӳ���
3.���apache�����ļ��û���Ҫ��parker_apache������apache�������ļ��£�������apache

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
*           Copyright (c)2001-2002 by steff( ���� )
*           Nanjing University, China, 210093
*                  All rights reserved.
*
**************************************************************

    Parker��һ��Linux��ftp�����������������ԭ����Jiri A. Randus��
д���󾭹��ڸ��ֶ�θĽ�������汾������������ĺ������Ͼ��������޸�
�õ��ġ�����Ŀǰ��������FTP���ǳ���ʵ�ã����ռ���Ѱ�Ҹ���FTP�Ƿǳ���
Ҫ�ģ�ϣ�����������ܹ�����Ҫ�����Ѵ���һЩ���㡣
    �ڴ���Ҫ��л�������Ұ����������ǣ���ʶ�ġ�����ʶ�ģ������ǵĹ���
�Ұ�������һֱ��������
    ���������ѭGPL 2.0Э��, �κ��˿���ʹ��,�޸�,����,�õ�Դ���롣
Ϊ���ܹ����ϴ��һ�𿪷�Parker, ����sourceForge��������һ��Project.
��ӭ����Ȥ����һ������Parker���ĸ��á�

Author: steff
E-mail: mesolinux@263.net
Home  : http://sourceforge.net/projects/parker
CVS   : cvs.parker.sourceforge.net

***************************************************************
*  ��װָ�ϣ�
***************************************************************
1 ����ѱ䶯�����ݷ���/var�¡�
  ��root��½��
  �⿪�ļ���(����Ѿ��⿪,���Կ�����ȥ)

	# cd /var
	# tar vxzf parker-200x-xx-xx.tar.gz
	# ln -s parker-200x-xx-xx parker

  �����web�������(��apache)��cgi-binĿ¼�½���parkerĿ¼

	# mkdir /var/www/cgi-bin/parker

  ��parker�����ҳ�������ݸ��Ƶ����web����������ӦĿ¼(��apache
  ��DocumentRoot��ָ��λ�ã�

	# cp /var/parker-200x-x-xx/html/parker /var/www/html/parker -r

2 �趨�����ļ��Ķ�дȨ��

	# chown nobody.nobody /var/parker-200x-xx-xx -R
	# chmod 770 /var/parker-200x-xx-xx -R

   nobody��cgi��ִ���û�.

3 �޸�Դ�����ļ�·����
  (����ı䰲װĿ¼�Ļ�)(Ĭ��Ϊ/var/parker)

  src/Makefile		PARKER_HOME= /var/parker            (��װĿ¼)
			CGI_HOME= /var/www/cgi-bin/parker (cgiĿ¼)

4 ����ϲ���޸�cgi�������
  src/cgi*		������Ҫ�޸ģ���Ҫ�޸�ҳ���������
  			��Ҫ���������
		370	PutHeaders(void)
		386	Search(void)
		494	void PutEnd(void)
  ������ʾ�����ļ�  src/language.h 
5 ���±������
	#cd src
	#make
	#make install
  
  Ȼ����԰Ѹ������Ŀ¼��Ȩ���ټ��һ�¡�
  
6 ����crontab���Ա㶨ʱ�Զ���������
  crontab -u parker -e
   1 1 * * * /var/parker/bin/flashdata %(������ÿ�����)

7 �༭վ���б��ļ� /var/parker/etc/CollectList 

8 ִ���Ѽ����ݵ�����:
  [root@linux /var/parker/bin]$ ./flashdata &

9 ������������
  html�����ṩ��ҳ������.
  http://localhost/Parker/index.html
***************************************************************
*  �����Ǹ����ļ���Ŀ¼�����úͰ���:
***************************************************************
bin/:
  collect [-l|-r|-d] [ftp site]
		�Ѽ�ftpվ�������õ�, ftpվ�㶨���� etc/CollectList
                ����ѹ������ڷ���LZOData��. ��ʱ����Ŀ¼�� tmp/, վ���ͳ�����ݷ��� Statist/��
		����ʹ������: 	-l ʹ��LIST -lR�����г��������ϵ�Ŀ¼��
				-d ͨ������������ ls-lR.tgz���ļ������Ŀ¼�����еķ������ϻᶨʱ
					�����������ļ���
				-r �Զ����������Ŀ¼�õ�����Ŀ¼���б�
				Ĭ�ϵĵ���˳���� -l,-d,-r ����etc/CollectList�е�ftpվ��

  flashdata	�������ݵĵ����ļ�.����collect��makestat

  convertmask	�Ѵ�http://www.nic.edu.cn/RS/ipstat/internalip/
		ȡ�õĽ��������IP�����б���µ�$parker/HostData/Edu.mask
		Ҫ�������ļ���ʽ����:
                host	subnet	netmask
	��	202.119.32.0	0.0.31.255	255.255.224.0
		ʹ�÷������追���������ļ�Ϊlist.txt, ִ��convertmask list.txt

  flashlist	һ��ʱ���,�Ѻܴ��$parker/etc/onworkhostһЩ�޷������
		IPȥ��, �����ļ���$parker/etc/backlist

  listfilter	���Ѽ�����ftpվ���б�ת��onworkhost

  getdnsname <ip>
		����ip��DNS����. û�еĻ�,���ؿ���.

  makestat	��ftpվ���ϵĸ����ļ�����ͳ������, ��Statistʹ�ã����Է�����ͬվ��Բ�ͬ�ļ���ƫ�á�

  lzo_comp	��Ŀ¼������LZO�㷨����ѹ���ĳ���. ��ȡ�Ĳ�����searchֱ�ӽ��С�

src/:
  Makefile	make�����ļ�
  config.h	Ŀ¼,�ļ���Ŀ¼�Ķ���, makeʱ��conf.head��conf.end����
  conf.head
  conf.end
  parker.h	ͷ�ļ�,��������Ķ���, 

  collect.c	�Ѽ�ftpվ�����ݵģô���Դ����
  collect.h	ͷ�ļ�
  listfilter.c	�ܹ��Ƚ��Զ���һЩftpվ���б��ļ��г�ȡ��ip, �����ļ�oldlist.txt����:
        ---------------------------------------
        �ϴ�FTP: 202.119.32.32
        ����FTP: xxx.xxx.xxx.xx     "good"
        202.119.46.57
        �廪 166.111.19.56
            ..........
            ........
        ---------------------------------------
	ʹ������"listfilter oldlist", ���Եõ�
	-----------------------------------
	202.119.32.101
	xxx.xxx.xxx.xx
	166.111.19.56
	..........
	.....
	-----------------------------------

  getdnsname.c ->bin/getdnsname	ȡ��IP��DNS����
  convertmask.c	�Ѵ�http://www.nic.edu.cn/RS/ipstat/internalip/
                ȡ�õ����IP�����б���µ�$parker/HostData/Edu.mask	
  
  common.c	�õ�ftp����������Ϣ��һЩ��ͬ�ĺ���
  log.c		��¼��������ʷ, ���� log/, ��search.c ����
  search.c	�����õ�cgi       -<LZOData/*
  statist.c	��ʾͳ����Ϣ��cgi  -<Statist/*
  submit.c	�ύ��ftpվ���cgi   ->etc/CollectList
  
  minilzo.h	miniLZOѹ���������ͷ�ļ�
  minilzo.c	miniLZOѹ������������ļ�
  lzoconf.h
  
  lzo_comp.c	����miniLZOѹ���������Ŀ¼����ѹ����
  lzo_fun.c	����miniLZOѹ��������Ľ�ѹ������,��search.c����

etc/:
  CollectList	FTP�������б��ļ�

LZOData/:
  ֻ����Ѽ�����Ŀ¼����, ��ipΪ�ļ���, ��ѹ����ʽ���.

HostData/:
  ���վ�����Ϣ, ip.infoΪ��Ӧ��������
  $IP.stat	����FTP��ͳ������,��󼯺ϵ�stat
  stat		ͳ�Ƶ�ԭʼ����
  ����		��������ͬҪ������Ľ��
  Inn.mask	Ϊ�Ͼ���ѧ�ڲ������IP����,
  Nju.mask	Ϊ�Ͼ���ѧ��ʽIP�Ķ���.
  Edu.mask	Ϊ��������IP����(���)

log/:
  search.log	�û�������������¼
  searchkey.log	�����Ĺؼ��ּ�¼
  submit.log	�û��Ƽ�վ��ļ�¼

tmp/:
  ����Ŀ¼

############################################################################# 
#   �޸��б� 
############################################################################# 
4.0 (May-8-2001) 
(1)Support multi keywords, exclusion and filename extention. 
(2)FTP site Statistic, include number of files, sepcial archives like .rm,.mp3, 
  Linux files(tar, gz, bz2),and show the last updata time. 
(3)show the updata time at each site when shows the search result. 
(4)sort the FTP sites, make the nearest site to the client as the first site. according to IP. 
(5)remove many useless features, include interface of telnet,gopher and dos utils. 
(6)remove Btree, less memory request, less harddisk space. can contain more site, more data. 
    ���ʹ��Btree����������100Mԭʼ����ʹ��Ҫ����250M���ڴ棬�ڴﵽ150��վ����кܶ���������޷��������ݡ�
(7)�����Ƽ�FTPվ��, detect the site IP, 
  DNS name and scope. and collect site data automaticly.

4.1 (May-20-2001) 
(1)ʹ��LZOѹ���㷨����Ŀ¼���ݣ���ʡӲ�̿ռ䣬�������������ٶȡ�
   ԭ��100M����ѹ����30M���ң������Զ������ڻ����У������Ͳ����ܵ�Ӳ���ٶȵ����ơ�
(2)�Ż������Ĺؼ����룬�������������ٶȡ���������PIII-450������100��վ������ֻ����4�롣

4.2 (FEB-16-2002) 
(1)����perl����д���Ѽ����ݲ�����C������д��ʹ��
   �ܹ��������µ�Linux�汾֮�ϡ�����Redhat7.x, Mandrake 8.x
(2)������ʱ�Զ��ų�̫��û�и��µ�վ�㣻
(3)Multi user language supported:English and Chinese 
(4)���������bug
(5)����һЩ�������Ĺ�����ʹ����ӵļ�ࣻ
(6)�����б�ָ�����֧��, ��ȻҲ������ip

############################################################################# 
#   ����ƽ̨
############################################################################# 
4.0 Redhat Linux 6.2, PIII-800
4.2 Redhat Linux 7.2,Mandrake Linux 8.1, Celeron 900, PIII-900 
�����˶�Linux ws-ftpd, Windows NT Server�Ĳ���


steff
2002.3.10
