#!/bin/bash

HEIGHT=15
WIDTH=40
CHOICE_HEIGHT=4
BACKTITLE="ESP32 target selection"
TITLE="ESP32 target selection"
MENU="Choose one of the following options:"

OPTIONS=(1 "NNC Badge 2019"
         2 "M5StickC"
         3 "TTGO (custom)")

CHOICE=$(dialog --clear \
                --backtitle "$BACKTITLE" \
                --title "$TITLE" \
                --menu "$MENU" \
                $HEIGHT $WIDTH $CHOICE_HEIGHT \
                "${OPTIONS[@]}" \
                2>&1 >/dev/tty)

clear
rm sdkconfig
rm sdkconfig.defaults
rm -r build/

case $CHOICE in
        1)
            cp sdkconfig.defaults.nnc2019 sdkconfig.defaults
            ;;
        2)
            cp sdkconfig.defaults.m5stickc sdkconfig.defaults
            ;;
        3)
            cp sdkconfig.defaults.ttgo sdkconfig.defaults
            ;;
esac

make menuconfig