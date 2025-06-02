[![GPLv3 License](https://img.shields.io/badge/License-GPL%20v3-yellow.svg)](https://opensource.org/licenses/)
![Repo Stars](https://img.shields.io/github/stars/AcronixOS-Inc/codename-speedster)
![Repo Forks](https://img.shields.io/github/forks/AcronixOS-Inc/codename-speedster)
![Releases](https://img.shields.io/github/v/release/AcronixOS-Inc/codename-speedster)


<h3 align="center">codename-speedster</h3>

<h3 align="center">
  <img width="800" style="border-radius:5px;" alt="screenshot" src="screenshots/image.png">
</h3>
<h3 align="center">
  <img width="800" style="border-radius:5px;" alt="screenshot" src="screenshots/unstable.png">
</h3>

# codename-speedster (unstable)

codename-speedster — это современное экспериментальное ядро операционной системы, разрабатываемое с целью исследования низкоуровневых технологий, обучения системному программированию и тестирования новых концепций в области ОС
## Быстрый старт

```
git clone https://github.com/AcronixOS-INC/codename-speedster.git
cd codename-speedster
make all
make run
```
## Зависимости

```
# Debian/Ubuntu
sudo apt-get install build-essential nasm gcc-multilib qemu-system-x86 gdb
```

```
# Arch Linux
sudo pacman -S base-devel nasm qemu gdb multilib-gcc
```

```
# Fedora/RHEL
sudo dnf install @development-tools nasm qemu gdb
```
## Лицензия

Этот репозиторий, включая все его содержимое, подпроекты, модули и компоненты, лицензирован в соответствии с [GNU GPL v3](https://www.gnu.org/licenses/gpl-3.0.txt), если явно не указано иное. Сторонние библиотеки, включенные в этот репозиторий, могут подчиняться их собственным лицензиям.
