#!/bin/bash

# Some of the regex's used in sed
# Catch basic IP6 address   "s/\([0-9a-fA-F]\{1,4\}:\)\{7,7\}[0-9a-fA-F]\{1,4\}/y.y.y.y.y.y.y.y/g"
# Catch y::y.y.y.y          "s/[0-9a-fA-F]\{1,4\}:\(:[0-9a-fA-F]\{1,4\}\)\{1,4\}/y::y.y.y.y/g"
# IP4 d.d.d.d decimal	    "s/\([0-9]\{1,3\}\.\)\{3,3\}[0-9]\{1,3\}/x.x.x.x/g"
# mac address	            "s/\([0-9a-fA-F]\{2,2\}\:\)\{5,5\}[0-9a-fA-F]\{2,2\}/m.m.m.m/g"


display_info_drm() {
   # If running X then can use xrandr, otherwise
   # dump the /sys/class entries for the displays
   if command -v xrandr > /dev/null &&
      DISPLAY=${DISPLAY:-:0} xrandr --listmonitors &>/dev/null;
   then
      echo "Running (F)KMS and X"
      echo

      DISPLAY=${DISPLAY:-:0} xrandr --verbose
   else
      echo "Running (F)KMS, console"
      echo

      for card in /sys/class/drm/card[0-9]-*;
      do
         echo $card

         # if kmsprint isn't installed print basic mode info
         if ! command -v kmsprint > /dev/null; then
            if [ -f $card/modes ];
            then
               cat $card/modes
            else
               echo "No modes found"
            fi
         fi

         if [ -f $card/edid ];
         then
            base64 $card/edid
         else
            echo "No EDID found"
         fi
         echo
      done
   fi

   # kmsprint is more useful, but not always installed
   echo
   if command -v kmsprint > /dev/null; then
      kmsprint
      echo
      kmsprint -m
   else
      echo "kmsprint is not installed. Install with: sudo apt install kms++-utils"
   fi

   echo

   # dump the /sys/class entries for the displays
   cardfound=0
   for card in `seq 0 9`;
   do
      if sudo test -f "/sys/kernel/debug/dri/${card}/state";
      then
        for hdmi in 0 1;
        do
         if sudo test -f "/sys/kernel/debug/dri/${card}/hdmi${hdmi}_regs";
         then
            echo "HDMI${hdmi}: $(sudo cat /sys/kernel/debug/dri/$card/hdmi${hdmi}_regs | grep HOTPLUG)"
         fi
      done
      echo
      echo "/sys/kernel/debug/dri/$card/state:"
      sudo cat "/sys/kernel/debug/dri/$card/state"
      echo
      cardfound=1
      fi
   done
   if [ "$cardfound" == "0" ];
   then
      echo "kms state not found"
   fi
   echo

}

display_info_legacy() {
   # Legacy mode
   echo "Running Legacy framebuffer"
   echo

   for card in `seq 0 9`;
   do
      F="/dev/fb${card}"
      if test -e $F;
      then
         echo Framebuffer: $F
         fbset -s -fb $F
      fi
   done

   disps=`tvservice -l | awk '/Display Number/{print substr($3,1,1)}'`

   tmp=$(mktemp)

   for display in $disps
   do
      echo
      echo "Display: " $display

      tvservice -v $display -s
      tvservice -v $display -n
      tvservice -v $display -m CEA
      tvservice -v $display -m DMT

      echo
      tvservice -v $display -d $tmp > /dev/null
      base64 $tmp
   done

   rm $tmp
}

display_info() {
   # Check if we are running a KMS/DRM system

   if [ -d "/dev/dri" ];
   then
      display_info_drm
   else
      display_info_legacy
   fi
}

OUT=raspinfo.txt

rm -f $OUT

exec > >(tee -ia $OUT)

echo "System Information"
echo "------------------"
echo

cat /sys/firmware/devicetree/base/model | sed 's/\x0//g'
echo

cat /etc/os-release | head -4
echo

cat /etc/rpi-issue
echo
uname -a

cat /proc/cpuinfo | tail -3

echo "Throttled flag  : "`vcgencmd get_throttled`
echo "Camera          : "`vcgencmd get_camera`

echo
echo "Videocore information"
echo "---------------------"
echo

vcgencmd version
echo
vcgencmd mem_reloc_stats

echo
echo "Filesystem information"
echo "----------------------"

df
echo
cat /proc/swaps

echo
echo "Package version information"
echo "---------------------------"

apt-cache policy raspberrypi-ui-mods | head -2
apt-cache policy raspberrypi-sys-mods | head -2
apt-cache policy openbox | head -2
apt-cache policy lxpanel | head -2
apt-cache policy pcmanfm | head -2
apt-cache policy rpd-plym-splash | head -2

echo
echo "Networking Information"
echo "----------------------"
echo

ifconfig | sed -e "s/\([0-9a-fA-F]\{1,4\}:\)\{7,7\}[0-9a-fA-F]\{1,4\}/y.y.y.y.y.y.y.y/g" | sed -e "s/[0-9a-fA-F]\{1,4\}:\(:[0-9a-fA-F]\{1,4\}\)\{1,4\}/y::y.y.y.y/g" | sed -e "s/\([0-9]\{1,3\}\.\)\{3,3\}[0-9]\{1,3\}/x.x.x.x/g" | sed -e "s/\([0-9a-fA-F]\{2,2\}\:\)\{5,5\}[0-9a-fA-F]\{2,2\}/m.m.m.m/g"

echo
echo "USB Information"
echo "---------------"
echo

lsusb -t

echo
echo "Display Information"
echo "-------------------"
echo

display_info

echo
echo "config.txt"
echo "----------"
echo

#cat /boot/config.txt | egrep -v "^\s*(#|^$)"
vcgencmd get_config int
vcgencmd get_config str


echo
echo "cmdline.txt"
echo "-----------"

cat /proc/cmdline

echo
echo "raspi-gpio settings"
echo "-------------------"
echo

raspi-gpio get

echo
echo "vcdbg log messages"
echo "------------------"
echo

if command -v vcdbg > /dev/null; then
   sudo vcdbg log msg 2>&1
elif command -v vclog > /dev/null; then
   sudo vclog --msg 2>&1
else
   echo "vcdbg not found"
fi

echo
echo "dmesg log"
echo "---------"
echo

dmesg | sed -e "s/\([0-9a-fA-F]\{1,4\}:\)\{7,7\}[0-9a-fA-F]\{1,4\}/y.y.y.y.y.y.y.y/g" | sed -e "s/[0-9a-fA-F]\{1,4\}:\(:[0-9a-fA-F]\{1,4\}\)\{1,4\}/y::y.y.y.y/g" | sed -e "s/\([0-9a-fA-F]\{2,2\}\:\)\{5,5\}[0-9a-fA-F]\{2,2\}/m.m.m.m/g"


if  grep -q "^Revision\s*:\s*[ 123][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F]11[0-9a-fA-F]$" /proc/cpuinfo
then
echo
echo "EEPROM"
echo "------"
echo
sudo rpi-eeprom-update
fi
