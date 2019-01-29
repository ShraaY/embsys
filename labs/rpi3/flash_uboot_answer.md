# Partie 3: Flashage de la carte et Bootloader

Dans cette partie, nous allons voir comment flasher l'OS sur la carte ainsi
qu'étudier le bootloader U-Boot.

### Préliminaires

Ce TP est à réaliser à partir de la même image Docker que la
[Partie 1](buildroot.md):


````
$ docker run -it embsys:rpi3-buildroot /bin/bash
# cd /root/
# tar zxvf buildroot-2017.08.tar.gz
# cd buildroot-2017.08
````

### Flashage

Avec le résultat de la compilation de Buildroot, il y a 2 possibilités pour
flasher l'OS sur la carte SD cible:
- mode manuel: partitionnement de la carte, copie du bootloader, ...
- mode "automatique": 1 seule commande à exécuter, rien à comprendre

Le mode automatique est évidemment le plus simple. Cependant, si vous souhaitez
configurer finement le comportement de votre RPI3 (par exemple utiliser un
bootloader, spécifique), il est indispensable de comprendre le mode manuel.

De plus, le mode automatique n'est possible que grâce à l'écriture de scripts
reprenant les étapes du flashage manuel.

Nous allons donc étudier les 2 modes durant ce TP.

#### Automatique

Tout d'abord récupérer l'image complet de la carte SD à partir du conteneur
Docker sur votre machine hôte:

```` shell
xxx = docker  ps pour trouver le nom
$ sudo docker cp xxxxxxxxxxxx:/root/buildroot-2017.08/output/images/sdcard.img .
````

Ensuite, sur une carte SD disponible à travers */dev/sdX* (remplacer *X* par le
path de votre carte. *dmesg* peut vous aider):

```` shell là ou mit le sdcard.img
$ sudo dd if=sdcard.img of=/dev/sdX
````
Ports Tx et Rx?
 : GPIO 14 et 15: ports 4 et 5 externe opposé des prises usb. Le Tx/14 doit etre de couleur blanche car entrée, Rx/15 vert, masse sur le 3ème port (même sens). tant que on veut recuperer les donnees, pas brancher fil rouge (1er port, 5v)
Insérer la carte SD dans la RPI3 et brancher les adaptateurs USB-TTL
correctement (cf datasheet de la carte). Utiliser un terminal série (minicom ou sudo gtkterm) pour établir une communication avec la carte.

lancer gtkterm, brancher et debrancher l'alim' jusqu'a que s'afficeh du text en changeant le baudraet( config port) et en gardant tty/usb0, plus grand baud 115200
8n1

user -1 users_group -1 =user1* /home/user /bin/sh -
   ->  login : user, psw = user1*
###"dmesg | grep eth0" pour adresse ip : b8ssh: connect to host 172.20.21.159 port 22: Connection refused:27:eb:96:9d:4e
"ifconfig" : ip = 172.20.21.159

login ordif004 : ufpws ordi f004: linux
sur ordi: ssh user@172.20.21.159 ou ssh root@172.20.21.159
root refuse la connection par sécurité (peux aussi changer le port22 pour plus de securtié). C'est configuré dans un fichier "/etc/ssh/sshd_config"
/!\ pb: connection refusé -> réseau école "ssh: connect to host 172.20.21.159 port 22: Connection refused"



#### Manuel

Pour flasher la carte manuellement, les étapes suivantes sont à réaliser dans
l'ordre à partir de la machine hôte:

1- Nettoyage
2- Partitionnement
3- Formattage
4- Copie du bootloader et de l'image kernel (entre autre) sur la 1ère partition
5- Extraction du RFS sur la 2ème partition

Tout d'abord, sur une carte SD disponible à travers */dev/sdX* (remplacer *X*
par le path de votre carte. *dmesg* peut vous aider), il faut nettoyer la
carte SD:

```` shell
$ dd if=/dev/zero of=/dev/sdX
````

Ensuite, informez vous sur l'utilisation de la commande *fdisk* afin de
partitionner la carte SD tel que:
- 1ère partition de 128M ( *+128M* ) avec flag de boot (commande *a*) et type W95 FAT32 (LBA) (commande *t* puis code *c*)
- 2ème partition avec le reste et la config par défaut

