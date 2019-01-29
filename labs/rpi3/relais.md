# Relais

Numérotation des pins pour la RPi3 Model B v1.2:

<p align="center">
  <img src="https://github.com/pblottiere/embsys/blob/master/labs/rpi3/imgs/rpi3pins.png" width="550" title="Github Logo">
</p>

# Contenu

  * [GPIO et LED](#gpio-et-led)
    * [Sysfs](#sysfs)
    * [Python](#python)
    * [PWM](#pwm)
  * [Relais](#relais)
    * [Code C](#c)

## GPIO et LED

### Sysfs

Dans l'espace utilisateur, les GPIO sont accessible via le système de fichier
dans **/sys/class/gpio**. Cette option est disponible seulement si on a installé
la lib **libsysfs** au moment de la compilation avec Buildroot.

Réaliser le montage suivant en utilisant la GPIO 17 (n'oubliez pas que la
broche la plus courte de la LED est le **-**):

<p align="center">
  <img src="https://github.com/pblottiere/embsys/blob/master/labs/rpi3/imgs/led_res.jpg" width="350" title="Github Logo">
</p>

Grâce à la documentation du kernel sur
[sysfs](https://www.kernel.org/doc/Documentation/gpio/sysfs.txt), allumer et
éteindre la LED connectée à la GPIO 17.

ground led sur pin 9 (en face du vert, 5 en partant de l'opposé des ports usb, en interne) et l'entrée sur port 11 (/!\ sur le pin gpio 17, 1 port après le ground en interne)
ds gpio
echo 17 > export pour ajouter un port 11/gpio 17
echo 17 > unexport pour enlever un port
echo "out" > ds gpio 11 : direction pour en faire une sortie
echo 1 > value pour allumer led
echo 0 > value pour entiendre led

### Python

Il existe aussi un paquet python pour la RPi. Ce paquet, installé via
buildroot, est nommé **python-rpi-gpio**. La documentation est
[ici](https://sourceforge.net/p/raspberry-gpio-python/wiki/Home/).

Utiliser cette API Python pour allumer/éteindre la LED connectée à la GPIO 17
(faire attention au mode BCM ou BOARD).

reflasher la carte pour avoir python, relancer le gtkterm
ds /root touch led.py, vi led.py avec 
import RPi.GPIO as GPIO # Import Raspberry Pi GPIO library
from time import sleep # Import the sleep function from the time module
GPIO.setwarnings(False) # Ignore warning for now
GPIO.setmode(GPIO.BOARD) # Use physical pin numbering
GPIO.setup(11, GPIO.OUT, initial=GPIO.LOW) # Set pin 8 to be an output pin and set initial value to low (off)
while True: # Run forever
	GPIO.output(11, GPIO.HIGH) # Turn on
	sleep(1) # Sleep for 1 second
	GPIO.output(11, GPIO.LOW) # Turn off
	sleep(1) # Sleep for 1 second
python led.py



### PWM

Le PWM, ou Pulse Mith Modulation, permet	 de simuler un signal continu en
moyenne grâce à des signaux discrets (de valeur constante fixe). En image,
cela donne ceci :

<p align="center">
  <img src="https://github.com/pblottiere/embsys/blob/master/labs/rpi3/imgs/pwm.png" width="500" title="Github Logo">
</p>

Deux paramètres permettent donc de gérer la tension moyenne souhaitée :
- le *duty_cycle*
- la *period*

Dans l'espace utilisateur, la logique d'utilisation est la même que pour les
GPIO : on passe par des fichiers virtuels de /sys/class pour le contrôle.

La carte RPi3 possède un connecteur J8 où se trouve des pins PWM. Ici, nous
allons utiliser la GPIO 1 (pin 12).

Pour faire du PWM via sysfs, voici la démarche théorique (théorique car
pour une raison qui m'échappe encore, le fichier
 **/sys/class/pwm/pwmchip0** n'est pas créé par le kernel: inspecter les
overlays):

```` shell
$ cd /sys/class/pwm/
$ ls
pwmchip0
$ export PWM_CHIP=pwmchip0
$ export PWM=12
$ cd pwmchip0/
$ ls
device     export     npwm       power      subsystem  uevent     unexport
$ echo $PWM > /sys/class/pwm/$PWM_CHIP/export
$ ls
device     npwm       pwm4       uevent
export     power      subsystem  unexport
$ echo 1 > /sys/class/pwm/$PWM_CHIP/pwm$PWM/enable
$ echo 50000 > /sys/class/pwm/$PWM_CHIP/pwm$PWM/duty_cycle
$ echo 100000 > /sys/class/pwm/$PWM_CHIP/pwm$PWM/period
````

Par contre nous pouvons utiliser l'API Python sans encombre:

```` python
# python
Python 2.7.13 (default, Jan  7 2018, 19:42:20)
[GCC 6.4.0] on linux2
Type "help", "copyright", "credits" or "license" for more information.
>>> import RPi.GPIO as GPIO
>>> GPIO.setmode(GPIO.BOARD)
>>> GPIO.setup(12, GPIO.OUT)
>>> p = GPIO.PWM(12, 0.5)
>>> p.start(1)
>>> p.stop()
````

Modifier la fréquence et le duty_cycle pour faire varier la luminosité de votre
LED.

## Relais

L'intérêt d'un relai est de contrôler un appareil allimenté en 230V (ou 5V,
12V, etc) à partir d'un GPIO. Typiquement, ouvrir une porte de garage!

Réaliser le montage suivant avec:
- les pins PWR/GND pour remplacer la pise 220V
- une LED à la place du réveil

<p align="center">
  <img src="https://github.com/pblottiere/embsys/blob/master/labs/rpi3/imgs/relai.png" width="350" title="Github Logo">
</p>

Tester l'allumage de la LED via le contrôle de la GPIO par Sysfs et Python.

### Code C

Écrire du code C dans l'espace utilisateur (via sysfs donc) en mode
client/serveur (socket) pour gérer l'état du relai.

Ensuite, cross-compiler, uploader vos binaire sur la RPi3 et configurer Domoticz
pour pouvoir changer l'état du  relai en cliquant dans l'interface grâce à vos
programmes client/serveur.
