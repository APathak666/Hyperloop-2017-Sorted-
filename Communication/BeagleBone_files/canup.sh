sudo modprobe can
sudo modprobe can-dev
sudo modprobe can-raw
echo BB-DCAN1 > /sys/devices/bone_capemgr.9/slots
echo BB-DCAN0 > /sys/devices/bone_capemgr.9/slots
sudo ip link set can0 up type can bitrate 1000000
sudo ip link set can1 up type can bitrate 1000000