Puis, formattez les partitions:

```` shell
$ sudo mkfs.vfat /dev/sdX1
$ sudo mkfs.ext4 /dev/sdX2
````

En utilisant la commande *fdisk* et la commande *p* de son interpreteur pour
afficher la configuration de la carte, vous devriez alors obtenir ceci si vous
avez réalisé les étapes précédentes correctement:

```` shell
$ fdisk /dev/sdX
Command (m for help): p
Disk /dev/sdX: 14.9 GiB, 15931539456 bytes, 31116288 sectors
Units: sectors of 1 * 512 = 512 bytes
Sector size (logical/physical): 512 bytes / 512 bytes
I/O size (minimum/optimal): 512 bytes / 512 bytes
Disklabel type: dos
Disk identifier: 0x00000000

Device     Boot Start    End Sectors Size Id Type
/dev/sdX1  *        1  65536   65536  32M  c W95 FAT32 (LBA)
/dev/sdX2       65537 188416  122880  60M 83 Linux

Command (m for help):
$ lsblk -fs
NAME  FSTYPE LABEL     UUID                                 MOUNTPOINT
sdX1  vfat   boot      7794-9F86
└─sdX
sdX2  ext4   root      7e0c7c79-a446-46bc-b103-aeebc167ca13
└─sdX
````

Intéressons-nous maintenant au RFS. Pour information, Buildroot ne génère
pas de tarball de rootfs par défaut. Il faut reconfigurer le build
(*make menuconfig*) et activer la construction d'un tar. Ensuite recompiler
avec *make* et récupérer la tarball *rootfs.tar* sur la machine hôte.

Préparer manuellement la carte SD (vous aurez besoin de récupérer en local
quelques fichiers générés par buildroot):

```` shell
$ mkdir /media/sd
$ mount /dev/sdX1 /media/sd
$ cp bcm2710-rpi-3-b.dtb /media/sd
$ cp bcm2710-rpi-cm3.dtb /media/sd
$ cp -r rpi-firmware/* /media/sd
$ cp zImage /media/sd
$ umount /media/sd
$
$ mount /dev/sdX2 /media/sd
$ tar xf rootfs.tar -C /media/sd
$ umount /media/sd
````

Démarrer la RPI3 et établir une communication série (via des outils comme
minicom, gtkterm, putty, cu, ...).

### U-Boot

Bootloader industriel. Configurer dans Buildroot les options du bootloader:
- Build System: KCONFIG
- Board defconfig: rpi_3_32b (regarder dans le répertoire configs du code uboot ds build)
- cocher *U-Boot needs dtc*
- mkimage pour host

Recompiler le tout avec *make*.

Ensuite créer un nouveau fichier *boot.source* contenant:

````
fatload mmc 0:1 ${kernel_addr_r} zImage
fatload mmc 0:1 ${fdt_addr_r} bcm2710-rpi-3-b.dtb
setenv bootargs earlyprintk dwc_otg.lpm_enable=0 console=ttyAMA0,115200 console=tty1 root=/dev/mmcblk0p2 rootfstype=ext4 elevator=noop noinitrd rw rootwait
bootz ${kernel_addr_r} - ${fdt_addr_r}
````

Et compiler ce fichier avec *mkimage*:

````
root@xxxxxxxxxxxx:/# ./output/host/bin/mkimage -A arm -O linux -T script -C none -a 0x00000000 -e 0x00000000 -n boot.scr -d boot.source  boot.scr
````

Ensuite copier le binaire *u-boot.bin*, le *fichier boot.scr* et le fichier
*config.txt* suivant dans la 1ère partition de la carte SD:

````
kernel=u-boot.bin
enable_uart=1
dtoverlay=pi3-disable-bt
````

cmd version -> print monitor, compiler and linker version
    donne : U-Boot 2017.07 (Dec 16 2018 - 15:35:35 +0000)
            arm-buildroot-linux-uclibcgnueabihf-gcc.br_real (Buildroot 2017.08) 6.4.0
            GNU ld (GNU Binutils) 2.28.1



Tester la configuration.
